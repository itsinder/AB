#include "ab_incs.h"
#include "auxil.h"
#include "ab_globals.h"
#include "url.h"
#include "ab_process_req.h"
#include "init.h"
#include "post_from_log_q.h"

#include "add_test.h"
#include "chk_logger_conn.h"
#include "chk_db_conn.h"
#include "del_test.h"
#include "diagnostics.h"
#include "dump_log.h"
#include "route_get_variant.h"
#include "list_tests.h"
#include "ping_server.h"
#include "router.h"
#include "set_percentages.h"
#include "set_dev_spec_perc.h"
#include "stop_test.h"
#include "test_info.h"
#include "zero_globals.h"
#include "ua_to_device.h"

// START FUNC DECL
int 
ab_process_req(
    const char *api,
    const char *args
    )
// STOP FUNC DECL
{
  int status = 0;
  double ping_time;
  int ping_status; 
  uint64_t t_start, t_stop;

  //-----------------------------------------
  t_start = timestamp();
  memset(g_rslt, '\0', AB_MAX_LEN_RESULT+1);
  memset(g_err,  '\0', AB_ERR_MSG_LEN+1);
  memset(g_buf,  '\0', AB_ERR_MSG_LEN+1);

  // NW Specific
  if ( strcmp(api, "api/v1/health_check") == 0 ) { 
    strcpy(g_rslt, "{ \"HealthCheck\" : \"OK\" }");
    return status;
  }

  AB_REQ_TYPE req_type = get_req_type(api); 
  if ( g_verbose ) { fprintf(stderr, "%s-> %s \n", api, args); }
  //-----------------------------------------
  switch ( req_type ) {
    case Undefined : 
      go_BYE(-1);
      break;
      //--------------------------------------------------------
   case AddTest : /* done by Lua */
      status = l_add_test(args); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case CheckLoggerConnectivity :  /* done by C */
      status = chk_logger_connectivity(g_rslt, AB_MAX_LEN_RESULT); 
      cBYE(status);
      break;
      //--------------------------------------------------------
    case CheckDBConnectivity : /* done by Lua */
      status = l_chk_db_conn(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case DeleteTest :  /* done by Lua */
      status = l_del_test(args); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case Diagnostics : /* done by C and Lua */
      status = diagnostics(); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api); 
      break;
      //--------------------------------------------------------
    case DumpLog : /* done by C */
      status = dump_log(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case GetVariant :  /* done by C */
    case GetVariants :  /* done by C */
      status = route_get_variant(req_type, args);  cBYE(status);
      break;
      //--------------------------------------------------------
    case Halt : /* done by C */
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      g_halt = true;
      if ( g_sz_log_q > 0 ) { 
        // Tell consumer ead nothing more is coming 
        pthread_cond_signal(&g_condc);	/* wake up consumer */
        fprintf(stderr, "Waiting for consumer to finish \n");
        pthread_join(g_con, NULL);
        fprintf(stderr, "Consumer finished \n");
        pthread_mutex_destroy(&g_mutex);
        pthread_cond_destroy(&g_condc);
        pthread_cond_destroy(&g_condp);
      }
      break;
      //--------------------------------------------------------
    case HealthCheck :  /* done by C */
    case Ignore :  /* done by C */
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case ListTests : /* done by Lua */
      status = l_list_tests(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case PingLogServer : /* done by C */
      ping_status = ping_server(g_log_server, g_log_port, 
          g_log_health_url, &ping_time); 
      if ( ping_status < 0 ) { 
        sprintf(g_rslt, "{ \"%s\" : \"ERROR\" }", api);
      }
      else {
        if ( ping_time == 0 ) { 
          sprintf(g_rslt, "{ \"%s\" : \"LOG SERVER NOT SET\" }", api);
        }
        else {
          sprintf(g_rslt, "{ \"%s\" : \"%lf\" }", api, ping_time);
        }
      }
      break;
      //--------------------------------------------------------
    case PingSessionServer : /* done by C */
      ping_status = ping_server(g_ss_server, g_ss_port, 
          g_ss_health_url, &ping_time); 
      if ( ping_status < 0 ) { 
        sprintf(g_rslt, "{ \"%s\" : \"ERROR\" }", api);
      }
      else {
        if ( ping_time == 0 ) { 
          sprintf(g_rslt, "{ \"%s\" : \"SESSION SERVER NOT SET\" }", api);
        }
        else {
          sprintf(g_rslt, "{ \"%s\" : \"%lf\" }", api, ping_time);
        }
      }
      break;
      //--------------------------------------------------------
    case Reload : /* done by Lua */
    case Restart : /* done by Lua */
      // t1 = get_time_usec(); 
      g_halt = true;
      if ( g_sz_log_q > 0 ) { 
        // Tell consumer thread nothing more is coming 
        pthread_cond_signal(&g_condc);	/* wake up consumer */
        fprintf(stderr, "Waiting for consumer to finish \n");
        pthread_join(g_con, NULL);
        fprintf(stderr, "Consumer finished \n");
        pthread_mutex_destroy(&g_mutex);
        pthread_cond_destroy(&g_condc);
        pthread_cond_destroy(&g_condp);
      }
      g_halt = false;
      // common to restart and reload
      free_globals();
      status = zero_globals();  cBYE(status);
      // Note; We do NOT do zero_conf()
      status = init(); cBYE(status);
      if ( g_sz_log_q > 0 ) { 
        pthread_mutex_init(&g_mutex, NULL);	
        pthread_cond_init(&g_condc, NULL);
        pthread_cond_init(&g_condp, NULL);
        status = pthread_create(&g_con, NULL, &post_from_log_q, NULL);
        cBYE(status);
      }
      switch ( req_type ) { 
        case Reload: /* call Lua status = reload(false); */ break;
        case Restart : /* nothing to do  */ break;
        default : go_BYE(-1); break;
      }
      cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case Router : /* done by C */
      status = router(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case SetPercentages :  // done by Lua 
      status = l_set_percentages(args);  
      cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case StopTest :  /* done by Lua */
      status = l_stop_test(args); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case TestInfo : /* done by Lua */
      status = l_test_info(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case UAToDevice : /* done by C */
      status = ua_to_device(args, g_rslt,  AB_MAX_LEN_RESULT); cBYE(status);
      break;
      //--------------------------------------------------------
    default : 
      go_BYE(-1);
      break;
  }
BYE:
  // TODO: Improve the statsd story 
  if ( ( ( req_type == GetVariant )  || ( req_type == GetVariants ) ) && 
      ( status == 0 ) ) {
    t_stop = timestamp();
    if ( t_stop > t_start ) { 
      uint64_t t_delta = t_stop - t_start;
      if ( g_statsd_link != NULL ) { 
        statsd_timing(g_statsd_link, g_statsd_timing, t_delta);
        statsd_inc(g_statsd_link, g_statsd_inc, 1.0); 
      }
    }
  }
  return status ;
}

AB_REQ_TYPE
get_req_type(
    const char *api
    )
{
  if (strcasecmp(api, "Ignore") == 0) {
    return Ignore;
  }
  else if (strcasecmp(api, "PingLogServer") == 0) {
    return PingLogServer;
  }
  else if (strcasecmp(api, "PingSessionServer") == 0) {
    return PingSessionServer;
  }
  else if (strcasecmp(api, "CheckLoggerConnectivity") == 0) {
    return CheckDBConnectivity;
  }
  else if (strcasecmp(api, "CheckLoggerConnectivity") == 0) {
    return CheckDBConnectivity;
  }
  else if (strcasecmp(api, "HealthCheck") == 0) {
    return HealthCheck;
  }
  else if (strcasecmp(api, "ListTests") == 0) {
    return ListTests;
  }
  else if (strcasecmp(api, "TestInfo") == 0) {
    return TestInfo;
  }
  else if (strcasecmp(api, "Diagnostics") == 0) {
    return Diagnostics;
  }
  else if (strcasecmp(api, "AddTest") == 0) {
    return AddTest;
  }
  else if (strcasecmp(api, "DeleteTest") == 0) {
    return DeleteTest;
  }
  else if (strcasecmp(api, "DumpLog") == 0) {
    return DumpLog;
  }
  else if (strcasecmp(api, "SetPercentages") == 0) {
    return SetPercentages;
  }
  else if (strcasecmp(api, "StopTest") == 0) {
    return StopTest;
  }
  else if (strcasecmp(api, "GetVariant") == 0) {
    return GetVariant;
  }
  else if (strcasecmp(api, "GetVariants") == 0) {
    return GetVariants;
  }
  else if (strcasecmp(api, "Halt") == 0) {
    return Halt;
  }
  else if (strcasecmp(api, "Restart") == 0) {
    return Restart;
  }
  else if (strcasecmp(api, "Router") == 0) {
    return Router;
  }
  else if (strcasecmp(api, "UAToDevice") == 0) {
    return UAToDevice;
  }
  else if ( strcasecmp(api, "favicon.ico") == 0 ) { 
    return Ignore;
  }
  else {
    fprintf(stderr,  "Unknown API = %s \n", api);

    return Undefined;
  }
  return Undefined;
}
