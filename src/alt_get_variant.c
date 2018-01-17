#include "ab_incs.h"
#include "ab_globals.h"
#include "auxil.h"
#include "get_test_idx.h"
#include "add_test.h"
#include "alt_get_variant.h"
#include "get_variant.h"
#include "dump_log.h"

//<hdr>
int
make_guid(
    uint64_t seed,
    uint64_t curr_time,
    char temp_guid[2*AB_GUID_LENGTH],
    uint64_t *ptr_rnum // random number 
    )
//<hdr>
{
  int status = 0;
  char valid_chars[62] = {
    '0', '1', '2', '3', '4', '5', '6','7', '8', '9', 
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  // START: Get a pseudo-random number
  uint64_t pseudo_rand_num = spooky_hash64(&curr_time, sizeof(uint64_t), seed);
  *ptr_rnum = pseudo_rand_num;
  // STOP: Get a pseudo-random number
  // START: Make a guid
  memset(temp_guid, '\0', 2*AB_GUID_LENGTH);
  if ( g_xy_guid > 0 ) { 
    /* This is used for testing purposes */
    sprintf(temp_guid, "%" PRIu64, g_xy_guid++);
  }
  else {
    for ( int i = 0; i < AB_GUID_LENGTH; i++ ) { 
      int idx = pseudo_rand_num & 0x0000003F; // we want last 6 bits 
      if ( idx == 63 ) { idx = 0; } // We have only 62 valid characters
      if ( idx == 62 ) { idx = 1; } // We have only 62 valid characters
      // TODO: P4: Distribute extra bits better 
      temp_guid[i] = valid_chars[idx];
      pseudo_rand_num = pseudo_rand_num >> 6; // shift 6 bits to right
    }
  }
  // STOP: Make a guid --------------------------------
  return status;
}

//<hdr>
static int
find_test(
    const char *args, 
    int *ptr_test_idx
    )
//</hdr>
{
  int status = 0;
  *ptr_test_idx = -1;
  int test_idx = -1;
  char test_name[AB_MAX_LEN_TEST_NAME+1]; 
  uint64_t external_id = 0;
  // Test can be specified by name or external_id 
  memset(test_name, '\0', (AB_MAX_LEN_TEST_NAME+1));
  status = extract_name_value(args, "TestName=", '&', test_name, 
      AB_MAX_LEN_TEST_NAME); 
  cBYE(status);
  if ( test_name[0] == '\0' ) {
    char buf[32]; memset(buf, '\0', 32);
    status = extract_name_value(args, "ExternalID=", '&', buf, 32); 
    // DELETED DEC 2016 cBYE(status);
    if ( ( status != 0 ) || ( *buf == '\0' ) ) { 
      status = extract_name_value(args, "nw_campaign_id=", '&', buf, 32); 
      cBYE(status);
    }
    if ( *buf == '\0' ) { go_BYE(-1); } 
    int64_t tempI8;
    status = stoI8(buf, &tempI8); 
    if ( status < 0 ) { 
      fprintf(stderr, "buf = [%s] \n", buf);
    }
    cBYE(status);
    if ( tempI8 <= 0 ) { go_BYE(-1); }
    external_id = (uint64_t) tempI8;
    for ( int i = 0; i < AB_MAX_NUM_TESTS; i++ ) {
      if ( g_tests[i].external_id == external_id ) {
        test_idx = i; break;
      }
    }
  }
  else {
    status = get_test_idx(test_name, AB_TEST_TYPE_XY, &test_idx);
    cBYE(status);
  }
  // Verify test exists and is of type XYTest
  if ( test_idx < 0 ) { g_log_missing_test_xy++; go_BYE(-1); }
  if ( g_tests[test_idx].test_type != AB_TEST_TYPE_XY ) { go_BYE(-1); }
  *ptr_test_idx = test_idx;
BYE:
  return status;
}

//<hdr>
int alt_get_variant(
    const char *args
    )
//</hdr>
{
  int status = 0;
  int test_idx = -1;
  char temp_guid[2*AB_GUID_LENGTH];
  uint64_t pseudo_rand_num = 0; int bin = 0;
  uint64_t curr_time = RDTSC();
  char in_tracer[AB_MAX_LEN_TRACER+1];
  char out_tracer[AB_MAX_LEN_TRACER+1];
  uint32_t variant_idx;

  if ( ( args == NULL ) || ( *args == '\0' ) ) { go_BYE(-1); }
  memset(g_redirect_url, '\0', (AB_MAX_LEN_REDIRECT_URL+1));
  memset(in_tracer, '\0', AB_MAX_LEN_TRACER+1);
  memset(out_tracer, '\0', AB_MAX_LEN_TRACER+1);
  memset(temp_guid, '\0', 2*AB_GUID_LENGTH);

  status = find_test(args, &test_idx);cBYE(status);
  if ( test_idx < 0 ) { go_BYE(-1); }
  g_log_num_get_alt_variant_calls++; // increment only if test is legit 
  get_tracer(args, in_tracer); 
  set_tracer(out_tracer); 

  make_guid(g_tests[test_idx].seed, curr_time, temp_guid, &pseudo_rand_num); 
  bin = pseudo_rand_num % AB_NUM_BINS;
  if ( g_tests[test_idx].is_dev_specific ) {
    // TODO P3 Delete later. Following is temporary code to aid debugging
    char buf[16]; memset(buf, '\0', 16); 
    status = extract_name_value(args, "Device=", '&', buf, 16);
    bool found = false;
    if ( *buf != '\0' ) { 
      for ( unsigned int j = 0; j < g_n_devices; j++ ) { 
        if ( strcmp(g_devices[j].name, buf) == 0 ) { 
          found = true; g_device_idx = j; break;
        }
      }
    }
    // We change g_device_idx if (1) GET param Device sent (2) it is valid
    //-----------------------
    if ( !found ) { 
      fprintf(stderr, "Unknown device. Defaulting to other\n");
      g_device_idx = g_other_idx;
    }
    // TODO P1: Should not we be using MAX_NUM_BINS here??
    double rnum = drand48();
    // Based on device, X gives us proportions for that variant
    DEV_SPEC_PERC_TYPE *X = g_tests[test_idx].dev_spec_perc;
    uint32_t  nX = g_tests[test_idx].n_dev_spec_perc;
    if ( g_device_idx > nX ) { go_BYE(-1); }
    uint32_t nV = g_tests[test_idx].num_variants; 
    //----------------------------------
    if ( g_tests[test_idx].state == TEST_STATE_TERMINATED ) {
      variant_idx = X[test_idx].final_variant_idx;
    }
    else if ( g_tests[test_idx].state == TEST_STATE_STARTED ) {
    found = false;
      for ( uint32_t i = 0; i < nV; i++ ) { 
        if ( ( rnum <= X[i].cum_percentage ) && ( X[i].percentage > 0 ) ) {
          found = true; variant_idx = i; break;
        }
      }
      // catch a fall through 
      if ( !found ) { variant_idx = nV-1; } // TODO: Log error 
    }
    else {
      go_BYE(-1);
    }
  }
  else { // Test is NOT device specific 
    if ( g_tests[test_idx].state == TEST_STATE_TERMINATED ) {
      variant_idx = g_tests[test_idx].final_variant_idx;
      uint32_t variant_id  = g_tests[test_idx].final_variant_id;
      sprintf(g_rslt, "{ \"Variant\" : \"%s\", \"GUID\" : \"%s\", \"VariantID\" :  %d, \"URL\" : \"%s\" }",
          g_tests[test_idx].variants[variant_idx].name, 
          temp_guid,
          g_tests[test_idx].variants[variant_idx].id, 
          g_tests[test_idx].variants[variant_idx].url);
      if ( ( strlen(g_tests[test_idx].variants[variant_idx].url) + strlen(args) 
            + AB_GUID_LENGTH + 16 ) > AB_MAX_LEN_REDIRECT_URL ) {
        go_BYE(-1);
      }
      // START: Handle fact that question mark should occur only once
      char separator = '?';
      for ( char *cptr = g_tests[test_idx].variants[variant_idx].url; 
          *cptr != '\0'; cptr++ ) { 
        if ( *cptr == '?' ) { separator = '&'; break; }
      }
      if ( *args == '?' ) { args++; }
      // STOP: Handle qmark 

      int nw = snprintf(g_redirect_url, AB_MAX_LEN_REDIRECT_URL,
          "%s%c%s&VariantID=%uGUID=%s", 
          g_tests[test_idx].variants[variant_idx].url, 
          separator, 
          args, 
          variant_id,
          temp_guid);
      if ( nw >= AB_MAX_LEN_REDIRECT_URL ) { 
        memset(g_redirect_url, '\0', (AB_MAX_LEN_REDIRECT_URL+1));
        status = -1;
      }
      goto BYE;
    }
    else if ( g_tests[test_idx].state == TEST_STATE_STARTED )  { 
      variant_idx = g_tests[test_idx].variant_per_bin[bin];
      if ( variant_idx > g_tests[test_idx].num_variants ) { go_BYE(-1); }
      if ( g_tests[test_idx].variants[variant_idx].percentage == 0 ) {
        go_BYE(-1);
      }
    }
    else {
      go_BYE(-1);
    }
  }

  // double check 
  if ( *g_tests[test_idx].variants[variant_idx].url == '\0' ) {go_BYE(-1);}

  uint32_t variant_id = g_tests[test_idx].variants[variant_idx].id;
  uint32_t x_tst_id = g_tests[test_idx].x_tst_id;
  sprintf(g_rslt, " { \"Variant\" : \"%s\", \"GUID\" : \"%s\", \"VariantID\" : \"%u\", \"URL\" : \"%s\" }", 
      g_tests[test_idx].variants[variant_idx].name, 
      (char *)temp_guid,
      g_tests[test_idx].variants[variant_idx].id,
      g_tests[test_idx].variants[variant_idx].url);
  PAYLOAD_TYPE lcl_payload;
  memset(&lcl_payload, '\0', sizeof(PAYLOAD_TYPE));
  strcpy(lcl_payload.uuid, temp_guid);
  lcl_payload.time       = curr_time;
  lcl_payload.test_id    = x_tst_id;
  lcl_payload.variant_id = variant_id;

  status = log_decision(lcl_payload); cBYE(status);
  cBYE(status);
  // TODO: P4 Do we need trailing slash afterlanding page URL?
  // START: Handle fact that question mark should occur only once
  char separator = '?';
  for ( char *cptr = g_tests[test_idx].variants[variant_idx].url; 
      *cptr != '\0'; cptr++ ) { 
    if ( *cptr == '?' ) { separator = '&'; break; }
  }
  if ( *args == '?' ) { args++; }
  // STOP: Handle qmark 
  int nw = snprintf(g_redirect_url, AB_MAX_LEN_REDIRECT_URL, 
      "%s%c%s&VariantID=%d&GUID=%s", 
      g_tests[test_idx].variants[variant_idx].url, separator, args, 
      variant_id, temp_guid);
  if ( nw >= AB_MAX_LEN_REDIRECT_URL ) { go_BYE(-1); }

BYE:
  return status;
}

/*
Yes. The "query string" part of the URL always starts with a `?` and uses `&` to separate arguments, so .com?something=true&something_else=false is valid but .com&something=true&something_else=false and .com?something=true?something_else=false are invalid.

Typically you would lean on a package or write your own to parse a URL into it's pieces, operate on the pieces as native types like arrays and objects, then reassemble the URL when you're done. For example, in PHP the function http_build_query http://php.net/manual/en/function.http-build-query.php does exactly this, taking an associative array of keys and values and transforming them into a valid query string.


*/