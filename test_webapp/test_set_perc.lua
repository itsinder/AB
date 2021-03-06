
<<<<<<< HEAD
local S = require 'test_webapp/state_changes'
local mk_rand_test = require 'test_webapp/mk_rand_test'


local T = {}
T.t1 = function(num_iters)
  if ( not num_iters ) then num_iters = 100 end 
  for i = 1, num_iters do
    local T1, T2, T3
    -- make a test 
    local hdrs, body, status = mk_rand_test()
    local tid = get_test_id(hdrs)
    --= change percentages. it should work 
    T1 = get_test_info(tid)
    T2, status = set_random_percs(T1)
    assert(status)
    T3 = get_test_info(tid)
    assert(compare_test_info(T2, T3))
    -- publish it  and change percentages. it should work.
    S.publish(tid)
    T2, status = set_random_percs(T1)
    assert(status)
    T3 = get_test_info(tid)
    assert(compare_test_info(T2, T3))
    -- start it  and change percentages. it should work.
    S.start(tid)
    T2, status = set_random_percs(T1)
    assert(status)
    T3 = get_test_info(tid)
    assert(compare_test_info(T2, T3))
    -- terminate test and try to change percentages. should not work 
    S.terminate(tid)
    T2, status = set_random_percs(T1)
    assert(not status)
    -- archive test and try to change percentages. should not work 
    S.archive(tid)
    T2, status = set_random_percs(T1)
    assert(not status)
=======
local S = require 'test_webapp/state_change'
local mk_rand_test = require 'test_webapp/mk_rand_test'
local get_test_info = require 'test_webapp/get_test_info'
local set_perc = require 'test_webapp/set_perc'
local reset_db = require 'test_webapp/reset_db'


local function compare_percs(
  P,
  V
  )
  assert(type(P) == "table")
  assert(type(V) == "table")
  assert(#P == #V)
  for i = 1, #P do
    -- print(i, V[i].name, V[i].percentage, P[i])
    assert(tonumber(P[i]) == tonumber(V[i].percentage))
  end
end

local T = {}
T.t1 = function(num_iters)
  reset_db()
  if ( not num_iters ) then num_iters = 100 end 
  for i = 1, num_iters do
    local T1, T2, T3, P, status
    -- make a test 
    local tid = mk_rand_test()
    T1 = get_test_info(tid)
    --= change percentages. it should work 
    P  = set_perc(tid)
    T3 = get_test_info(tid)
    compare_percs(P, T3.Variants)

    -- publish it  and change percentages. it should work.
    S.publish(tid)
    P  = set_perc(tid)
    T3 = get_test_info(tid)
    -- assert(compare_test_info(T2, T3))
    -- start it  and change percentages. it should work.
    S.start(tid)
    P  = set_perc(tid)
    T3 = get_test_info(tid)
    -- assert(compare_test_info(T2, T3))
    -- terminate test and try to change percentages. should not work 
    S.terminate(tid)
    status = pcall(set_perc, tid)
    assert(not status)
    -- archive test and try to change percentages. should not work 
    S.archive(tid)
    status = pcall(set_perc, tid)
>>>>>>> dev
  end
end
T.t1(100)
return T
