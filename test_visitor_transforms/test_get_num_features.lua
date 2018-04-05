package.path = package.path .. ';../src/?.lua'
--local table_dt_feature = dofile('../config/dt_feature.lua')
--_G.g_dt_feature = assert(dofile('../config/dt_feature.lua'))
local cache = require 'cache'
local status, g_dt_feature = pcall(dofile, '../config/dt_feature.lua')
assert(status, 'loading dt_feature.lua failed')
assert(g_dt_feature, 'loading dt_feature failed')
cache.put("g_dt_feature", g_dt_feature)

describe("Testing get_num_features", function()
    

    local get_num_features = require 'get_num_features'
    it("should get you correct numbers under visitor model (hardcoded)", function()
        assert.are.same(get_num_features(), 124)
    end)

    it("should get you correct numbers for non-nested dicts", function()
        cache.put("g_dt_feature", {[0]=4, [4]=9})
        assert.are.same(get_num_features(), 2)
    end)

    it("should get you correct numbers for nested dicts", function()
        cache.put("g_dt_feature", {[1]={[2]={[3]=4, [4]=5}, [5]=6}, [6]=1})
        assert.are.same(get_num_features(), 4)
    end)

    it("should get you correct numbers when nothing exists", function()
        cache.put("g_dt_feature", {})
        assert.are.same(get_num_features(), 0)
        cache.put("g_dt_feature", g_dt_feature)
    end)
end)
