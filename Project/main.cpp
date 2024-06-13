#include "main.hpp"
#include "etl/keywords.h"

namespace Project {
    etl::Tasks tasks;
    etl::Mutex mutex; // global mutex
    etl::String<128> f; // global text formatter

    Oled oled { .i2c=periph::i2c2 };
    wizchip::Ethernet ethernet({
        .hspi=hspi1,
        .cs={.port=CS_GPIO_Port, .pin=CS_Pin},
        .rst={.port=RESET_GPIO_Port, .pin=RESET_Pin},
        .netInfo={ 
            .mac={0x00, 0x08, 0xdc, 0xff, 0xee, 0xdd},
            .ip={192, 168, 151, 248},
            .sn={255, 255, 255, 0},
            .gw={192, 168, 151, 1},
            .dns={1, 1, 1, 1},
            .dhcp=NETINFO_STATIC,
        },
    });
}

namespace Project::periph {
    ADCD adc1 { .hadc=hadc1 };
    Encoder encoder1 { .htim=htim1 };
    I2C i2c2 { .hi2c=hi2c2 };
    PWM pwm3channel1 { .htim=htim3, .channel=TIM_CHANNEL_1 };
    UART uart1 { .huart=huart1 };
    UART uart2 { .huart=huart2 };

    #ifdef F103_USE_CAN
    CAN can { .hcan=hcan };
    #endif
}

[[async]]
static void heap_info() {
    for (;;) {
        etl::this_thread::sleep(100ms);
        auto lock = mutex.lock().await();
        
        oled.setCursor({0, 0});
        oled << f("h: %lu/%lu t: %lu\n", etl::heap::freeSize.get(), etl::heap::totalSize.get(), tasks.resources());
        oled.setCursor({0, 4});
    }
}

[[test]]
extern void test_http_server();

extern "C" void project_init() {
    HAL_Delay(50);
    periph::adc1.init();
    periph::encoder1.init();
    periph::i2c2.init();
    periph::pwm3channel1.init();
    periph::uart1.init();
    periph::uart2.init();

    #ifdef F103_USE_CAN
    periph::can.init();
    #endif

    tasks.init();
    oled.init();
    mutex.init();
    ethernet.init();
    ethernet.logger.function = [] (const char* str) { 
        auto lock = mutex.lock().await();
        oled.setCursor({0, 1});
        oled << str;
    };

    test_http_server();
    etl::async(&heap_info);
}

extern "C" void panic(const char* msg) {
    tasks.terminate();
    portDISABLE_INTERRUPTS();
    __disable_irq();

    oled.clear();
    oled.setCursor({0, 0});
    oled << msg << '\n'; // print panic message to oled

    uint32_t psc;
    switch (IWDG->PR) {
        case IWDG_PRESCALER_4:   psc = 4;   break;
        case IWDG_PRESCALER_8:   psc = 8;   break;
        case IWDG_PRESCALER_16:  psc = 16;  break;
        case IWDG_PRESCALER_32:  psc = 32;  break;
        case IWDG_PRESCALER_64:  psc = 64;  break;
        case IWDG_PRESCALER_128: psc = 128; break;
        case IWDG_PRESCALER_256: psc = 256; break;
        default: psc = 0;
    }
    auto restart_time = ((IWDG->RLR) * psc) / 32000;
    oled << f("Restart in %d s\n", restart_time, IWDG->RLR);
    
    for (;;);
}
