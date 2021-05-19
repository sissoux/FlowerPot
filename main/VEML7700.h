/* Driver for Vishay VEML7700 ambiant light sensor*/


#ifndef H_VEML7700
#define H_VEML7700
#include "driver/i2c.h"

#define VEML7700_I2C_ADDRESS 0x10

#define VEML7700_CONF_0_REG 0x00
#define VEML7700_ALS_WH_REG 0x01
#define VEML7700_ALS_WL_REG 0x02
#define VEML7700_ALS_REG 0x04
#define VEML7700_WHITE_REG 0x05
#define VEML7700_ALS_INT_REG 0x06


#define _I2C_NUMBER I2C_NUM_0
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define WRITE_BIT 0x00                          /*!< I2C master write */
#define READ_BIT 0x01                           /*!< I2C master read */
#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

enum ALS_GAIN{
    Gain_1=0,
    Gain_2,
    Gain_8th,
    Gain_4th
};

enum ALS_IT{
    IT_100_ms=0x00,
    IT_200_ms,
    IT_400_ms,
    IT_800_ms,
    IT_25_ms=0x0C,
    IT_50_ms=0x08
};

enum ALS_PERS{
    Pers_1=0,
    Pers_2,
    Pers_4,
    Pers_8
};

enum ALS_SD{
    ON=0,
    OFF,
};


struct CommandRegister{
    enum ALS_SD shutdown:1;
    int :3;
    enum ALS_PERS ALS_Persistance:2;
    enum ALS_IT ALS_IntegrationTime:4;
    int :1;
    enum ALS_GAIN ALS_Gain:2;
    int :3;
}__attribute__((packed));

struct CommandRegister ConfRegister;

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
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

int StartMeasurement(i2c_port_t i2c_num)
{
    i2c_master_init();
    ConfRegister.ALS_Gain = Gain_1;
    ConfRegister.ALS_IntegrationTime = IT_100_ms;
    ConfRegister.ALS_Persistance = Pers_1;
    ConfRegister.shutdown = ON;
    
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, VEML7700_CONF_0_REG, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, *((uint8_t *)&ConfRegister), ACK_CHECK_EN);
    i2c_master_write_byte(cmd, *((uint8_t *)&ConfRegister+1), ACK_CHECK_EN);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
/*
    cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x03, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);*/
    return ret;
}

int getMeasurement(i2c_port_t i2c_num, float *Illuminance)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t data_h;
    uint8_t data_l;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, VEML7700_ALS_REG, ACK_CHECK_EN);
    //i2c_master_stop(cmd);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, VEML7700_I2C_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data_l, ACK_VAL);
    i2c_master_read_byte(cmd, &data_h, NACK_VAL);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    *Illuminance = ((float)((data_h << 8) + data_l))*0.0576;
    return ret;
}




#endif