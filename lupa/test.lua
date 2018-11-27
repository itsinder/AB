local plfile = require 'pl.file'
local JSON     = require 'lua/JSON'

local init     = require 'lupa/init'
local release  = require 'lupa/release'
local classify = require 'lupa/classify'
-- make_feature_vector is a global function
init('../DT/spam', "random_forest")
body = plfile.read("../DT/spam/sample_input.json")
assert(JSON:decode(body))
for i = 1, 100 do 
  local rslt = classify(body)
  print(i)
end
print("Result = ", rslt)
print("Success")
-- Introduced the explicit call to collectgarbage() because of the os.exit() statement
-- collectgarbage()
os.exit() -- Needed because of LuaJIT and OpenMP