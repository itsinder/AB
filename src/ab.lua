local csv = require "csv"
local json = require "json"
-- local ffi = require 'ffi' 
local tests = require 'add_test'
local cache = require 'cache'
local reload_tests = require 'reload'

local hard_code_config = require 'hard_code_config'
local x_load_config = require 'ab_load_config'
local x_update_config = require 'ab_update_config'

function hard_code_config(...)
  cache.put("config", hard_code_config(...))
end

function load_config(...)
  cache.put("config", x_load_config(...))
end

function update_config(...)
  x_update_config(...)
end
--=================================================
function preproc(...)
  tests.preprco(...)
end

function list_tests()
  return json.encode(cache.get('tests'))
end

function get_config()
  return cache.get('config')
end

function reload(...)
  return reload_tests.reload(...)
end

function check_db_conn()
  local configs = cache.get('config').AB.MYSQL
  local conn = load_config.db_connect(configs)
  assert(conn ~= nil, "must be a valid connection object")
end

function get_config()
  return json.encode(cache.get('config'))
end

function get_test_info(args)
  local j_table = json.decode(args)
  local tests = cache.get('tests')
  local test = nil
  if j_table.id ~= nil then
    test = tests[j_table.id]
    assert(test ~= nil, "Tests with id ", j_table.id, " does not exist")
  elseif j_table.name ~= nil and j_table.test_type ~= nil then
    for _, v in pairs(tests) do
      if v.name == j_table.name and v.test_type == j_table.test_type then
        assert(test == nil, "More than one match exists")
        test = v
      end
    end
  end
  return json.encode(test)
end

-- function add(c_str, c_data)
--    -- print("hi from lua", c_str)
--    local x = ffi.cast("int*", c_data)
--    print(c_str)
--    x[0] = 9
-- end
