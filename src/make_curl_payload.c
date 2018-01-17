#include <stdio.h>
#include "ab_constants.h"
#include "ab_types.h"
#include "macros.h"
#include "ab_globals.h"
#include "auxil.h"
#include "make_curl_payload.h"

/* Reads lcl_payload and writes to a string (curl_payload) that 
 will be POSTed to log server */
//<hdr>
int 
make_curl_payload(
    PAYLOAD_TYPE lcl_payload,
    char *curl_payload
    )
//</hdr>
{
  int status = 0;

  memset(g_curl_payload,            '\0', AB_MAX_LEN_PAYLOAD+1);
  size_t sz = snprintf(curl_payload, AB_MAX_LEN_PAYLOAD, 
"{  \
\"uuid\" : \"%s\", \
\"test_id\" : \"%u\", \
\"variant_id\" : \"%d\", \
\"in_tracer_id\" : \"%s\", \
\"out_tracer_id\" : \"%s\", \
\"time\" : \"%" PRIu64 "\" }",
      lcl_payload.uuid,
      lcl_payload.test_id, 
      lcl_payload.variant_id,
      lcl_payload.in_tracer,
      lcl_payload.out_tracer,
      lcl_payload.time
      ); 
  if ( sz >= AB_MAX_LEN_PAYLOAD ) { go_BYE(-1); }
  // TODO Log buffer overflow
BYE:
  return status;
}