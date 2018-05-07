/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2017, Magnus Edenhill
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Simple Apache Kafka producer
 * using the Kafka driver from librdkafka
 * (https://github.com/edenhill/librdkafka)
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>


/* Typical include path would be <librdkafka/rdkafka.h>, but this program
 * is builtin from within the librdkafka source tree and thus differs. */
#include "rdkafka.h"

rd_kafka_t *g_rk;         /* Producer instance handle */
rd_kafka_topic_t *g_rkt;  /* Topic object */
rd_kafka_conf_t *g_conf;  /* Temporary configuration object */
char g_errstr[512];       /* librdkafka API error reporting buffer */
char g_buf[512];          /* Message value temporary buffer */



static int run = 1;

/**
 * @brief Signal termination of program
 */
static void stop (int sig) {
	run = 0;
	fclose(stdin); /* abort fgets() */
}

static void close() {
	/* Wait for final messages to be delivered or fail.
	 * rd_kafka_flush() is an abstraction over rd_kafka_poll() which
	 * waits for all messages to be delivered. */
	fprintf(stderr, "%% Flushing final messages..\n");
	rd_kafka_flush(g_rk, 10*1000 /* wait for max 10 seconds */);

	/* Destroy topic object */
	rd_kafka_topic_destroy(g_rkt);

	/* Destroy the producer instance */
	rd_kafka_destroy(g_rk);
	g_rk = NULL;
	g_rkt = NULL;
	g_conf = NULL;
}


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
static void dr_msg_cb (rd_kafka_t *g_rk,
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


int setup(const char* topic, const char* brokers) {
	/*
	 * Create Kafka client configuration place-holder
	 */
	g_conf = rd_kafka_conf_new();

	/* Set bootstrap broker(s) as a comma-separated list of
	 * host or host:port (default port 9092).
	 * librdkafka will use the bootstrap brokers to acquire the full
	 * set of brokers from the cluster. */
	if (rd_kafka_conf_set(g_conf, "bootstrap.servers", brokers,
				g_errstr, sizeof(g_errstr)) != RD_KAFKA_CONF_OK) {
		fprintf(stderr, "%s\n", g_errstr);
		// add error to g_err
		return 1;
	}

	char* queue_size = "1000000"; // max 10000000
	/* Set message queue size */
	if (rd_kafka_conf_set(g_conf, "queue.buffering.max.messages", queue_size,
				g_errstr, sizeof(g_errstr)) != RD_KAFKA_CONF_OK) {
		fprintf(stderr, "%s\n", g_errstr);
		// add error to g_err
		return 1;
	}

	/* Set max retries before returning an error */
	char* retries = "5"; // max 10000000
	/* Set message queue size */
	if (rd_kafka_conf_set(g_conf, "message.send.max.retries", retries,
				g_errstr, sizeof(g_errstr)) != RD_KAFKA_CONF_OK) {
		fprintf(stderr, "%s\n", g_errstr);
		// add error to g_err
		return 1;
	}



	/* Set the delivery report callback.
	 * This callback will be called once per message to inform
	 * the application if delivery succeeded or failed.
	 * See dr_msg_cb() above. */
	rd_kafka_conf_set_dr_msg_cb(g_conf, dr_msg_cb);


	/*
	 * Create producer instance.
	 *
	 * NOTE: rd_kafka_new() takes ownership of the conf object
	 *       and the application must not reference it again after
	 *       this call.
	 */
	g_rk = rd_kafka_new(RD_KAFKA_PRODUCER, g_conf, g_errstr, sizeof(g_errstr));
	if (!g_rk) {
		fprintf(stderr,
				"%% Failed to create new producer: %s\n", g_errstr);
		// add err to g_err
		return 1;
	}


	/* Create topic object that will be reused for each message
	 * produced.
	 *
	 * Both the producer instance (rd_kafka_t) and topic objects (topic_t)
	 * are long-lived objects that should be reused as much as possible.
	 */
	g_rkt = rd_kafka_topic_new(g_rk, topic, NULL);
	if (!g_rkt) {
		fprintf(stderr, "%% Failed to create topic object: %s\n",
				rd_kafka_err2str(rd_kafka_last_error()));
		rd_kafka_destroy(g_rk);
		// add to g_err
		return 1;
	}



}

static void send_message(){
	/*
	 * Send/Produce message.
	 * This is an asynchronous call, on success it will only
	 * enqueue the message on the internal producer queue.
	 * The actual delivery attempts to the broker are handled
	 * by background threads.
	 * The previously registered delivery report callback
	 * (dr_msg_cb) is used to signal back to the application
	 * when the message has been delivered (or failed).
	 */
	size_t len = strlen(g_buf);
retry:
	if (rd_kafka_produce(
				/* Topic object */
				g_rkt,
				/* Use builtin partitioner to select partition*/
				RD_KAFKA_PARTITION_UA,
				/* Make a copy of the payload. */
				RD_KAFKA_MSG_F_COPY,
				/* Message payload (value) and length */
				g_buf, len,
				/* Optional key and its length */
				NULL, 0,
				/* Message opaque, provided in
				 * delivery report callback as
				 * msg_opaque. */
				NULL) == -1) {
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
			rd_kafka_poll(g_rk, 1000/*block for max 1000ms*/);
			goto retry;
		}
	} else {
		fprintf(stderr, "%% Enqueued message (%zd bytes) "
				"for topic %s\n",
				len, rd_kafka_topic_name(g_rkt));
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

}

int main (int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "%% Usage: %s <broker> <topic>\n", argv[0]);
		return 1;
	}

	const char* brokers = argv[1];
	const char* topic   = argv[2];
	setup(topic, brokers);


	/* Signal handler for clean shutdown */
	signal(SIGINT, stop);

	fprintf(stderr,
			"%% Type some text and hit enter to produce message\n"
			"%% Or just hit enter to only serve delivery reports\n"
			"%% Press Ctrl-C or Ctrl-D to exit\n");

	while (run && fgets(g_buf, sizeof(g_buf), stdin)) {
		size_t len = strlen(g_buf);

		if (g_buf[len-1] == '\n') /* Remove newline */
			g_buf[--len] = '\0';

		if (len == 0) {
			/* Empty line: only serve delivery reports */
			rd_kafka_poll(g_rk, 0/*non-blocking */);
			continue;
		}
		send_message();
	}

	close();
	return 0;
}