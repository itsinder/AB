#undef CAPTURE_SERVER_ERROR
#include "ab_incs.h"
#include "ab_constants.h"
#include "auxil.h"
#include "mmap.h"
#include <err.h>
#include <event2/keyvalq_struct.h>
#include <event.h>
#include <evhttp.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <jansson.h>

extern int
check_payload(
    const char * const cbuf
    );

// START GLOBALS
bool g_verbose;
bool g_err;
uint64_t g_num_posts;
uint64_t g_sum_time;
uint64_t g_max_time;
uint64_t g_min_time;
uint64_t g_prev_time; // last time seen in POST from RTS 
char g_logfile[AB_MAX_LEN_FILE_NAME+1];
FILE *g_fp;
// STOP GLOBALS

extern void 
generic_handler(
    struct evhttp_request *req, 
    void *arg);

void 
generic_handler(
    struct evhttp_request *req, 
    void *arg
    )
{
  int status = 0;
  uint64_t t_start, t_stop, t_delta;
  struct event_base *base = (struct event_base *)arg;
  struct evbuffer *inbuf = NULL;
  struct evbuffer *opbuf = NULL;
  char lbuf[1024];
  memset(lbuf, '\0', 1024);
  opbuf = evbuffer_new();
  if ( opbuf == NULL) {
    err(1, "failed to create response buffer");
  }

  char api[AB_MAX_LEN_API_NAME+1]; 
  char args[AB_MAX_LEN_ARGS+1];

  memset(api,  '\0', AB_MAX_LEN_API_NAME+1);
  memset(args, '\0', AB_MAX_LEN_ARGS+1);
  const char *uri = evhttp_request_uri(req);
  //--------------------------------------
  int i = 0;
  char *cptr = (char *)uri; cptr++; // Jump over forward slash
  for ( ; ( ( *cptr !=  '?' ) && ( *cptr !=  '\0' ) ); ) {
    if ( i > AB_MAX_LEN_API_NAME ) { 
      fprintf(stderr, "Bad api in [%s] \n", uri); return;
    }

    api[i++] = *cptr++;
  }
  if ( *cptr == '?' ) { cptr++; /* Skip over '?' */ }
  //--------------------------------------
  i = 0;
  for ( ; *cptr !=  '\0'; ) {
    if ( i > AB_MAX_LEN_ARGS ) { 
      fprintf(stderr, "Bad api in [%s] \n", uri); return;
    }
    args[i++] = *cptr++;
  }
  //--------------------------------------
  // fprintf(stderr, "uri  = %s \n", uri);
  // fprintf(stderr, "api  = %s \n", api);
  // fprintf(stderr, "args = %s \n", args);
  if ( strcmp(api, "Log") == 0 ) {
    // Check that input headers contains "Content-Type: application/json"
    bool found_header_1 = false;
    bool found_header_2 = false;
    struct evkeyvalq *headers = NULL;
    struct evkeyval  *header = NULL;
    headers = evhttp_request_get_input_headers (req);
    header = headers->tqh_first;
    bool is_error = false;
    for (header = headers->tqh_first; header; 
        header = header->next.tqe_next) {
      // fprintf(stderr, "%s => %s\n",  header->key, header->value);
      if ( strcasecmp(header->key, "Content-Type") == 0 ) { 
        found_header_1 = true;
        if ( strcasecmp(header->value, "application/json") != 0 ) { 
          is_error = true; 
        }
      }
      if ( strcasecmp(header->key, "X-Caller-Client-ID") == 0 ) { 
        found_header_2 = true;
        if ( strcasecmp(header->value, "ab-runtime-service") != 0 ) { 
          is_error = true; 
        }
      }
    }
    // TODO FIX P1 if ( is_error ) { go_BYE(-1); }
    if ( !found_header_1 ) { go_BYE(-1); }
    if ( !found_header_2 ) { go_BYE(-1); }
    //-----------------------------------------------------
    g_num_posts++;
    t_start = timestamp();
    inbuf = evhttp_request_get_input_buffer(req);
    while (evbuffer_get_length(inbuf)) {
      int n;
      char cbuf[AB_MAX_LEN_BODY+1];
      memset(cbuf, '\0', AB_MAX_LEN_BODY+1);
      n = evbuffer_remove(inbuf, cbuf, sizeof(cbuf));
      if ( n == 0 ) { go_BYE(-1); }
      if ( n > 0) { // verify that it is good JSON
        status = check_payload(cbuf); cBYE(status);
      }
    }
    t_stop = timestamp();
    if ( t_stop > t_start ) { 
      t_delta = t_stop - t_start;
      g_min_time = min(g_min_time, t_delta);
      g_min_time = max(g_max_time, t_delta);
      g_sum_time += t_delta;
    }
    else {
      g_num_posts--;
    }
    evbuffer_add_printf(opbuf, "{ \"Log\" : \"OK\" }");
  }
  else if ( strcmp(api, "SetOutput") == 0 ) { 
    fprintf(stderr, "Got %s:%s \n", api, args);
#define BUFLEN 15
    char buf[BUFLEN+1];
    char logfile[AB_MAX_LEN_FILE_NAME+1];
    memset(buf, '\0', BUFLEN+1);
    status = extract_name_value(args, "Action=", '&', buf, BUFLEN);
    cBYE(status);
    memset(logfile, '\0', AB_MAX_LEN_FILE_NAME+1);
    status = extract_name_value(args, "LogFile=", '&', logfile, 
        AB_MAX_LEN_FILE_NAME);
    cBYE(status);
    if ( strcasecmp(buf, "Start") == 0 ) { 
      if ( *logfile == '\0' ) { go_BYE(-1); }
      strcpy(g_logfile, logfile);
      fclose_if_non_null(g_fp); 
      g_fp = fopen(g_logfile, "w");
      return_if_fopen_failed(g_fp, g_logfile, "w");
      fprintf(stderr, "Writing to %s \n", g_logfile);
    }
    else if ( strcasecmp(buf, "Stop") == 0 ) { 
      fflush(g_fp); 
      fclose_if_non_null(g_fp); 
    }
    else if ( strcasecmp(buf, "Continue") == 0 ) { 
      if ( *g_logfile == '\0' ) { go_BYE(-1); }
      fclose_if_non_null(g_fp); 
      g_fp = fopen(g_logfile, "a");
      return_if_fopen_failed(g_fp, g_logfile, "a");
    }
    else {
      go_BYE(-1);
    }
    evbuffer_add_printf(opbuf, "{ \"SetOutput\" : \"OK\" }");
  }
  else if ( strcmp(api, "SetVerbose") == 0 ) { 
    fprintf(stderr, "Got %s:%s \n", api, args);
    if ( strcasecmp(args, "true") == 0 ) { 
      g_verbose = true;
    }
    else if ( strcasecmp(args, "false") == 0 ) { 
      g_verbose = false;
    }
    else {
      // Status quo 
    }
    evbuffer_add_printf(opbuf, "{ \"SetVerbose\" : \"OK\" }");
  }
  else if ( strcmp(api, "PerfMetrics") == 0 ) { 
    double mu = 0;
    if ( g_num_posts > 0 ) { 
      mu = (double)g_sum_time / (double)g_num_posts;
    }
    int nw = snprintf(lbuf, 1024, "{ \"NumPosts\" : %" PRIu64 ", \"AvgTime\" : \"%lf\" }", g_num_posts, mu);
    if ( nw >= 1024 ) { go_BYE(-1); }
    evbuffer_add_printf(opbuf, "%s", lbuf);
  }
  else if ( strcmp(api, "Ignore") == 0 ) {
    evbuffer_add_printf(opbuf, "{ \"Ignore\" : \"OK\" }");
  }
  else if ( strcmp(api, "IsError") == 0 ) {
    evbuffer_add_printf(opbuf, "{ \"IsError\" : \"%d\" }", g_err);
    if ( strcasecmp(args, "Reset") == 0 ) { 
      g_err = false;
    }
  }
  else if ( strcmp(api, "Config") == 0 ) {
    evbuffer_add_printf(opbuf, "{ \"LogFile\" : \"%s\", \"Verbose\" : \"%d\" } ", g_logfile, g_verbose);
  }
  else if ( strcmp(api, "Restart") == 0 ) {
    g_verbose    = false;
    g_err        = false;
    g_num_posts  = 0;
    g_sum_time   = 0;
    g_max_time   = 0;
    g_min_time   = 0;
    g_prev_time  = 0;
    fclose_if_non_null(g_fp); 
    memset(g_logfile, '\0', AB_MAX_LEN_FILE_NAME+1);


    evbuffer_add_printf(opbuf, "{ \"Restart\" : \"OK\" }");
  }
  else if ( strcmp(api, "Halt") == 0 ) {
    evbuffer_add_printf(opbuf, "{ \"Halt\" : \"OK\" }");
    evhttp_send_reply(req, HTTP_OK, "OK", opbuf);
    event_base_loopbreak(base);
  }
BYE:
  if ( status < 0 ) { 
    evbuffer_add_printf(opbuf, "ERROR");
    evhttp_send_reply(req, HTTP_BADREQUEST, "ERROR", opbuf);
  }
  else {
    evhttp_send_reply(req, HTTP_OK, "OK", opbuf);
  }
  evbuffer_free(opbuf);
}

