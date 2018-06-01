local ffi = require 'ffi'
require 'DT/dt'
hard_code_config()
update_config()

local post_proc_preds = require 'DT/lua/post_proc_preds'
ffi.cdef([[
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void * malloc(size_t size);
void free(void *ptr);
   ]])

local tests = {}

local X = {}
local Y = {}
Y.n_opvec = 1
X[1] = Y

tests.t1 = function()
  for k, v in ipairs(X) do 
    local n_opvec = v.n_opvec
    local opvec = ffi.cast("float *", ffi.gc(ffi.C.malloc(n_opvec * ffi.sizeof("float")), ffi.C.free))
    opvec[0] = 0.5
    local sz_out_buf = 65536
    local out_buf = ffi.C.malloc(sz_out_buf)
    assert( post_proc_preds(opvec, n_opvec, out_buf, sz_out_buf))
    print(ffi.string(ffi.cast("char*", out_buf)))
  end
end
tests.t1()
return tests
