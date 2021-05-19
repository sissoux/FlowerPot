#include "SoilHumidity.h"



uint8_t soilHumidity_ValueReady=0;
volatile uint16_t soilHumidity =0;

void soil_humidity_sensor_init(void)
{
    // Initialize touch pad peripheral.
    // The default fsm mode is software trigger mode.
    touch_pad_init();

    //Configure and start Soil humidity read
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(SOIL_SENSOR_PAD_PIN, 0);
    
    touch_pad_filter_start(100);
}

void soil_humidity_start_readings(void)
{
    xTaskCreate(&soil_humidity_read_task, "soil_humidity_read_task", 2048, NULL, 5, NULL);
}


/*
  Read value sensed on soil humidity pad (using touch functions).
 Print out values in a loop on a serial monitor.
 */
void soil_humidity_read_task(void *pvParameter)
{
    uint16_t touch_value;
    uint16_t touch_filter_value;
    while(1) 
    {
        touch_pad_read_raw_data(SOIL_SENSOR_PAD_PIN, &touch_value);
        touch_pad_read_filtered(SOIL_SENSOR_PAD_PIN, &touch_filter_value);
        //printf("Soil:[%4d,%4d]\n", touch_value, touch_filter_value);
        soilHumidity = touch_filter_value;
        soilHumidity_ValueReady = 1;
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

}