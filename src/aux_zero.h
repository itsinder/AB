extern int
zero_test(
    uint32_t test_idx
    );
extern int
free_test(
    uint32_t test_idx
    );
extern void 
shutdown_curl(
    void
    );
extern int
malloc_test(
    int test_idx,
    int num_variants,
    int is_dev_specific,
    int state
    );
extern int
free_variant_per_bin(
    uint32_t test_idx
    );
extern int
malloc_final_variant(
    uint32_t test_idx,
    int num_devices
    );
