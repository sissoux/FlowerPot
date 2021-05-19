#ifndef H_SOIL_HUMIDITY
#define H_SOIL_HUMIDITY

#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SOIL_SENSOR_PAD_PIN TOUCH_PAD_NUM2

extern uint8_t soilHumidity_ValueReady;
extern volatile uint16_t soilHumidity;

/*
  Read value sensed on soil humidity pad (using touch functions).
 Print out values in a loop on a serial monitor.
 */
void soil_humidity_read_task(void *pvParameter);
void soil_humidity_sensor_init(void);
void soil_humidity_start_readings(void);

#endif