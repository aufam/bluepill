#include "periph/i2c.h"

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    using namespace Project::Periph;
    I2C *i2c;
    if (hi2c->Instance == i2c2.hi2c.Instance) i2c = &i2c2;
    else return;

    auto& cb = i2c->txCallback;
    if (cb.fn) cb.fn(cb.arg);
}
