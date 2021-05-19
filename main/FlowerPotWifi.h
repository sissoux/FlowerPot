#ifndef H_FLOWERPOT_WIFI_
#define H_FLOWERPOT_WIFI_

#include "esp_wifi.h"
#include "esp_event.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_log.h"
#include "nvs_flash.h"


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define WIFI_SSID "alexwifiap"
#define WIFI_PW "motdepasse"
#define WIFI_MAXIMUM_RETRY 5

static const char *TAG = "wifi station";

static int s_retry_num = 0;

void wifi_init_nvs(void);
void wifi_init_sta(void);
void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

#endif