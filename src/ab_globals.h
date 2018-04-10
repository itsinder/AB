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

EXTERN CFG_TYPE g_cfg; // Set by Lua . Config variables
//
EXTERN char *g_ss_response; // For C. Get response from Session Server
EXTERN int  g_sz_ss_response; // For C. Size of response

EXTERN statsd_link *g_statsd_link; // For C to talk to statsd server

EXTERN uint32_t g_n_log_q;   // For C
EXTERN PAYLOAD_TYPE *g_log_q; // For C
EXTERN uint32_t g_q_rd_idx; // For C: spot where producer is to write
EXTERN uint32_t g_q_wr_idx; // For C: spot where consumer is to read

EXTERN char *g_uuid; // For C
EXTERN uint64_t g_xy_guid; // For C. Set to positive integer for debugging. 
// Normally this should be 0

EXTERN char g_my_name[AB_MAX_LEN_HOSTNAME+1]; // For C

EXTERN uint64_t g_t_start; // For C
EXTERN bool g_halt; // For C TODO IS THIS NEEDED?
//-----------------------------------------------------------------
EXTERN TEST_META_TYPE g_tests[AB_MAX_NUM_TESTS];  // Set by Lua 
//--------------------------------------------------------
EXTERN uint64_t g_seed1; // For C
EXTERN uint64_t g_seed2; // For C
EXTERN spooky_state g_spooky_state; // For C

EXTERN CURL  *g_ch ; // For C: to talk to Logger
EXTERN struct curl_slist *g_curl_hdrs; // For C: to talk to Logger

EXTERN CURL  *g_ss_ch ; // For C: to talk to session service
EXTERN struct curl_slist *g_ss_curl_hdrs; // For C: to talk to session service

// Following initialized as needed
#ifdef NW_SPECIFIC
EXTERN char g_nw_x_caller_client_id[AB_MAX_LEN_HDR_VAL];
EXTERN char g_nw_x_cookie_id[AB_MAX_LEN_HDR_VAL];
#endif
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

EXTERN uint32_t g_justin_cat_id; 
EXTERN uint32_t g_os_id; 
EXTERN uint32_t g_browser_id; 
EXTERN uint32_t g_device_type_id; 

EXTERN uint32_t g_device_idx; // TODO P0 TO DELETE 

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

EXTERN LKP_REC_TYPE *g_referer_class_lkp; 
EXTERN int g_n_referer_class_lkp; 

EXTERN float *g_dt_feature_vector; 
EXTERN int g_n_dt_feature_vector  ;  // Set by Lua after reading configs
// TODO INDRAJEET ABOVE
EXTERN int g_n_cards;
// ADDED BY LUMPY

EXTERN char g_valid_chars_in_url[256]; // Set by C
EXTERN LKP_REC_TYPE *g_justin_cat_lkp; 
EXTERN int g_n_justin_cat_lkp; 
EXTERN char g_valid_chars_in_ua[256]; // Set by C
EXTERN char g_valid_chars_in_ab_args[256]; // Set by C

EXTERN MAXMIND_REC_TYPE g_maxmind;
//------------------------ For Lua
EXTERN lua_State *g_L; // Set by C
EXTERN lua_State *g_L_DT; // Set by C


#include "ab_log_globals.h"
