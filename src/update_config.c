#include "ab_incs.h"
#include "ab_globals.h"
#include "auxil.h"
#include "zero_globals.h"
#include "init.h"
#include "update_config.h"
#include "load_lkp.h"
#include "load_classify_ua_map.h"
#include "load_dt.h"

int
update_config(
    void
    )
{
  int status = 0;
  // sz_log_q
  free_if_non_null(g_log_q);
  if ( g_cfg.sz_log_q > 0 ) { 
    g_log_q = malloc(g_cfg.sz_log_q * sizeof(PAYLOAD_TYPE)); 
    return_if_malloc_failed(g_log_q);
    memset(g_log_q, '\0', (g_cfg.sz_log_q * sizeof(PAYLOAD_TYPE)));
    g_n_log_q = 0;
  }
  //---------------------------------------------------

  // statsd.server 
  // statsd.port 
  if ( g_statsd_link != NULL ) { 
    statsd_finalize(g_statsd_link);
    g_statsd_link = NULL;
  }
  if ( ( *g_cfg.statsd.server == '\0' ) || ( g_cfg.statsd.port <= 0 ) ) {
    fprintf(stderr, "WARNING! Not logging to statsd \n");
  }
  else {
    g_statsd_link = statsd_init(g_cfg.statsd.server, g_cfg.statsd.port);
    if ( g_statsd_link == NULL ) { go_BYE(-1); }
  }
  // log_server
  // log_port
  // log_url
  // log_health_url
  // dim_server
  // dim_port
  // dim_url
  // dim_health_url
  shutdown_curl();
  CURLcode res;
  res = curl_global_init(CURL_GLOBAL_DEFAULT);
  if ( res != CURLE_OK) {
    fprintf(stderr, "curl_global_init() failed: %s\n",
        curl_easy_strerror(res));
    go_BYE(-1); 
  }
  // Following for curl for logging GetVariant 
  if ( ( *g_cfg.logger.server == '\0' ) || ( *g_cfg.logger.url == '\0' ) ||
      ( ( g_cfg.logger.port <= 1 )  || ( g_cfg.logger.port >= 65535 ) ) ) {
    fprintf(stderr, "WARNING! /GetVariant NOT being logged\n");
  }
  else {
    status = setup_curl("POST", NULL, g_cfg.logger.server, 
        g_cfg.logger.port, g_cfg.logger.url, g_cfg.logger.health_url, 
        AB_LOGGER_TIMEOUT_MS, &g_ch, &g_curl_hdrs);
    cBYE(status);
  }
  // Following for curl  for Session Service
  if ( ( *g_cfg.ss.server == '\0' ) || ( *g_cfg.ss.url == '\0' ) ||
      ( ( g_cfg.ss.port <= 1 )  || ( g_cfg.ss.port >= 65535 ) ) ) {
    fprintf(stderr, "WARNING! SessionService not in use\n");
  }
  else {
    /* The get_or_create endpoint averages around 25ms response, with 
     * a 95th percentile of 30ms. The plain get endpoint would be 
     * expected to be faster -- Andrew Hollenbach */
    status = setup_curl("GET", g_ss_response, g_cfg.ss.server, 
        g_cfg.ss.port, g_cfg.ss.url, g_cfg.ss.health_url, 
        AB_SS_TIMEOUT_MS, &g_ss_ch, &g_ss_curl_hdrs);
    cBYE(status);
  }
  // num_post_retries, Nothing to do 
  // verbose, Nothing to do 
  // mysql_server For Lua
  // mysql_user For Lua
  // mysql_password For Lua
  // mysql_database For Lua
  // default_url, Nothing to do 
  // reload_on_startup, Nothing to do 
  // xy_guid, Nothing to do 
  // uuid_len, Nothing to do 

  // justin cat file 
  free_if_non_null(g_justin_cat_lkp);  
  g_n_justin_cat_lkp = 0; 
  if ( *g_cfg.justin_cat_file != '\0' ) { 
    status = load_lkp(g_cfg.justin_cat_file, &g_justin_cat_lkp, 
        &g_n_justin_cat_lkp);
    cBYE(status);
  }
  //--------------------------------------------------------
  // os file 
  free_if_non_null(g_os_lkp);  
  g_n_os_lkp = 0; 
  if ( *g_cfg.os_file != '\0' ) { 
    status = load_lkp(g_cfg.os_file, &g_os_lkp, 
        &g_n_os_lkp);
    cBYE(status);
  }
  //--------------------------------------------------------
  // browser file 
  free_if_non_null(g_browser_lkp);  
  g_n_browser_lkp = 0; 
  if ( *g_cfg.browser_file != '\0' ) { 
    status = load_lkp(g_cfg.browser_file, &g_browser_lkp, 
        &g_n_browser_lkp);
    cBYE(status);
  }
  //--------------------------------------------------------
  // device_type file 
  free_if_non_null(g_device_type_lkp);  
  g_n_device_type_lkp = 0; 
  if ( *g_cfg.device_type_file != '\0' ) { 
    status = load_lkp(g_cfg.device_type_file, &g_device_type_lkp, 
        &g_n_device_type_lkp);
    cBYE(status);
  }
  //--------------------------------------------------------
  // referer_class file 
  free_if_non_null(g_referer_class_lkp);  
  g_n_referer_class_lkp = 0; 
  if ( *g_cfg.referer_class_file != '\0' ) { 
    status = load_lkp(g_cfg.referer_class_file, &g_referer_class_lkp, 
        &g_n_referer_class_lkp);
    cBYE(status);
  }
  //--------------------------------------------------------
  // ua_to_dev_map_file
  if ( ( g_classify_ua_map != NULL ) && ( g_len_classify_ua_file != 0 ) ) {
    munmap(g_classify_ua_map, g_len_classify_ua_file);
  }
  if ( *g_cfg.ua_to_dev_map_file != '\0' ) { 
    status = load_classify_ua_map(g_cfg.ua_to_dev_map_file, 
        &g_classify_ua_map, &g_len_classify_ua_file, &g_num_classify_ua_map);
    cBYE(status);
  }
  //--------------------------------------------------------
  // dt_file
  if ( ( g_dt_map != NULL ) && ( g_len_classify_ua_file != 0 ) ) {
    munmap(g_dt_map, g_len_classify_ua_file);
  }
  if ( *g_cfg.ua_to_dev_map_file != '\0' ) { 
    status = load_dt(g_cfg.ua_to_dev_map_file, 
        &g_dt_map, &g_len_classify_ua_file, &g_num_dt_map);
    cBYE(status);
  }

BYE:
  return status;
}
