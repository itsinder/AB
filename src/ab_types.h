#ifndef __ABTYPES_H
#define __ABTYPES_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <curl/curl.h>
#include "ab_constants.h"

typedef enum _ab_req_type {
  Undefined, // --- & ---
  AddFakeTest, // Read & C (for testing)
  AddTest, // Write &  Lua
  CheckLoggerConnectivity, // Read &  C
  CheckKafkaConnectivity, // Read &  C
  CheckDBConnectivity, // Config &  Lua
  Classify, // Read & C 
  ClassifyIP, // Read &  C
  ClassifyUA, // Read &  C
  Diagnostics, // Read &  C  AND Lua
  DumpLog, // Read &  C
  EvalDT, // Read & C
  GetConfig, // Read &  Lua
  GetDomain, // Debugging &  Lua
  GetHost, // Debugging &  Lua
  GetNumFeatures, // Read &  Lua
  GetVariant, // Read &  C
  GetVariants, // Read &  C
  Halt, // Read &  C
  HealthCheck, // Read &  C
  Ignore, // Read &  C
  IgnoreKafkaErrors, // Read &  C
  ListTests, // Read &  Lua
  LoadConfig, // Write &  Lua
  MakeFeatureVector, // Read &  Lua
  MdlMeta, // Read & Lua 
  NumTests, // Read &  Lua
  PingServer, // Read &  C
  PostProcPreds, // Read &  C
  Reload, // Write &  Lua
  Restart, // Read &  C
  Router, // Read &  C
  StopTest, // Write & C (for testing)
  TestInfo, // Read &  Lua
  ToKafka, // Pass through & C
  UTMKV, // Read &  C
  ZeroCounters // Write &  C
} AB_REQ_TYPE;

typedef struct _variant_rec_type {
  uint32_t id;
  float percentage;
  char separator; // either question mark or ampersand
  char name[AB_MAX_LEN_VARIANT_NAME+1];
  char * url; // AB_MAX_LEN_VARIANT_URL+1
  char * custom_data; // AB_MAX_LEN_CUSTOM_DATA+1
  int count; // as a quick and dirty check to see variant splitting
} VARIANT_REC_TYPE;

typedef struct _test_meta_type {
  char *test_as_str;
  char name[AB_MAX_LEN_TEST_NAME+1];
  int test_type; // whether AB_TEST_TYPE or XY_TEST_TYPE or ..
  uint32_t id; // external test id
  uint32_t ramp; // ramp, starts at 1 and increments every time percentage changes
  uint64_t name_hash; // set by Lua, read by C
  uint64_t external_id; // exposed to external entities
  bool has_filters; // has filters using categorical/boolean attributes
  bool is_dev_specific; // whether device specific
  int state;   // one of TEST_STATE_....
  uint64_t seed; // seed for spooky hash

  // Following are set by Lua, read by C
  uint32_t num_variants;
  VARIANT_REC_TYPE *variants;

  // If device specific is not set, we use device_idx = 0
  // above Should be same as g_n_justin_cat_lkp
  int *final_variant_id; // [g_n_justin_cat_lkp];
  int *final_variant_idx; // [g_n_justin_cat_lkp];
  uint8_t **variant_per_bin; // [g_n_justin_cat_lkp][AB_NUM_BINS];
  int **count_device_x_variant; // [g_n_justin_cat_lkp][num_variants];

} TEST_META_TYPE;

typedef struct _kafka_rec_type { 
  char *data;
  size_t sz;
  char out_tracer[AB_MAX_LEN_TRACER+1];
} KAFKA_REC_TYPE;

typedef struct _payload_rec_type {
  char uuid[AB_MAX_LEN_UUID+1];
  char in_tracer[AB_MAX_LEN_TRACER+1];
  char out_tracer[AB_MAX_LEN_TRACER+1];
  uint32_t ramp;
  uint64_t time;
  uint32_t test_id;
  uint32_t variant_id;
} PAYLOAD_REC_TYPE;

typedef struct _service_type {
  int32_t  port;
  char server[AB_MAX_LEN_SERVER_NAME+1];
  char url[AB_MAX_LEN_URL+1];
  char health_url[AB_MAX_LEN_URL+1];
} SERVICE_TYPE;

typedef struct _mysql_type {
  int32_t  port;
  char host[AB_MAX_LEN_MYSQL_PARAM+1];
  char user[AB_MAX_LEN_MYSQL_PARAM+1];
  char pass[AB_MAX_LEN_MYSQL_PARAM+1];
  char db[AB_MAX_LEN_MYSQL_PARAM+1];
} MYSQL_TYPE;

typedef struct _kafka_cfg_type {
  char brokers[AB_MAX_LEN_KAFKA_PARAM+1];
  char topic[AB_MAX_LEN_KAFKA_PARAM+1];
  char retries[AB_MAX_LEN_KAFKA_PARAM+1];
  char max_buffering_time[AB_MAX_LEN_KAFKA_PARAM+1];
} KAFKA_CFG_TYPE;

typedef struct _statsd_keys_cfg_type {
  char inc[AB_MAX_LEN_STATSD_KEY+1];
  char count[AB_MAX_LEN_STATSD_KEY+1];
  char gauge[AB_MAX_LEN_STATSD_KEY+1];
  char timing[AB_MAX_LEN_STATSD_KEY+1];
} STATSD_KEYS_CFG_TYPE;

typedef struct _cfg_type {

  char mmdb_file[AB_MAX_LEN_FILE_NAME+1]; // For MaxMind

  char ua_dir[AB_MAX_LEN_FILE_NAME+1]; // For classifying user agent

  char dt_dir[AB_MAX_LEN_FILE_NAME+1]; // For decision tree
  char model_name[AB_MAX_LEN_FILE_NAME+1]; // For decision tree


  SERVICE_TYPE logger;
  MYSQL_TYPE mysql;
  SERVICE_TYPE statsd;
  SERVICE_TYPE webapp;
  KAFKA_CFG_TYPE kafka;

  int32_t sz_log_q;
  int32_t  port;  // port on which AB RTS will run
  int max_len_uuid;
  int num_post_retries;
  bool verbose;    // how chatty should RTS be
  char default_url[AB_MAX_LEN_REDIRECT_URL+1];

  STATSD_KEYS_CFG_TYPE statsd_keys;

} CFG_TYPE;

typedef struct _justin_map_rec_type {
  char justin_cat[AB_MAX_LEN_LKP_NAME+1];
  char device[AB_MAX_LEN_LKP_NAME+1];
  char device_type[AB_MAX_LEN_LKP_NAME+1];
  char os[AB_MAX_LEN_LKP_NAME+1];
  char browser[AB_MAX_LEN_LKP_NAME+1];
} JUSTIN_MAP_REC_TYPE;

typedef struct _maxmind_rec_type {
  char postal_code[AB_MAX_LEN_POSTAL_CODE+1]; // "postal", "code"
  char time_zone[AB_MAX_LEN_TIME_ZONE+1]; // "location", "time_zone"
  char country[4]; // "country", "iso_code"
  char state[AB_MAX_LEN_STATE+1]; // "subdivisions", "names", "en"
  char city[AB_MAX_LEN_CITY+1]; // "city", "names", "en"
} MAXMIND_REC_TYPE;

#endif

