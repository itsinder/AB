return [=[{
  "AB": {
    "SESSION_SERVICE" : {
      "HEALTH_URL": {
        "COMMENT": "Health check URL for SS",
        "VALUE": "/health_check"
      },
      "PORT": {
        "COMMENT": "Port on which SS is running",
        "VALUE": 80
      },
      "SERVER": {
        "COMMENT": " Name of Server running SS",
        "VALUE": "logger"
      },
      "URL": {
        "COMMENT": "URL for SS",
        "VALUE": "\"session\""
      }
    },
    "LOGGER": {
      "HEALTH_URL": {
        "COMMENT": "Health check URL for DP",
        "VALUE": "/health_check/"
      },
      "PORT": {
        "COMMENT": "Port on which DP is listening",
        "VALUE": "80"
      },
      "SERVER": {
        "COMMENT": " Name of Server running DP",
        "VALUE": "logger"
      },
      "URL": {
        "COMMENT": "URL on which DP needs data POSTed",
        "VALUE": "/abserver/ABLog/"
      },
      "NUM_POST_RETRIES": {
        "COMMENT": "Number of times it will attempt to POST to log server.",
        "VALUE": "1"
      },
      "SZ_LOG_Q": {
        "COMMENT": "Size of queue of messages to Log Server.  Provides buffering so that POSTing to Log Server can be done by separate thread and does not slow down",
        "VALUE": "65536"
      }
    },
    "MYSQL": {
      "DATABASE": {
        "COMMENT": "Name of database in MySQL server",
        "VALUE": "abdb"
      },
      "PASSWORD": {
        "COMMENT": "Password for user for MySQL server",
        "VALUE": "toor"
      },
      "SERVER": {
        "COMMENT": "Name of MySQL server that back-ends the WebApp",
        "VALUE": "127.0.0.1"
      },
      "PORT": {
        "COMMENT": "Port on which the mysql server is listening",
        "VALUE": "3306"
      },
      "USER": {
        "COMMENT": "User for MySQL database",
        "VALUE": "root"
      }
    },
    "STATSD": {
      "SERVER": {
        "COMMENT": "setup for statsd",
        "VALUE": "localhost"
      },
      "STATSD_INC": {
        "COMMENT": "Determined by ...",
        "VALUE": "nw.metrics.ab.requests"
      },
      "PORT": {
        "COMMENT": "Default 8125. Determined by DataDog",
        "VALUE": "8125"
      },
      "STATSD_TIMING": {
        "COMMENT": "Determined by ...",
        "VALUE": "nw.metrics.ab.request_time"
      }
    },
    "DEFAULT_URL": {
      "COMMENT": "For URL Router",
      "VALUE": "https://www.nerdwallet.com"
    },
    "DEV_FILE": {
      "VALUE": "\"./device.csv\""
    },

    "PORT": {
      "COMMENT": "Port on which A/B server will run",
      "VALUE": "800"
    },
    "RELOAD_ON_STARTUP": {
      "COMMENT": "If true then Reload occurs when server comes up",
      "VALUE": "true"
    },
    "SZ_UUID_HT": {
      "COMMENT": "Number of unique UUIDs that can be supported.",
      "VALUE": "4194304"
    },
    "TEST_UUID_LEN": {
      "COMMENT": "If true",
      "VALUE": "false"
    },
    "UA_TO_DEV_MAP_FILE": {
      "VALUE": "\"./ua_to_dev.bin\""
    },
    "UDP_NODE": {
      "COMMENT": "Only for local testing",
      "VALUE": "localhost"
    },
    "UDP_SERVICE": {
      "COMMENT": "Only for local testing",
      "VALUE": "8012"
    },
    "UUID_LEN": {
      "COMMENT": "Maximum Length of UUID. If test_uuid_len=true",
      "VALUE": "36"
    },
    "VERBOSE": {
      "COMMENT": "Set to true if you want logs on stderr",
      "VALUE": "false"
    },
    "XY_GUID": {
      "COMMENT": "Used for testing.Set to 0 or omit this line in production",
      "VALUE": "1"
    }
  }
}
]=]