#include "ab_incs.h"
#include "ab_globals.h"
#include "auxil.h"
#include "ab_auxil.h"
#include "get_test_idx.h"
#include "log_decision.h"
#include "get_variant.h"
#include "make_curl_payload.h"
#include "statsd.h"

int 
get_variant(
    const char *args
    )
{
  int status = 0;
  char test_name[AB_MAX_LEN_TEST_NAME+1]; 
  int test_idx = -1;
  int test_type = AB_TEST_TYPE_AB;
  TEST_META_TYPE *T = NULL;
  char in_tracer[AB_MAX_LEN_TRACER+1];
  uint64_t curr_time = get_time_usec();

  if ( ( args == NULL ) || ( *args == '\0' ) ) { go_BYE(-1); }
  memset(g_uuid, '\0', g_cfg.max_len_uuid+1);
  //-------------------------------------------------------------
  status = get_test_name(args,  test_name);  cBYE(status);
  status = get_test_idx(test_name, test_type, &test_idx); cBYE(status);
  cBYE(status);
  T = &(g_tests[test_idx]);
  g_log_get_variant_calls++;
  STATSD_COUNT("get_variant_calls", 1);
  // Extract UUID
  status = extract_name_value(args, "UUID=", '&', g_uuid, g_cfg.max_len_uuid);
  if ( ( status < 0 ) || ( *g_uuid == '\0' ) ) { 
    status = -1;
    g_log_no_uuid++;
    STATSD_COUNT("no_uuid", 1);
  }
  cBYE(status);
  status = chk_uuid(g_uuid, g_cfg.max_len_uuid); 
  if ( status < 0 ) {
    status = AB_ERROR_CODE_BAD_UUID;
    g_log_bad_uuid++;
    STATSD_COUNT("bad_uuid", 1);
  }
  cBYE(status);
  get_tracer(args, in_tracer);
  uint32_t nV = g_tests[test_idx].num_variants;
  //--------------------------------------------------------
  // Deal with filters
  bool is_exclude = false; 
  if ( T->has_filters ) {  
    // TODO  RAMESH P1 
    if ( is_exclude ) {
      int nw = snprintf(g_rslt, AB_MAX_LEN_RESULT,
          "{ \"Variant\" : \"Ineligible\", \"VariantID\" :  0, \"Test\" : \"%s\", \"TestID\" : %d }",
          test_name, g_tests[test_idx].id);
      if ( nw >= AB_MAX_LEN_RESULT ) { go_BYE(-1); }
      goto BYE;
    }
  }
  // Deal with special case of Terminated test 
  if ( g_tests[test_idx].state == TEST_STATE_TERMINATED ) {
    uint32_t final_variant_idx = g_tests[test_idx].final_variant_idx[0];
    if ( final_variant_idx >= nV ) { go_BYE(-1); }
    const char *cd = g_tests[test_idx].variants[final_variant_idx].custom_data;
    const char *vname = g_tests[test_idx].variants[final_variant_idx].name;
    int vid = g_tests[test_idx].variants[final_variant_idx].id;
    if ( cd == NULL ) { cd = "null"; }
    int nw = snprintf(g_rslt, AB_MAX_LEN_RESULT, 
"{ \
\"Variant\" : \"%s\", \
\"variant\" : \"%s\", \
\"VariantID\" :  %d, \
\"variant_id\" :  %d, \
\"CustomData\" : %s, \
\"custom_data\" : %s, \
\"Test\" : \"%s\", \
\"test\" : \"%s\", \
\"TestID\" : %d,  \
\"test_id\" : %d  \
}",
        vname,  vname,
        vid, vid,
        cd, cd,
        test_name,  test_name,
        g_tests[test_idx].id, g_tests[test_idx].id
        ); 
    if ( nw >= AB_MAX_LEN_RESULT ) { go_BYE(-1); }
    goto BYE; // Nothing more to do. Early exit
  }
  //-----------------------------------------------------------
  if ( g_tests[test_idx].state != TEST_STATE_STARTED )  { go_BYE(-1); }
  uint64_t uuid_hash = 
    spooky_hash64(g_uuid, g_cfg.max_len_uuid, g_tests[test_idx].seed);
  //-----------------------------------------------------------
  uint32_t bin = uuid_hash % AB_NUM_BINS;
  if ( g_tests[test_idx].is_dev_specific ) { go_BYE(-1); }

  uint32_t variant_idx = g_tests[test_idx].variant_per_bin[0][bin];
  if ( variant_idx >= g_tests[test_idx].num_variants ) { go_BYE(-1); }
  uint32_t variant_id  = g_tests[test_idx].variants[variant_idx].id;
  const char *cptr = g_tests[test_idx].variants[variant_idx].custom_data;
  if ( cptr == NULL ) { cptr = "null"; }
  int nw = snprintf(g_rslt, AB_MAX_LEN_RESULT, 
      "{ \"Variant\" : \"%s\", \"VariantID\" : %d, \"CustomData\" : %s, \"Test\" : \"%s\", \"TestID\" : %d }", 
      g_tests[test_idx].variants[variant_idx].name, 
      variant_id, 
      cptr, 
      test_name, 
      g_tests[test_idx].id);
  if ( nw >= AB_MAX_LEN_RESULT ) { go_BYE(-1); }
  //------------------------------------------------
  PAYLOAD_REC_TYPE lcl_payload;
  zero_payload(&lcl_payload);
  strcpy(lcl_payload.uuid, g_uuid);
  strcpy(lcl_payload.in_tracer,  in_tracer);
  strcpy(lcl_payload.out_tracer, g_out_tracer);
  lcl_payload.test_id    = g_tests[test_idx].id;
  lcl_payload.variant_id = variant_id;
  lcl_payload.time       = curr_time;
  status = log_decision(&lcl_payload); cBYE(status);
  //--------------------------------------------------------
BYE:
  return status;
}

