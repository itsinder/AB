require 'lua/str'
local JSON = require 'lua/JSON'
local curl = require 'lua/curl'

local function set_follow_on(
  tid,
  tid_to_follow
  )
  local furl =  -- set state URL 
 "http://localhost:8080/AB/php/endpoints/endpoint_set_follow_on.php"
  assert(tid); assert(type(tid) == "number")
  assert(tid_to_follow); assert(type(tid_to_follow) == "number")

  local T = {}
  T.tid = tid
  T.tid_to_follow = tid_to_follow
  local hdrs, outbody, status = curl.post(furl, nil, JSON:encode(T))
  -- print(outbody)
  -- for k, v in pairs(hdrs) do print(k, v) end 
  assert(status == 200)
  return true
end
return set_follow_on
