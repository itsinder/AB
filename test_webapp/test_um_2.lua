package.path = package.path .. ";./../src/?.lua;./../test_webapp/?.lua;../test_generator/?.lua"
require 'str'
local JSON = require 'JSON'
local plfile = require 'pl.file'
local plpath = require 'pl.path'
local curl = require 'curl'
local mk_test = require 'mk_test'
local reset_db = require 'reset_db'
local get_test_id = require 'get_test_id'
local get_test_info = require 'get_test_info'
local get_error_code = require 'get_error_code'
local states = require 'states'
--==========================
local ssurl =  -- set state URL 
 "http://localhost:8080/AB/php/endpoints/endpoint_set_state.php"
local tburl =  -- test basic URL 
 "http://localhost:8080/AB/php/endpoints/endpoint_test_basic.php"
local fourl =  -- set follow on URL 
 "http://localhost:8080/AB/php/endpoints/endpoint_set_follow_on.php"

-- STOP: Stuff common to all tests in this suite
local tests = {}
tests.t1 = function (
  just_pr
  )
  local description = "A test to check possibilities of duplicate name of test"
  if ( just_pr ) then print(description) return end

   -- START: Make some test 
  local chk_T
  reset_db()
  local hdrs, outbody, status = mk_test("good_basic1.lua")
  assert(status == 200)
  local test_id_1 = get_test_id(hdrs)
  local T1 = get_test_info(test_id_1)
  assert(test_id_1 == tonumber(T1.id))
  for k, v in pairs(T1) do print(k, v) end 

  -- Try to create a test with same name again
  local hdrs, outbody, status = mk_test("good_basic1.lua")
  assert(status ~= 200)
  for k, v in pairs(T1) do print(k, v) end 

  -- Create test name with same name but another test type
  T1.TestType = "ABTest"
  T1.id = ""
  local hdrs, outbody, status = mk_test(T1)
  for k, v in pairs(hdrs) do print(k, v) end
  assert(status == 200)
  local test_id_2 = get_test_id(hdrs)
  local T2 = get_test_info(test_id_2)
  assert(test_id_2 == tonumber(T2.id))

-- SET STATE of T1 to dormant
  T1.TestType = "XYTest" 
  T1.id = test_id_1
  T1.NewState = "dormant"
  T1.Updater =  "joe" -- TODO Improve this hard coding
  local hdrs, outbody, status = curl.post(ssurl, nil, JSON:encode(T1))
  assert(status == 200)
  chk_T = get_test_info(test_id_1)

  assert(chk_T.State == "dormant")

-- SET STATE of T2 to started
  T1.NewState = "started"
  T1.Updater =  "joe" -- TODO Improve this hard coding
  local hdrs, outbody, status = curl.post(ssurl, nil, JSON:encode(T1))
  assert(status == 200)
  chk_T = get_test_info(test_id_1)
  -- for k, v in pairs(chk_T) do print(k, v) end 
  assert(chk_T.State == "started")

-- SET STATE of T2 to terminated
  T1.NewState = "terminated"
  T1.Winner = "Control"
  local hdrs, outbody, status = curl.post(ssurl, nil, JSON:encode(T1))
  assert(status == 200)
  chk_T = get_test_info(test_id_1)
  -- for k, v in pairs(chk_T) do print(k, v) end 
  assert(chk_T.State == "terminated")

-- SET STATE of T2 to archived
  T1.NewState = "archived"
  T1.Updater =  "joe" -- TODO Improve this hard coding
  local hdrs, outbody, status = curl.post(ssurl, nil, JSON:encode(T1))
  assert(status == 200)
  chk_T = get_test_info(test_id_1)
  -- for k, v in pairs(chk_T) do print(k, v) end 
  assert(chk_T.State == "archived")


-- Reproduce test 1 of same testtype XYTest
  local hdrs, outbody, status = mk_test("good_basic1.lua")
  assert(status == 200)
  local test_id_3 = get_test_id(hdrs)
  local T3 = get_test_info(test_id_3)
  assert(test_id_3 == tonumber(T3.id))
  for k, v in pairs(T1) do print(k, v) end 


end



--===================================================
tests.t1() -- TODO: DELETE ONCE I FIGURE OUT WHY TESTRUNNER NOT WORKING HERE
return tests
