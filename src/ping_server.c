#include "ab_incs.h"
#include "auxil.h"
#include "ab_globals.h"
#include "ping_server.h"
//<hdr>
int 
ping_server(
    const char *service,
    const char *server,
    int port,
    const char *url,
    char *rslt
    )
//</hdr>
{
  int status = 0;
  CURL *ch = NULL; CURLcode curl_res;
  char *buffer = NULL; int buflen = 0;
  long http_code;

  double time = 0;
  bool is_server = true;
  if ( ( server == NULL ) || ( *server == '\0' ) )  {
    is_server = false;
  }
  if ( ( port <= 0 ) || ( port >= 65536 ) ) { 
    is_server = false;
  }
  if ( ( url == NULL ) || ( *url == '\0' ) )  {
    is_server = false;
  }
  if ( is_server == false ) { 
    fprintf(stderr, "log server not identified. Not checking\n");
    goto BYE;
  }
  ch = curl_easy_init();
  buflen = strlen(server) + strlen(url) + 32;
  buffer = malloc(buflen * sizeof(char));
  return_if_malloc_failed(buffer);

  int nw = snprintf(buffer, buflen, "http://%s:%d/%s", server, port, url);
  if ( nw >= buflen ) { go_BYE(-1); }
  curl_easy_setopt(ch, CURLOPT_URL, buffer);
  curl_easy_setopt(ch, CURLOPT_TIMEOUT_MS, 1000);
  curl_res = curl_easy_perform(ch);
  if ( curl_res != CURLE_OK ) { 
    fprintf(stderr, "Failure at [%s] \n", buffer);
    go_BYE(-1); 
  }
  curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &http_code);
  if ( http_code != 200 ) { 
    fprintf(stderr, "Failure at [%s] \n", buffer);
    go_BYE(-1); 
  }
  curl_easy_getinfo(ch, CURLINFO_TOTAL_TIME, &time);
BYE:
  if ( rslt != NULL ) { 
    if ( status < 0 ) { 
      sprintf(rslt, "{ \"Ping\" : \"ERROR\", \"Service\" : \"%s\", \"Server\" : \"%s\", \"Port\" : %d, \"URL\" : \"%s\" }", service, server, port, url);
    }
    else {
      if ( time == 0 ) { 
        sprintf(rslt, "{ \"Ping\" : \"LOG SERVER NOT SET\" }");
      }
      else {
        sprintf(rslt, "{ \"Ping\" : \"OK\", \"Service\" : \"%s\", \"Server\" : \"%s\", \"Port\" : %d, \"URL\" : \"%s\", \"Time\" : \"%lf\" }", service, server, port, url, time);
      }
    }
  }
  free_if_non_null(buffer);
  curl_easy_cleanup(ch); 
  return status;
}