int 
main(
    int argc, 
    char **argv
    )
{
  int status = 0;
  struct evhttp *httpd;
  struct event_base *base;
  //--------------------------------------------
  g_fp = NULL;
  int itemp;
  if ( argc != 2 )  { go_BYE(-1); }
  status = stoI4(argv[1], &itemp); cBYE(status);
  if ( ( itemp <= 0 ) || ( itemp >= 65536 ) ) { go_BYE(-1); }
  uint16_t g_port = (uint16_t) itemp;
  g_verbose   = false;
  g_err       = false;
  g_num_posts = 0;
  g_sum_time  = 0;
  g_max_time  = LLONG_MIN;
  g_min_time  = LLONG_MAX;
  g_prev_time = 0;
  //--------------------------------------------
  base = event_base_new();
  httpd = evhttp_new(base);
  evhttp_set_max_headers_size(httpd, AB_MAX_HEADERS_SIZE);
  evhttp_set_max_body_size(httpd, AB_MAX_LEN_BODY);
  status = evhttp_bind_socket(httpd, "0.0.0.0", g_port); cBYE(status);
  evhttp_set_gencb(httpd, generic_handler, base);
  event_base_dispatch(base);    
  evhttp_free(httpd);
  event_base_free(base);
BYE:
  return status;
}

