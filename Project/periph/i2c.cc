#include "periph/i2c.h"

using namespace Project::periph;

static I2C* selector(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == i2c2.hi2c.Instance) 
        return &i2c2;
    return nullptr;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    auto i2c = selector(hi2c);
    if (i2c) 
        i2c->txCallback();
}
