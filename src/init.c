#include "ab_incs.h"
#include "ab_globals.h"
#include "auxil.h"
#include "spooky_hash.h"
#include "zero_globals.h"
#include "init.h"
#include "ping_server.h"

static size_t write_data(
    void *buffer, 
    size_t size, 
    size_t nmemb, 
    void *userp
    )
{
  // basically do nothing with response
  return size * nmemb;
}

size_t 
WriteMemoryCallback(
    void *contents, 
    size_t size, 
    size_t nmemb, 
    void *userp
    )
{
  int realsize = size * nmemb;
  if ( realsize > g_sz_ss_response ) { 
    for ( ; g_sz_ss_response < realsize; ) { 
      g_sz_ss_response *= 2 ;
    }
    g_ss_response = realloc(g_ss_response, g_sz_ss_response);
  }
  memcpy(userp, contents, realsize);
  return realsize;
}

#ifdef TEST_STATSD
  statsd_count(g_statsd_link, "count1", 123, 1.0);
  statsd_count(g_statsd_link, "count2", 125, 1.0);
  statsd_gauge(g_statsd_link, "speed", 10);
  statsd_timing(g_statsd_link, "request", 2400);
  sleep(1);
  statsd_inc(g_statsd_link, "count1", 1.0);
  statsd_dec(g_statsd_link, "count2", 1.0);
  int i;
  for (i=0; i<10; i++) {
      statsd_count(g_statsd_link, "count3", i, 0.8);
  }
#endif

int
init_lua(
    void
    )
{
  int status = 0;
  if ( g_disable_lua ) { return status; }
  g_L = luaL_newstate(); if ( g_L == NULL ) { go_BYE(-1); }
  luaL_openlibs(g_L);  
  status = luaL_dostring(g_L, "require 'RTS/ab'"); cBYE(status);
  if ( status != 0 ) { 
    fprintf(stderr, "Loading RTS/ab failed: %s\n", 
        lua_tostring(g_L, -1));
    lua_pop(g_L, 1); go_BYE(-1);
  }

  if ( !g_disable_dt ) {
    g_L_DT = luaL_newstate(); if ( g_L_DT == NULL ) { go_BYE(-1); }
    luaL_openlibs(g_L_DT);  
    status = luaL_dostring(g_L_DT, "require 'DT/dt'"); cBYE(status);
    if ( status != 0 ) { 
      fprintf(stderr, "Loading DT/dt failed: %s\n", 
          lua_tostring(g_L_DT, -1));
      lua_pop(g_L_DT, 1); go_BYE(-1);
    }
  }

BYE:
  return status;
}

int
setup_curl(
    const char *method,
    char *write_buffer,
    const char *service,
    const char *server,
    int port,
    const char *url,
    const char *health_url,
    uint32_t timeout_ms,
    CURL **ptr_ch,
    struct curl_slist **ptr_curl_hdrs
)
{
  int status = 0;
  char *full_url = NULL;
  CURL *ch = NULL;
  struct curl_slist *curl_hdrs = NULL;
  CURLcode res;

  *ptr_ch = NULL;
  *ptr_curl_hdrs = NULL;


  ch = curl_easy_init();
  if ( ch == NULL ) { go_BYE(-1); }
  // TODO WHY IS SETTING HEADERS NOT WORKING??????
  // I am getting Content-Type => application/x-www-form-urlencoded
  // I am getting X-Caller-Client-ID as empty string
  curl_hdrs = curl_slist_append(curl_hdrs, "Content-Type: application/json");
  curl_hdrs = curl_slist_append(curl_hdrs, "X-Caller-Client-ID: ab-runtime-service");
  /*
   * Robby Bryant 
   * fwiw - when you hit logger APIs, can send a custom header called
   * `X-Caller-Client-ID` with a a value of `ab-runtime-service` (or 
   * any other string of your choosing)?  that way we can slice out 
   * your traffic in our logs and metrics
   * */
  int len = strlen(server) + strlen(url) + 32;
  full_url = malloc(len); return_if_malloc_failed(full_url);
  int nw;
  if ( port <= 0 ) { 
    nw = snprintf(full_url, len-1, "http://%s/%s", server, url);
  }
  else {
    nw = snprintf(full_url, len-1, "http://%s:%d/%s", server,port,url);
  }
  if ( nw >= len-1 ) { go_BYE(-1); }
  res = curl_easy_setopt(ch, CURLOPT_URL, full_url);
  if ( res != CURLE_OK ) { go_BYE(-1);  }
  res = curl_easy_setopt(ch, CURLOPT_VERBOSE, 0);
  if ( res != CURLE_OK ) { go_BYE(-1);  }
  res = curl_easy_setopt(ch, CURLOPT_TIMEOUT_MS,     timeout_ms);
  if ( res != CURLE_OK ) { go_BYE(-1);  }
  res = curl_easy_setopt(ch, CURLOPT_DNS_CACHE_TIMEOUT,     100);
  if ( res != CURLE_OK ) { go_BYE(-1);  }
  if ( strcmp(method, "POST") == 0 ) { 
    res = curl_easy_setopt(ch, CURLOPT_POST, 1L);
    if ( res != CURLE_OK ) { go_BYE(-1);  }
    //  we do not want any response for POST . See write_data()
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, write_data);
  }
  else {
    res = curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    if ( res != CURLE_OK ) { go_BYE(-1);  }
    res = curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)write_buffer);
    if ( res != CURLE_OK ) { go_BYE(-1);  }
  }
  /* set our custom set of headers */ 
  res = curl_easy_setopt(ch, CURLOPT_HTTPHEADER, curl_hdrs);
  if ( res != CURLE_OK ) { go_BYE(-1);  }
  //-------
  // Check that log server is listening
  if ( ( health_url != NULL ) && ( *health_url != '\0' ) ) {
    int ping_status = ping_server(service, server, port, health_url, NULL);
    if ( ping_status < 0 ) { 
      fprintf(stderr, "WARNING! Server %s:%d, url %s  not running\n", 
          server, port, health_url);
      go_BYE(-1); 
    }
    else {
      fprintf(stderr, "[INFO] Server %s:%d running.\n", server, port);
    }
  }
  *ptr_ch = ch;
  *ptr_curl_hdrs = curl_hdrs;
BYE:
  if ( status < 0 ) { 
    // TODO P2: Free anything you allocated here
  }
  free_if_non_null(full_url);
  return status;
}

void
init_globals(
    void
    )
{
  g_seed1 = AB_SEED_1;
  g_seed2 = AB_SEED_2;
  spooky_init(&g_spooky_state, g_seed1, g_seed2);
  srand48(g_seed1);

  // TODO Check with Braad that this is good
  const char *url_str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&=/:_-%,;[].?+() ";
  for ( char *cptr = (char *)url_str; *cptr != '\0'; cptr++ ) {
    g_valid_chars_in_url[(uint8_t)(*cptr)] = true;
  }

  // TODO Check with Braad that this is good
  const char *ua_str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&=/:_-%,;[].?+() ";
  for ( char *cptr = (char *)ua_str; *cptr != '\0'; cptr++ ) {
    g_valid_chars_in_ua[(uint8_t)(*cptr)] = true;
  }

  const char *arg_str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&=_";
  for ( char *cptr = (char *)arg_str; *cptr != '\0'; cptr++ ) {
    g_valid_chars_in_ab_args[(uint8_t)(*cptr)] = true;
  }
}
