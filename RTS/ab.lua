-- ab.lua is the only place where we have globals rest all use locals.
-- All function call from C are registered here.
-- Also the globals are functions.
local cache = require 'lua/cache'
local JSON  = require 'lua/JSON'
local x_hard_code_config = require 'lua/hard_code_config'
local x_load_config = require 'RTS/ab_load_config'
local x_update_config = require 'RTS/ab_update_config'
local x_diagnostics = require 'RTS/diagnostics'

local tests = require 'RTS/add_test'
local reload_tests = require 'RTS/reload'

function load_config(...)
  cache.put("config", x_load_config.load_config(...))
end

function update_config(...)
  x_update_config(...)
end
--=================================================
function preproc(...)
  tests.preproc(...)
end

function add(...)
  tests.add(...)
end


function list_tests()
  local tests = cache.get('tests')
  local o_table = {}
  if ( not tests ) then return " [] " end
  for _,v in pairs(tests) do
    local entry = {}
    entry.name = v.name
    entry.TestType = v.TestType
    o_table[#o_table + 1] = entry
  end
  return JSON:encode(o_table)
end

function reload(...)
  return reload_tests.reload(...)
end

function check_db_conn()
  local configs = cache.get('config').AB.DB
  local conn = load_config.db_connect(configs)
  assert(conn ~= nil, "must be a valid connection object")
  conn:close() -- TODO RS Integrate into webserver
end

function get_config()
  return JSON:encode(cache.get('config'))
end

function get_test_info(args)
  local j_table = JSON:decode(args)
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
  return JSON:encode(test)
end

function diagnostics(...)
  x_diagnostics()
end
-- function add(c_str, c_data)
--    -- print("hi from lua", c_str)
--    local x = ffi.cast("int*", c_data)
--    print(c_str)
--    x[0] = 9
-- end
