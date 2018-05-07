#include "kafka_msg_callback.h"

/**
 * @brief Message delivery report callback.
 *
 * This callback is called exactly once per message, indicating if
 * the message was succesfully delivered
 * (rkmessage->err == RD_KAFKA_RESP_ERR_NO_ERROR) or permanently
 * failed delivery (rkmessage->err != RD_KAFKA_RESP_ERR_NO_ERROR).
 *
 * The callback is triggered from rd_kafka_poll() and executes on
 * the application's thread.
 */
void  kafka_msg_callback(rd_kafka_t *g_rk,
    const rd_kafka_message_t *rkmessage, void *opaque) {
  if (rkmessage->err){
    fprintf(stderr, "%% Message delivery failed: %s\n",
        rd_kafka_err2str(rkmessage->err));
    // count in statsd
  }
  //else
  //        fprintf(stderr,
  //                "%% Message delivered (%zd bytes, "
  //                "partition %"PRId32")\n",
  //                rkmessage->len, rkmessage->partition);

  /* The rkmessage is destroyed automatically by librdkafka */
}

