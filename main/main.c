/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "FlowerPotWifi.h"
#include "SoilHumidity.h"
#include "VEML7700.h"


#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define ENABLED_WIFI false


static const char *DEVICE_NAME = "MyFlowerPot_1.0";
static const char *ENV_TAG = "Env sensor";


/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;


#define SENSORS_READ_PERIOD 8 //s
#define WIFI_SEND_PERIOD 5 //times the read period

#define DEEP_SLEEP_SEC SENSORS_READ_PERIOD
RTC_DATA_ATTR uint16_t SoilHumidityBuffer[WIFI_SEND_PERIOD];
RTC_DATA_ATTR float IlluminanceBuffer[WIFI_SEND_PERIOD];

//ADD TIMESTAMP BUFFER



void app_main(void)
{
  soil_humidity_sensor_init();
  soil_humidity_start_readings();
  while(!soilHumidity_ValueReady) vTaskDelay(1 / portTICK_PERIOD_MS);
  ESP_LOGI(ENV_TAG, "Soil value is ready: %4d",soilHumidity);
  SoilHumidityBuffer[boot_count] = soilHumidity;

  float Illuminance = 0.0;
  VEML7700_init();
  VEML7700_autoAdjust();
  vTaskDelay(150 / portTICK_PERIOD_MS); //Max integration time is 100ms ==> Wait 150ms for new measurement to be available
  VEML7700_getMeasurement(&Illuminance);
  ESP_LOGI(ENV_TAG,"Got illuminance value: %.2f lux",Illuminance);
  VEML7700_stop();
  IlluminanceBuffer[boot_count] = Illuminance;

  boot_count++;
  if (boot_count==WIFI_SEND_PERIOD)
  {
    boot_count = 0;
    if (ENABLED_WIFI)
    {
      wifi_init_nvs();
      wifi_init_sta();
    }
    for (int i =0; i < WIFI_SEND_PERIOD; i++)
    {
      printf("[%d] ==> Soil = %4d, Illum = %.2f lux\n", i, SoilHumidityBuffer[i], IlluminanceBuffer[i]);
    }
    //IF WIFI UNAVAILABLE STORE IN FLASH
    //SYNC UP TIME
    //SEND DATA
    if (ENABLED_WIFI)
    {
      esp_wifi_stop();
    }
  }


  ESP_LOGI(DEVICE_NAME, "Boot %d/%d. Entering deep sleep for %d seconds", boot_count, WIFI_SEND_PERIOD, DEEP_SLEEP_SEC);
  esp_deep_sleep(1000000LL * DEEP_SLEEP_SEC); 

}
