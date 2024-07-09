#include "main.hpp"

namespace Project {
    etl::Tasks tasks;
    etl::Mutex mutex;
    etl::String<128> f;

    Oled oled { .hi2c=hi2c2 };
    wizchip::Ethernet ethernet({
        .hspi=hspi1,
        .cs={.port=CS_GPIO_Port, .pin=CS_Pin},
        .rst={.port=RESET_GPIO_Port, .pin=RESET_Pin},
        .netInfo={ 
            .mac={0x00, 0x08, 0xdc, 0xff, 0xee, 0xdd},
            .ip={10, 20, 30, 2},
            .sn={255, 255, 255, 0},
            .gw={10, 20, 30, 1},
            .dns={10, 20, 30, 1},
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

using namespace Project;

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
    ethernet.logger.function = [](const char* str) { 
        auto lock = mutex.lock().await();
        oled.setCursor({0, 1});
        oled << str;
    };

    App::run("*");
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

App::App(const char* name, App::function_t test) {
    if (name == etl::string_view("")) {
        panic("App name cannot be empty");
    }
    if (cnt == 16) {
        panic("App buffer is full");
    }
    functions[cnt] = test;
    names[cnt++] = name;
}

void App::run(const char* fil) {
    auto filter = etl::string_view(fil);
    if (filter.len() == 0) {
        panic("App run filter token is not provided");
    }
    
    for (int i = 0; i < cnt; ++i) {
        auto test = functions[i];
        auto name = names[i];

        if (filter == name or (filter.back() == '*' and ::strncmp(name, filter.data(), filter.len() - 1) == 0))
            test();
    }
}

App::function_t App::functions[16] = {};
const char* App::names[16] = {};
int App::cnt = 0;
