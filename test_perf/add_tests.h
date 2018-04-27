extern int
add_tests(
    CURL *ch,
    char *server,
    int port,
    int num_tests,
    char ***ptr_test_urls
    );
extern int
del_tests(
    CURL *ch,
    char *server,
    int port,
    int num_tests
    );
extern int
stop_tests(
    CURL *ch,
    char *server,
    int port,
    int num_tests
    );