#ifdef __AB_MAIN_PROGRAM
#define EXTERN extern
#else
#define EXTERN 
#endif
// Global State that persists across invocations
#include "unistd.h"
#include "spooky_hash.h"
#include "statsd-client.h"
#include "ab_incs.h"
#include <curl/curl.h>
#include <semaphore.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <pthread.h>
#include "rdkafka.h"

EXTERN CFG_TYPE g_cfg; // Set by Lua . Config variables
//
EXTERN char *g_ss_response; // For C. Get response from Session Server
EXTERN int  g_sz_ss_response; // For C. Size of response

EXTERN statsd_link *g_statsd_link; // For C to talk to statsd server

EXTERN int32_t g_n_log_q;   // For C
#ifdef AB_AS_KAFKA
EXTERN KAFKA_REC_TYPE *g_log_q; // For C [g_cfg.sz_log_q] 
#else
EXTERN PAYLOAD_REC_TYPE *g_log_q; // For C
#endif
EXTERN uint32_t g_q_rd_idx; // For C: spot where producer is to write
EXTERN uint32_t g_q_wr_idx; // For C: spot where consumer is to read

EXTERN char *g_uuid; // For C

EXTERN char g_my_name[AB_MAX_LEN_HOSTNAME+1]; // For C

EXTERN bool g_halt; // For C TODO IS THIS NEEDED?
//-----------------------------------------------------------------
EXTERN TEST_META_TYPE g_tests[AB_MAX_NUM_TESTS];  // Set by Lua 
// TODO P3 needs more documentation for following
EXTERN const char *g_test_str[AB_MAX_NUM_TESTS];  // Set by Lua 
//--------------------------------------------------------
EXTERN uint64_t g_seed1; // For C
EXTERN uint64_t g_seed2; // For C
EXTERN spooky_state g_spooky_state; // For C

EXTERN CURL  *g_ch ; // For C: to talk to Logger
EXTERN struct curl_slist *g_curl_hdrs; // For C: to talk to Logger

EXTERN CURL  *g_ss_ch ; // For C: to talk to session service
EXTERN struct curl_slist *g_ss_curl_hdrs; // For C: to talk to session service

// Following initialized as needed
EXTERN char g_redirect_url[AB_MAX_LEN_REDIRECT_URL+1]; // For C: router()
EXTERN char g_err[AB_ERR_MSG_LEN+1]; // For C: ab_process_req()
EXTERN char g_buf[AB_ERR_MSG_LEN+1]; // For C: ab_process_req()
EXTERN char g_rslt[AB_MAX_LEN_RESULT+1]; // For C: ab_process_req()
EXTERN char g_mrslt[AB_MAX_LEN_RESULT+1]; // For C: for GetVariants
EXTERN char g_curl_payload[AB_MAX_LEN_PAYLOAD+1]; // For C: post_from_log_q()
// above initialized as needed
//
// Stuff needed for multi-threading 
EXTERN pthread_t g_con; // For C
EXTERN pthread_cond_t g_condc, g_condp; // For C
EXTERN pthread_mutex_t g_mutex; // For C

// START: User Agent Classifier
#include "ua_types.h"
EXTERN uint32_t g_justin_cat_other_id; 
EXTERN uint32_t g_justin_cat_id; 
EXTERN uint32_t g_os_id; 
EXTERN uint32_t g_browser_id; 
EXTERN uint32_t g_device_type_id; 

EXTERN UA_REC_TYPE *g_classify_ua_map; // Set by C
EXTERN size_t g_len_classify_ua_file; // Set by C
EXTERN uint32_t g_num_classify_ua_map; // Set by C

EXTERN LKP_REC_TYPE *g_justin_cat_lkp; 
EXTERN int g_n_justin_cat_lkp; 

EXTERN LKP_REC_TYPE *g_os_lkp; 
EXTERN int g_n_os_lkp; 

EXTERN LKP_REC_TYPE *g_browser_lkp; 
EXTERN int g_n_browser_lkp; 

EXTERN LKP_REC_TYPE *g_device_type_lkp; 
EXTERN int g_n_device_type_lkp; 

// STOP : User Agent Classifier

// get ip address from browser headers
EXTERN char g_ip_address[AB_MAX_LEN_IP_ADDRESS+1];  
// get date from browser headers
EXTERN char g_date[AB_MAX_LEN_DATE+1];  
EXTERN char g_in_tracer[AB_MAX_LEN_TRACER+1];
EXTERN char g_out_tracer[AB_MAX_LEN_TRACER+1];
EXTERN LKP_REC_TYPE *g_referer_class_lkp; 
EXTERN int g_n_referer_class_lkp; 

EXTERN float *g_dt_feature_vector; 
EXTERN int g_n_dt_feature_vector  ;  // Set by Lua after reading configs

EXTERN char g_valid_chars_in_url[256]; // Set by C
EXTERN LKP_REC_TYPE *g_justin_cat_lkp; 
EXTERN int g_n_justin_cat_lkp; 
EXTERN char g_valid_chars_in_ua[256]; // Set by C
EXTERN char g_valid_chars_in_ab_args[256]; // Set by C

EXTERN MAXMIND_REC_TYPE g_maxmind;
//------------------------ For Lua
EXTERN lua_State *g_L; // Set by C
EXTERN lua_State *g_L_DT; // Set by C
EXTERN bool g_disable_lua; // used for testing, normally false
EXTERN bool g_disable_dt; // set to false if no decision tree
EXTERN bool g_disable_ua; // set to false if no user agent classifier
EXTERN bool g_disable_ip; // set to false if no MaxMind Database
EXTERN bool g_disable_sd; // disable statsd
EXTERN bool g_disable_wa; // disable WebApp
EXTERN bool g_disable_lg; // disable Logger
EXTERN bool g_disable_kf; // disable Kafka


#include "ab_log_globals.h"

#include "dt_types.h"
EXTERN DT_REC_TYPE *g_dt; /* decision tree [g_n_dt]  */
EXTERN uint32_t g_n_dt;
EXTERN size_t g_len_dt_file; 
EXTERN RF_REC_TYPE *g_rf; /* random forest * [g_n_rf] */
EXTERN uint32_t g_n_rf;
EXTERN size_t g_len_rf_file; 
EXTERN MDL_REC_TYPE *g_mdl; /* models [g_n_mdl] */
EXTERN uint32_t g_n_mdl;
EXTERN size_t g_len_mdl_file; 
EXTERN float *g_predictions;  /* [g_n_mdl] */

#include "utm_kv.h"
EXTERN UTM_REC_TYPE g_utm_kv;

// For Kafka
EXTERN bool g_ignore_kafka_errors; 
EXTERN rd_kafka_t *g_rk;         /* Producer instance handle */
EXTERN rd_kafka_topic_t *g_rkt;  /* Topic object */
EXTERN rd_kafka_conf_t *g_kafka_conf;  /* Temporary configuration object */
EXTERN uint8_t g_kafka_callback;
EXTERN char g_errstr[512];       /* librdkafka API error reporting buffer */
// char g_buf[512];          /* Message value temporary buffer */

EXTERN char g_body[AB_MAX_LEN_BODY+1];
EXTERN int g_sz_body;
EXTERN int g_kafka_memory; 

EXTERN char g_statsd_buf[AB_MAX_LEN_STATSD_BUF+1];

#include "ua_types.h"
EXTERN  MODEL_REC_TYPE *g_ua_M; int g_ua_nM;
EXTERN  MODEL_NAME_TYPE *g_ua_N; int g_ua_num_models;
EXTERN  uint64_t *g_ua_H; int g_ua_nH;
