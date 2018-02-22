local bin_anonymous = {}
local assertx = require 'assertx'
local consts = require 'ab_consts'
local ffi = require 'ab_ffi'
local spooky_hash = require 'spooky_hash'

local function set_variants_per_bin(bin, dev_variants, var_id_to_index_map)

  local total = 0
  local num_set = 0
  local total_percentage = 0i
  -- In case the bins dont fill up
  ffi.fill(bin, consts,AB_NUM_BINS, var_id_to_index_map[dev_variants[0].id])

  for index, variant in ipairs(dev_variants) do
    local percent = assert(tonumber(variant.percentage), "Variant must have a valid percentage")
    local total_bins = math.floor(percent * 0.01 * consts.AB_NUM_BINS)
    assert(num_set + total_bins <= consts.AB_NUM_BINS, "Total number of bins should be less that total bins")
    for set_vals=num_set, num_set + total_bins - 1 do
      bin[set_vals] = var_id_to_index_map[variant.id]
    end
    total = total + total_bins
    total_percentage = total_percentage + percent
  end
  assert(total <= consts.AB_NUM_BINS, "More than max bins cannot be occupied")
  assert(total_percentage == 100, "Total percentage should add up to 100 percent")
end

local function get_all_variants(json_table)
  local variants = {}
  for device_name ,variants_table  in ipairs(json_table.DeviceCrossVariant) do
    local device_id = variants_table[0].device_id
    for _, variant in ipairs(variants_table) do
      variants[#variants +1] = variant
      assert(device_id == variant.device_id, "Device id in the same device should not change")
    end
  end
  -- Sort all the variants
  table.sort(variants, function(a, b) return tonumber(a.id) < tonumber(b.id) end)
  return variants
end

local function populate_variants(c_test, variants)
  local var_id_to_index_map = {}
  c_test.variants = ffi.cast( "VARIANT_REC_TYPE*", ffi.gc(ffi.C.malloc(ffi.sizeof("VARIANT_REC_TYPE") * #variants), ffi.C.free)) -- ffi malloc array of variants
  ffi.fill(c_test.variants, ffi.sizeof("VARIANT_REC_TYPE") * #variants)
  -- TODO final variants, HELP
  for index, value in ipairs(variants) do
    entry = c_test.variants[index - 1]
    var_id_to_index_map[value.id] = index - 1
    entry.id = assert(tonumber(value.id), "Expected to have entry for id of variant")
    -- TODO check about final variant
    -- if entry.id == c_test.final_variant_id then
    --   c_test.final_variant_idx = curr_index
    -- end
    entry.percentage = assert(tonumber(value.percentage), "Every variant must have a percentage")
    assertx(value.name and #value.name<= consts.AB_MAX_LEN_VARIANT_NAME, "Valid name for variant at position " , index)
    ffi.copy(entry.name, value.name)
    entry.url = assert(value.url, "Entry should have a url") -- TODO why do we have a max length?
    entry.custom_data = value.custom_data or "NULL" -- TODO why do we have a max length
  end
  return var_id_to_index_map
end

local function add_device_specific(c_test, json_table)
  -- get all variants
  local variants = get_all_variants(json_table)
  local num_devices = #json_table.DeviceCrossVariant -- TODO this comes as a constant not based on count heret

  local var_id_to_index_map = populate_variants(c_test, variants)

  local cast_type = string.format("uint8_t *(&)[%s]", consts.AB_NUM_BINS)
  -- TODO num devices not #variants
  c_test.variant_per_bin = ffi.cast(cast_type, ffi.gc(
  ffi.C.malloc(ffi.sizeof(cast_type)*num_devices), ffi.C.free))
  ffi.fill(c_test.variant_per_bin, ffi.sizeof(cast_type)*num_devices)

  table.sort(json_table.DeviceCrossVariant, function(a,b) return tonumber(a[0].device_id) < tonumber(b[0].device_id) end)

  for index, dev_variants in ipairs(json_table.DeviceCrossVariant) do
    -- TODO strong assumption is that the devices will have ids starting from 0
    -- Idea is bin[device_id]
    set_variants_per_bin(c_test.variant_per_bin[index -1], dev_variants, var_id_to_index_map)
  end
end

local function add_device_agnostic(c_test, json_table)
  local variants = json_table.Variants
  assert(#variants >= consts.AB_MIN_NUM_VARIANTS and #variants >= consts.AB_MAX_NUM_VARIANTS, "invalid number of variants")
  c_test.num_variants = #variants -- TODO check for device specific too`
  table.sort(variants, function(a,b) return tonumber(a[0].device_id) < tonumber(b[0].device_id) end)
   local var_id_to_index_map = populate_variants(c_test, variants)

end

function bin_anonymous.add_bins_and_variants(c_test, test_data)
  local external_id = assert(test_data.external_id, "External id needs to be specified for test")
  c_test.external_id = spooky_hash.convert_str_to_u64(external_id)
  local is_dev_spec = assert(tonumber(test_data.is_dev_specific), "Must have boolean device specific or not")
  assert(is_dev_spec == consts.FALSE or is_dev_spec == consts.TRUE, "Test canonly have TRUE or FALSE in  device specific routing")
  c_test.is_dev_specific = is_dev_spec
  if c_test.is_dev_specific == consts.TRUE then
    add_device_specific(c_test, json_table)
  else
    add_device_agnostic(c_test, json_table)
  end
end

return bin_anonymous
