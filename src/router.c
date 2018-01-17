#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "ab_constants.h"
#include "macros.h"
#include "ab_types.h"
#include "ab_globals.h"
#include "router.h"
#include "auxil.h"
#include "alt_get_variant.h"

int 
router(
    const char *args
    )
{
  int status = 0;

  g_log_router_calls++;
  memset(g_redirect_url, '\0', AB_MAX_LEN_REDIRECT_URL+1);
  status = alt_get_variant(args);
  // Notice that you cannot afford to return a status of -1
  // This has to succeed *always* . Hence, the need for a default URL
  if ( ( status < 0 ) || ( *g_redirect_url == '\0' ) ) {
    status = 0;
    g_log_bad_router_calls++;
    strcpy(g_redirect_url, g_default_url);
    // following helps to detect error 
    // TODO P3: Make sure that g_redirect_url does 
    // not have a question mark in it
    const char *qparams = "?VariantID=0&GUID=XXXXXXXX&"; 
    if ( (strlen(qparams) + strlen(g_redirect_url)) 
        < AB_MAX_LEN_REDIRECT_URL ) {
      strcat(g_redirect_url, qparams);
    }
    if ( (strlen(args) + strlen(g_redirect_url)) 
        < AB_MAX_LEN_REDIRECT_URL ) {
      strcat(g_redirect_url, args);
    }
    // fprintf(stderr, "%s\n", g_redirect_url);
    fprintf(stderr, "Bad input to router [%s] \n", args);
    WHEREAMI; 
  }
  char buf[64];
  memset(buf, '\0', 64);

  sprintf(buf,"&DeviceID=%d", g_devices[g_device_idx].id);
  strcat(g_redirect_url, buf);

  sprintf(buf,"&Device=%s", g_devices[g_device_idx].name);
  strcat(g_redirect_url, buf);

  // Notice that you cannot afford to return a status of -1
  // This has to succeed *always* . Hence, the need for a default URL
  return status;
}
