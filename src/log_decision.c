#include "ab_incs.h"
#include "ab_globals.h"
#include "auxil.h"
#include "post_url.h"
#include "make_curl_payload.h"
#include "log_decision.h"
#include "kafka_add_to_queue.h"
#include "statsd.h"
  
int
log_decision(
    void *X
    )
{
  int status = 0;
  if ( g_n_log_q >= g_cfg.sz_log_q ) { go_BYE(-1); }
  // Following if statement was put in to say that we would rather 
  // drop messages on the floor, then stall the main thread
  if ( g_n_log_q > g_cfg.sz_log_q - 2 ) {
    g_log_dropped_posts++; 
    STATSD_COUNT("dropped_posts", 1);
    goto BYE; 
  }

  pthread_mutex_lock(&g_mutex);	/* protect buffer */
  while ( g_n_log_q == g_cfg.sz_log_q ) {
    /* If there is no space in the buffer, then wait */
    fprintf(stderr, "producer waiting\n");
    pthread_cond_wait(&g_condp, &g_mutex);
    fprintf(stderr, "producer free to put in Q\n");
  }

  int eff_wr_idx = g_q_wr_idx % g_cfg.sz_log_q;
#ifdef AB_AS_KAFKA
  memcpy(g_log_q+eff_wr_idx, (KAFKA_REC_TYPE *)X,
      sizeof(KAFKA_REC_TYPE));
#else
  PAYLOAD_REC_TYPE payload;
  payload = *((PAYLOAD_REC_TYPE *)X);
  g_log_q[eff_wr_idx] = payload;
  // OLD g_log_q[eff_wr_idx] = lcl_payload;
#endif
  g_q_wr_idx++; 
  g_n_log_q++;
  // TODO P2 Does the order of the following 2 matter?
  pthread_mutex_unlock(&g_mutex);	/* release the buffer */
  pthread_cond_signal(&g_condc);	/* wake up consumer */
BYE:
  return status;
}
