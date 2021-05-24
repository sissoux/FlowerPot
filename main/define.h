
#ifndef H_DEFINE
#define H_DEFINE

#define ENABLED_WIFI false

#define SENSORS_READ_PERIOD 8 //s
#define WIFI_SEND_PERIOD 5 //times the read period

#define DEEP_SLEEP_SEC SENSORS_READ_PERIOD

static const char *DEVICE_NAME = "MyFlowerPot_1.0";
static const char *ENV_TAG = "Env sensor";
static const char *MQTT_TAG = "Env sensor";


#endif