local assertx = require 'assertx'

local function postal_code_to_vars(zip_code)
  -- I expect zip_code as a string, but otherwise it's okay, 
  -- will do it as both string or numeric.
  if type(zip_code) == 'string' and tonumber(zip_code) ~= nil then
    zip_code = tonumber(zip_code)
  end
  assert(g_postal_cd_features, "g_postal_cd_features is missing.")
  op = g_postal_cd_features[zip_code]
  assertx(op, 'Postal code ', zip_code, ' not found in Zillow/TU database.')
  return op
end

return postal_code_to_vars