int
check_payload(
    const char * const cbuf
    )
{
  int status = 0;

  json_t *root = NULL;
  json_t *val  = NULL;
  json_error_t error;
  root = json_loads(cbuf, 0, &error);
  if ( root == NULL ) { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "uuid");
  if ( val == NULL ) { go_BYE(-1); }
  const char *uuid = json_string_value(val);
  if ( uuid == NULL )  { g_err = true; go_BYE(-1); }
  if ( strlen(uuid) > AB_MAX_LEN_UUID ) { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "version");
  const char *version = json_string_value(val);
  if ( version == NULL )  { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "test_id");
  const char *test_id = json_string_value(val);
  if ( test_id == NULL )  { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "variant_id");
  const char *variant_id = json_string_value(val);
  if ( variant_id == NULL )  { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "time");
  const char *time = json_string_value(val);
  if ( time == NULL )  { g_err = true; go_BYE(-1); }

  val = json_object_get(root, "in_tracer_id");
  const char *in_tracer_id = json_string_value(val);
  if ( in_tracer_id != NULL )  { 
    if ( !chk_tracer(in_tracer_id, AB_MAX_LEN_TRACER) ) { go_BYE(-1); }
  }

  val = json_object_get(root, "out_tracer_id");
  const char *out_tracer_id = json_string_value(val);
  if ( out_tracer_id != NULL )  { 
    if ( !chk_tracer(out_tracer_id, AB_MAX_LEN_TRACER) ) { go_BYE(-1); }
  }

  int32_t tempI4; int64_t tempI8; 
  status = stoI4(version, &tempI4); cBYE(status);
  if ( tempI4 < 1 ) { g_err = true; go_BYE(-1); }
  status = stoI4(test_id, &tempI4); cBYE(status);
  if ( tempI4 < 1 ) { g_err = true; go_BYE(-1); }
  status = stoI4(variant_id, &tempI4); cBYE(status);
  if ( tempI4 < 1 ) { g_err = true; go_BYE(-1); }
  status = stoI8(time, &tempI8);  cBYE(status); 
  if ( tempI8 < 1 ) { g_err = true; go_BYE(-1); }
  FILE *fp = NULL;
  if ( g_verbose ) { 
    if ( g_fp == NULL ) { fp = stdout; } else { fp = g_fp; }
    fprintf(fp, "%s,%s,%s,%s\n",
        uuid, test_id, variant_id ,time);
  }
  json_decref(root);
BYE:
  if ( status < 0 ) { fprintf(stderr, "%s\n", cbuf); }
  return status;
}
