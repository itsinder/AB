#include "ab_incs.h"
#include "auxil.h"
#include "ab_globals.h"
#include "url.h"
#include "ab_process_req.h"
#include "init.h"
#include "post_from_log_q.h"

<<<<<<< HEAD
#include "l_add_test.h"
#include "num_tests.h"
=======
>>>>>>> dev
#include "l_mdl_meta.h"
#include "l_get_num_features.h"
#include "l_make_feature_vector.h"
#include "l_post_proc_preds.h"

#include "add_test.h"
#include "chk_db_conn.h"
#include "reload.h"
#include "test_info.h"
#include "list_tests.h"
#include "get_config.h"

#include "num_tests.h"
#include "chk_logger_conn.h"
#include "kafka_check_conn.h"
#include "diagnostics.h"
#include "to_kafka.h"
#include "dump_log.h"
#include "add_fake_test.h"
#include "route_get_variant.h"
#include "ping_server.h"
#include "router.h"
#include "update_config.h"
#include "zero_globals.h"
#include "classify.h"
#include "classify_ua.h"
#include "ext_classify_ip.h"
#include "ext_classify_ua.h"
#include "setup.h"


#include "delete_test.h"
#include "stop_test.h"
#include "get_utm_kv.h"
#ifdef KAFKA
#include "kafka_close_conn.h"
#endif

extern char g_config_file[AB_MAX_LEN_FILE_NAME+1];

// START FUNC DECL
  int
