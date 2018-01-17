#include <stdlib.h>
#include <string.h>
#include "ab_constants.h"
#include "ab_types.h"
#include "ab_globals.h"
#include "macros.h"
#include "auxil.h"
#include "dump_log.h"

int
dump_log(
    const char * const args
    )
{
  int status = 0;
  char log_dir[255+1];
  FILE *fp = NULL;
  char *file_name = NULL;
  bool is_stdout = false;

  memset(log_dir, '\0', 255+1);
  status = extract_name_value(args, "LogDirectory=", '&', log_dir, 255);
  cBYE(status);
  if ( ( status < 0 ) || ( *log_dir == '\0' ) ) { 
    is_stdout = true;
    fp = stdout;
  }
  else {
    int len = strlen(log_dir) + 64;
    file_name = malloc(len); return_if_malloc_failed(file_name);

    memset(file_name, '\0', len);
    strcpy(file_name, log_dir);
    strcat(file_name, "/_scalars.csv");
    fp = fopen(file_name, "w");
    return_if_fopen_failed(fp, file_name, "w");
  }
  //---- Scalars
  fprintf(fp, "StartTime,%" PRIu64 "\n",     g_log_start_time);

  fprintf(fp, "NoUserAgent,%" PRIu64 "\n",     g_log_no_user_agent);
  fprintf(fp, "BadUserAgent,%" PRIu64 "\n",    g_log_bad_user_agent);


  fprintf(fp, "SessionServiceCalls,%" PRIu64 "\n", g_log_ss_calls);
  fprintf(fp, "SessionServiceNonAscii,%" PRIu64 "\n", g_log_ss_non_ascii);
  fprintf(fp, "SessionServiceBadCode,%" PRIu64 "\n", g_log_ss_bad_code);
  fprintf(fp, "SessionServiceNoSession,%" PRIu64 "\n", g_log_ss_no_session);
  fprintf(fp, "SessionServiceBadJSON,%" PRIu64 "\n", g_log_ss_bad_json);

  fprintf(fp, "MissingTestAB,%" PRIu64 "\n",   g_log_missing_test_ab);
  fprintf(fp, "MissingTestXY,%" PRIu64 "\n",   g_log_missing_test_xy);

  fprintf(fp, "NoTestName,%" PRIu64 "\n",      g_log_no_test_name);
  fprintf(fp, "NoTestNames,%" PRIu64 "\n",     g_log_no_test_names);

  fprintf(fp, "NumPosts,%" PRIu64 "\n",        g_log_posts);
  fprintf(fp, "BadPosts,%" PRIu64 "\n",        g_log_bad_posts);
  fprintf(fp, "FailedPosts,%" PRIu64 "\n",     g_log_failed_posts);

  fprintf(fp, "BadUUID,%" PRIu64 "\n",         g_log_bad_uuid);
  fprintf(fp, "BadTestType,%" PRIu64 "\n",     g_log_bad_test_type);
  fprintf(fp, "BadTestName%" PRIu64 "\n",      g_log_bad_test_name);

  fprintf(fp, "NumGetAltVariantCalls,%" PRIu64 "\n", g_log_num_get_alt_variant_calls);
  fprintf(fp, "NumGetVariantCalls,%" PRIu64 "\n", g_log_num_get_variant_calls);
  fprintf(fp, "NumGetVariantsCalls,%" PRIu64 "\n", g_log_num_get_variant_calls);
  fprintf(fp, "NumRouterCalls,%" PRIu64 "\n",      g_log_num_router_calls);
  fprintf(fp, "NumBadRouterCalls,%" PRIu64 "\n",  g_log_num_bad_router_calls);

BYE:
  free_if_non_null(file_name);
  if ( !is_stdout ) { fclose_if_non_null(fp); }
  return status;
}
