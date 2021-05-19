#include "VEML7700.h"

bool VELM_initialized = false;
i2c_port_t i2c_num = _I2C_NUMBER;



/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    int i2c_port = _I2C_NUMBER;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_port, &conf);
    return i2c_driver_install(i2c_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

int VEML7700_init(void)
{
    i2c_master_init();
    ConfRegister.ALS_Gain = Gain_8th;
    ConfRegister.ALS_IntegrationTime = IT_100_ms;
    ConfRegister.ALS_Persistance = Pers_1;
    ConfRegister.shutdown = ON;
    VELM_initialized = true;
    return VEML7700_sendConfig(ConfRegister);
}

//Auto adjust ambiant light sensor. set gain to minimum, increase gain depending on raw level measured. if sensor is saturated set integration time to minimum and reperform gain adjustment
int VEML7700_autoAdjust(void)
{
    if (!VELM_initialized) return -1;
    ConfRegister.ALS_Gain = Gain_8th;
    ConfRegister.ALS_IntegrationTime = IT_100_ms;
    VEML7700_sendConfig(ConfRegister);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    float val = 0;
    uint16_t RawValue = VEML7700_getMeasurement(&val);
    if (RawValue == 65535)
    {
        ConfRegister.ALS_IntegrationTime = IT_25_ms;
        VEML7700_sendConfig(ConfRegister);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        RawValue = VEML7700_getMeasurement(&val);
    }
    if (RawValue < 3275) ConfRegister.ALS_Gain = Gain_2;
    else if (RawValue < 6550) ConfRegister.ALS_Gain = Gain_1;
    else if (RawValue < 26214) ConfRegister.ALS_Gain = Gain_4th;
    VEML7700_sendConfig(ConfRegister);
    return 0;
}

int VEML7700_stop(void)
{
    if(!VELM_initialized) return -1;
    ConfRegister.shutdown = OFF;
    return VEML7700_sendConfig(ConfRegister);
}

int VEML7700_sendConfig(struct CommandRegister configRegister)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, VEML7700_CONF_0_REG, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, *((uint8_t *)&configRegister), ACK_CHECK_EN);
    i2c_master_write_byte(cmd, *((uint8_t *)&configRegister+1), ACK_CHECK_EN);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

int VEML7700_getMeasurement(float *Illuminance)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t data_h;
    uint8_t data_l;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, VEML7700_ALS_REG, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data_l, ACK_VAL);
    i2c_master_read_byte(cmd, &data_h, NACK_VAL);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    *Illuminance = ((float)((data_h << 8) + data_l))*VEML7700_getGain(ConfRegister);
    
    return (data_h << 8) + data_l;
}


float VEML7700_getGain(struct CommandRegister regIn)
{
    float localGain = 0.0576;
    switch (regIn.ALS_Gain)
    {
    case Gain_1:
        localGain *= 1.0;
        break;
    case Gain_2:
        localGain *= 0.5;
        break;
    case Gain_4th:
        localGain *= 4.0;
        break;
    case Gain_8th:
        localGain *= 8.0;
        break;
    default:
        localGain *= 1.0;
        break;
    }

    switch (regIn.ALS_IntegrationTime)
    {
    case IT_100_ms:
        return localGain * 1.0;
        break;
    case IT_200_ms:
        return localGain * 0.5;
        break;
    case IT_400_ms:
        return localGain * 0.25;
        break;
    case IT_800_ms:
        return localGain * 0.125;
        break;
    case IT_50_ms:
        return localGain * 2.0;
        break;
    case IT_25_ms:
        return localGain * 4.0;
        break;
        
    default:
        return 1.0;
        break;
    }
}


