#include "kafka_check_conn.h"


int kafka_check_conn(char* buf, size_t len){
  int status = 0;
retry:
  if (g_rk == NULL) {
    go_BYE(-1);
  }
  // TODO put some message in the g_buf
  g_kafka_callback = 0 ;
  if (rd_kafka_produce(
        /* Topic object */
        g_rkt,
        /* Use builtin partitioner to select partition*/
        RD_KAFKA_PARTITION_UA,
        /* Make a copy of the payload. */
        RD_KAFKA_MSG_F_COPY,
        /* Message payload (value) and length */
        buf, len,
        /* Optional key and its length */
        NULL, 0,
        /* Message opaque, provided in
         * delivery report callback as
         * msg_opaque. */
        1) == -1) {
    /**
     * Failed to *enqueue* message for producing.
     */
    fprintf(stderr,
        "%% Failed to produce to topic %s: %s\n",
        rd_kafka_topic_name(g_rkt),
        rd_kafka_err2str(rd_kafka_last_error()));
    // add to g_err

    /* Poll to handle delivery reports */
    if (rd_kafka_last_error() ==
        RD_KAFKA_RESP_ERR__QUEUE_FULL) {
      /* If the internal queue is full, wait for
       * messages to be delivered and then retry.
       * The internal queue represents both
       * messages to be sent and messages that have
       * been sent or failed, awaiting their
       * delivery report callback to be called.
       *
       * The internal queue is limited by the
       * configuration property
       * queue.buffering.max.messages */
      rd_kafka_poll(g_rk, 10000/*block for max 1000ms*/);
      goto retry;
    }
    go_BYE(-1);
  } 
  else {
    /* fprintf(stderr, "%% Enqueued message (%zd bytes) "
        "for topic %s\n",
        len, rd_kafka_topic_name(g_rkt));
        */
  }


  /* A producer application should continually serve
   * the delivery report queue by calling rd_kafka_poll()
   * at frequent intervals.
   * Either put the poll call in your main loop, or in a
   * dedicated thread, or call it after every
   * rd_kafka_produce() call.
   * Just make sure that rd_kafka_poll() is still called
   * during periods where you are not producing any messages
   * to make sure previously produced messages have their
   * delivery report callback served (and any other callbacks
   * you register). */
  rd_kafka_poll(g_rk, 0/*non-blocking*/);
  rd_kafka_flush(g_rk, -1);
  rd_kafka_poll(g_rk, 1*1000); // wait for buffer to flush
  if (g_kafka_callback != 0) {
    g_kafka_callback = 0;
    go_BYE(-1);
  }

BYE:
  return status;
}