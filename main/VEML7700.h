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
extern bool VELM_initialized;


esp_err_t i2c_master_init(void);
int VEML7700_init(void);
int VEML7700_autoAdjust(void);
int VEML7700_stop(void);
int VEML7700_sendConfig(struct CommandRegister configRegister);
int VEML7700_getMeasurement(float *Illuminance);
float VEML7700_getGain(struct CommandRegister);

#endif