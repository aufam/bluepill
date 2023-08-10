#include "periph/i2c.h"

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    using namespace Project::periph;
    I2C *i2c;
    if (hi2c->Instance == i2c2.hi2c.Instance) i2c = &i2c2;
    else return;

    i2c->txCallback();
}
