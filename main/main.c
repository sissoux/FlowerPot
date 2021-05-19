/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "FlowerPotWifi.h"
#include "SoilHumidity.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//static const char *device_name = "MyFlowerPot_1.0";

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;


#define SENSORS_READ_PERIOD 5 //s
#define WIFI_SEND_PERIOD 4 //times the read period

#define DEEP_SLEEP_SEC SENSORS_READ_PERIOD
RTC_DATA_ATTR uint16_t SoilHumidityBuffer[WIFI_SEND_PERIOD];
//ADD TIMESTAMP BUFFER



void app_main(void)
{
    soil_humidity_sensor_init();
    soil_humidity_start_readings();
    while(!soilHumidity_ValueReady) vTaskDelay(200 / portTICK_PERIOD_MS);
    printf("Soil value is ready: %4d\n",soilHumidity);
    SoilHumidityBuffer[boot_count] = soilHumidity;

    boot_count++;
    if (boot_count==WIFI_SEND_PERIOD)
    {
      boot_count = 0;
      wifi_init_nvs();
      wifi_init_sta();
      for (int i =0; i < WIFI_SEND_PERIOD; i++)
      {
        printf("Soil[%d] = %4d\n", i, SoilHumidityBuffer[i]);
      }
      //IF WIFI UNAVAILABLE STORE IN FLASH
      //SYNC UP TIME
      //SEND DATA
      esp_wifi_stop();
    }

    ESP_LOGI(TAG, "Boot number: %d.\nEntering deep sleep for %d seconds", boot_count, DEEP_SLEEP_SEC);
    esp_deep_sleep(1000000LL * DEEP_SLEEP_SEC); 

}