ab_process_req(
    AB_REQ_TYPE req_type,
    const char *const api,
    const char *args,
    const char *body
    )
  // STOP FUNC DECL
{
  int status = 0;
  char server[AB_MAX_LEN_SERVER_NAME+1];
  int num_features, itemp;
  //-----------------------------------------
  memset(g_rslt, '\0', AB_MAX_LEN_RESULT+1);
  memset(g_err,  '\0', AB_ERR_MSG_LEN+1);
  memset(g_buf,  '\0', AB_ERR_MSG_LEN+1);
  //-----------------------------------------
  switch ( req_type ) {
    case Undefined :
      go_BYE(-1);
      break;
      //--------------------------------------------------------
    case AddFakeTest : /* done by C */
      status = add_fake_test(args);  cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case AddTest : /* done by Lua */
      status = add_test(body); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case CheckLoggerConnectivity :  /* done by C */
      status = chk_logger_connectivity(g_rslt, AB_MAX_LEN_RESULT);
      cBYE(status);
      break;
      //--------------------------------------------------------
    case CheckKafkaConnectivity :  /* done by C */
      status = kafka_check_conn(g_rslt, AB_MAX_LEN_RESULT);
      cBYE(status);
      break;
      //--------------------------------------------------------
    case CheckDBConnectivity : /* done by Lua */
      status = chk_db_conn(); cBYE(status);
      break;
      //--------------------------------------------------------
    case Classify : /* done by C */
      status = classify(body, g_rslt, AB_MAX_LEN_RESULT); cBYE(status);
      break;
      //--------------------------------------------------------
    case ClassifyIP : /* done by C */
      status = ext_classify_ip(args, g_rslt,AB_MAX_LEN_RESULT); cBYE(status);
      break;
      //--------------------------------------------------------
    case ClassifyUA : /* done by C */
      status = ext_classify_ua(args, g_rslt,AB_MAX_LEN_RESULT); cBYE(status);
      break;
      //--------------------------------------------------------
      /* DeleteTest uses AddTest where state == archived */
    case Diagnostics : /* done by C and Lua */
      status = diagnostics(args); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case DumpLog : /* done by C */
      status = dump_log(); cBYE(status);
      break;
      //--------------------------------------------------------
    case GetConfig : /* done by Lua */
      status = get_config(); cBYE(status);
      break;
      //--------------------------------------------------------
    case GetNumFeatures : /* done by Lua */
      status = l_get_num_features(&num_features); cBYE(status);
      sprintf(g_rslt, " { \"NumFeatures\" : \"%d\", \"GNumfeatures\" : \"%d\" } \n", num_features, g_n_dt_feature_vector);
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
      if ( g_cfg.sz_log_q > 0 ) {
        // Tell consumer that nothing more is coming
        g_halt = true;
        fprintf(stderr, "HALT: Signalling consumer \n");
        pthread_cond_signal(&g_condc);  /* wake up consumer */
        fprintf(stderr, "HALT: Waiting for consumer to finish \n");
        pthread_join(g_con, NULL);
        fprintf(stderr, "HALT: Consumer finished \n");
        pthread_mutex_destroy(&g_mutex);
        pthread_cond_destroy(&g_condc);
        pthread_cond_destroy(&g_condp);
      }
#ifdef KAFKA
      kafka_close_conn();
#endif
      break;
      //--------------------------------------------------------
    case HealthCheck :  /* done by C */
    case Ignore :  /* done by C */
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case IgnoreKafkaErrors :  /* done by C */
      g_ignore_kafka_errors = true;
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case ListTests : /* done by Lua */
      status = list_tests(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case MakeFeatureVector : /* done by Lua */
      status = l_make_feature_vector(body, true);
      cBYE(status);
      break;
      //--------------------------------------------------------
    case MdlMeta : /* done by Lua */
      status = l_mdl_meta(); cBYE(status);
      break;
      //--------------------------------------------------------
    case NumTests : /* done by Lua */
      status = num_tests(&itemp); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"%d\" }", api, itemp);
      break;
      //--------------------------------------------------------
    case PingServer : /* done by C */
      memset(server, '\0', AB_MAX_LEN_SERVER_NAME);
      status = extract_name_value(args, "Service=", '&',
          server, AB_MAX_LEN_SERVER_NAME);
      cBYE(status);
      if ( *server == '\0' ) { go_BYE(-1); }
      if ( strcmp(server, "logger") == 0 ) {
        status = ping_server("logger", g_cfg.logger.server,
            g_cfg.logger.port, g_cfg.logger.health_url, g_rslt);
      }
      else if ( strcmp(server, "webapp") == 0 ) {
        status = ping_server("webapp", g_cfg.webapp.server,
            g_cfg.webapp.port, g_cfg.webapp.health_url, g_rslt);
      }
      else if ( strcmp(server, "kafka") == 0 ) {
        // Indrajeet TODO P1
      }
      else if ( strcmp(server, "statsd") == 0 ) {
        // Indrajeet TODO P1
      }
      else {
        go_BYE(-1);
      }
      break;
      //--------------------------------------------------------
    case PostProcPreds : /* done by C */
      status = l_post_proc_preds(args, g_predictions, g_n_mdl,
          g_rslt, AB_MAX_LEN_RESULT);
      cBYE(status);
      break;
      //--------------------------------------------------------
    case Reload : /* done by Lua */
    case Restart : /* done by Lua */
      // t1 = get_time_usec();
      if ( g_cfg.sz_log_q > 0 ) {
        // Tell consumer thread nothing more is coming
        g_halt = true;
        fprintf(stderr, "Restart: Signalling consumer \n");
        pthread_cond_signal(&g_condc);  /* wake up consumer */
        fprintf(stderr, "Restart: Waiting for consumer to finish \n");
        pthread_join(g_con, NULL);
        fprintf(stderr, "Restart: Consumer finished \n");
        pthread_mutex_destroy(&g_mutex);
        pthread_cond_destroy(&g_condc);
        pthread_cond_destroy(&g_condp);
      }
      // common to restart and reload
      status = setup(g_config_file); cBYE(status);
      pthread_mutex_init(&g_mutex, NULL);
      pthread_cond_init(&g_condc, NULL);
      pthread_cond_init(&g_condp, NULL);
      status = pthread_create(&g_con, NULL, &post_from_log_q, NULL);
      cBYE(status);

      if ( req_type == Reload ) { 
        status = reload();  cBYE(status); 
      }
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case Router : /* done by C */
      status = router(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case StopTest : /* done by C */
      status = stop_test(args); cBYE(status);
      sprintf(g_rslt, "{ \"%s\" : \"OK\" }", api);
      break;
      //--------------------------------------------------------
    case TestInfo : /* done by Lua and C */
      status = test_info(args); cBYE(status);
      break;
      //--------------------------------------------------------
    case UTMKV : /* done by C */
      status = get_utm_kv(args, g_rslt, AB_MAX_LEN_RESULT); cBYE(status);
      break;
      //--------------------------------------------------------
    case ToKafka : /* done by C */
      status = to_kafka(g_body, g_sz_body); cBYE(status);
      break;
      //--------------------------------------------------------
    case ZeroCounters : /* done by C */
      zero_log();
      break;
      //--------------------------------------------------------
    default :
      go_BYE(-1);
      break;
  }
BYE:
  return status ;
}
