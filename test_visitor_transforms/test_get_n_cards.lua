package.path = package.path .. ';../src/?.lua'
local cache = require 'cache'
local status, ccid_mapping = pcall(dofile, '../config/ccid_mapping.lua')
assert(status, 'loading ccid_mapping.lua failed')
assert(ccid_mapping, 'loading ccid_mapping failed')
cache.put("ccid_mapping", ccid_mapping)

describe("Testing get_n_cards", function()
    
    local get_n_cards = require 'get_n_cards'
    it("should get you correct numbers under visitor model (hardcoded)", function()
        assert.are.same(get_n_cards(), 21)
    end)

    it("should get you correct numbers for dicts", function()
        cache.put("ccid_mapping", {[0]=4, [4]=9})
        assert.are.same(get_n_cards(), 2)
    end)

    it("should fail when nothing exists (cos of 0-indexing req)", function()
        cache.put("ccid_mapping", {})
        assert.equal(pcall(get_n_cards), false)
        cache.put("ccid_mapping", ccid_mapping)
    end)
end)
