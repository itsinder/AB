#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ab_constants.h"
#include "macros.h"
#include "ab_types.h"
#include "auxil.h"
#include "get_ss_info.h"
#include "ab_globals.h"
#include "dump_log.h"
#include "statsd.h"

/* TODO P3 Verify that g_ss_response does not overflow */
int
get_ss_info(
    const char * const uuid
    )
{
  int status = 0;
  char url[AB_MAX_LEN_URL+1];
  CURLcode curl_res;  long http_code = 0;
  uint64_t t_start = 0, t_stop = 0;
  //----------------------------------------------
  // start assembling URL for session server
  memset(url, '\0', AB_MAX_LEN_URL+1);
  /* Sample 
   *  curl --url "logger/session/v1/cbcace73?caller_client_id=foo
   *  */
  size_t nw = snprintf(url, AB_MAX_LEN_URL,
      "%s:%d/%s/v1/%s/?caller_client_id=abrts&fields=", 
      g_cfg.ss.server, g_cfg.ss.port, g_cfg.ss.url, uuid);
  if ( nw > AB_MAX_LEN_URL ) { go_BYE(-1); } 

  // Fire URL to session service
  // fprintf(stderr, "url = %s \n", url);
  t_start = RDTSC();
  g_log_ss_calls++;
  curl_easy_setopt(g_ss_ch, CURLOPT_URL, url);
  double ss_resp_time = 0;
  curl_res = curl_easy_perform(g_ss_ch);
  t_stop = RDTSC();
  STATSD_COUNT("ss_calls", 1);

  // check response and exit early if needed
  if ( curl_res != CURLE_OK ) {
    g_log_ss_bad_code++; 
    STATSD_COUNT("ss_bad_code", 1); go_BYE(-2); 
  } 
  curl_easy_getinfo(g_ss_ch, CURLINFO_RESPONSE_CODE, &http_code);
  if ( http_code != 200 )  {  
    fprintf(stderr, "url = %s, uuid = %s \n", url, uuid);
    go_BYE(-2); 
  }
  // check ascii and lower case as you go 
  int idx = 0;
  for ( char *cptr = g_ss_response; *cptr != '\0'; cptr++ ) { 
    if ( !isascii(*cptr) ) { 
      g_log_ss_non_ascii++;
       STATSD_COUNT("ss_non_ascii", 1); go_BYE(-2);
    }
    else {
      g_ss_response[idx++] = tolower(*cptr);
    }
  }

  curl_easy_getinfo(g_ss_ch, CURLINFO_TOTAL_TIME, &ss_resp_time);
  // TODO Make sure you are emitting proper statistics 
  /* For Lua to do 
  int is_no_session;
  int is_bad_json;
  int is_null_data;
  // log response time TODO
  status = lua_is_exclude(g_ss_response, g_sz_ss_response, 
   ptr_is_exclude, &is_no_session, &is_bad_json);
  if ( is_null_data ) { g_log_ss_null_data++; go_BYE(-2); }
  if ( is_bad_json )   { g_log_ss_bad_json++; go_BYE(-2); }
  if ( is_no_session ) { g_log_ss_no_session++; go_BYE(-2); }
  */

BYE:
  memset(g_ss_response, '\0', g_sz_ss_response);
  return status;
}
