#include "ab_globals.h"
#include "statsd.h"
#include <stdlib.h>

extern bool g_disable_sd;
int 
STATSD_INC(
    char *stat
    )
{
  int status = 0;
  if ( g_disable_sd ) { return 0; }
  if ( g_statsd_link != NULL ) { return -1; }
  snprintf(g_statsd_buf, AB_MAX_LEN_STATSD_BUF, "%s.%s", 
      g_cfg.statsd_keys.inc, stat); 
  status = statsd_inc(g_statsd_link, stat, 1.0);
  return status;
}

int 
STATSD_DEC(
    const char *stat
    )
{
  int status = 0;
  if ( g_disable_sd ) { return 0; }
  if ( g_statsd_link != NULL ) { return -1; }
  snprintf(g_statsd_buf, AB_MAX_LEN_STATSD_BUF, "%s.%s", 
      g_cfg.statsd_keys.inc, stat); 
  status = statsd_dec(g_statsd_link, g_statsd_buf, 1.0);

  return status;
}

int 
STATSD_COUNT(
    const char *stat, 
    size_t cnt
    )
{
  int status = 0;
  if ( g_disable_sd ) { return 0; }
  if ( g_statsd_link != NULL ) { return -1; }
  snprintf(g_statsd_buf, AB_MAX_LEN_STATSD_BUF, "%s.%s", 
      g_cfg.statsd_keys.count, stat);
  status = statsd_count(g_statsd_link, g_statsd_buf, cnt, 1.0);
  return status;
}

int 
STATSD_GAUGE(
    const char *stat, 
    size_t value
    )
{
  int status = 0;
  if ( g_disable_sd ) { return 0; }
  if ( g_statsd_link != NULL ) { return -1; }
  snprintf(g_statsd_buf, AB_MAX_LEN_STATSD_BUF, "%s.%s", 
      g_cfg.statsd_keys.gauge, stat);
  status = statsd_gauge(g_statsd_link, g_statsd_buf, value);
  return status;
}

int 
STATSD_TIMING(
    const char *stat, 
    size_t ms
    )
{
  int status = 0;
  if ( g_disable_sd ) { return 0; }
  if ( g_statsd_link != NULL ) { return -1; }
  snprintf(g_statsd_buf, AB_MAX_LEN_REDIRECT_URL, "%s.%s", 
      g_cfg.statsd_keys.inc, stat);
  status = statsd_timing(g_statsd_link, g_statsd_buf, ms);
  return status;
}
