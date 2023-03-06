#include "project.h"
#include "oled/oled.h"
#include "etl/all.h"
#include "etl/python_keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;
using namespace Project::etl::literals;

enum { EVENT_NONE, EVENT_BT_ROT, EVENT_ROT_CW, EVENT_ROT_CCW };
auto event = Queue<int, 1> {};
auto oled = Oled { i2c2 };
auto &encoder = encoder1;
auto f = String {};
auto &pwm = pwm3channel1;

bool pwmIsOn = false;
const int inc = 100;
int frequency = 200;
int dutyCycle = 50;

void setPWM() {
    pwm.setPrescaler(SystemCoreClock / 10'000 / frequency - 1);
    pwm.setPeriod(10'000 - 1);
    pwm.setPulse(10'000 * dutyCycle / 100 - 1);
}

void incFrequency(int val) {
    frequency += 5 * val;
    frequency = clamp(frequency, 1, 200);
    setPWM();
}

void incDutyCycle(int val) {
    dutyCycle += 5 * val;
    dutyCycle = clamp(dutyCycle, 5, 95);
    setPWM();
}

struct Option {
    char* (* text)();
    void (* add)(int);
    void print(bool invertColor) const { oled.print(text(), invertColor); }
};

const Array<Option, 3> options = {
        Option {
            []() { return f("PWM is %s", pwmIsOn ? "on" : "off"); },
            [](int) { }
            },
        Option {
            []{ return  f("Freq = %d", frequency); },
            incFrequency
            },
        Option {
            []{ return  f("Duty = %d", dutyCycle); },
            incDutyCycle
            }
};

void mainThread(void *) {
    event.init();
    oled.init();
    pwm.init(SystemCoreClock / 10'000 - 1, 10'000 - 1, 5'000 - 1);
    setPWM();
    encoder.init();

    exti.setCallback(button_rot_Pin, [](auto) { event << EVENT_BT_ROT; });
    encoder.setIncrementCB([](auto) { event << EVENT_ROT_CW; });
    encoder.setDecrementCB([](auto) { event << EVENT_ROT_CCW; });

    for (auto [optionIndex, editMode] = pair(0, false);;) {
        oled.setCursor(0, 0);
        for (auto [i, option] in enumerate(options)) {
            option.print(optionIndex == i);
            oled << '\n';
        }

        auto freq = (float) SystemCoreClock / (float) (pwm.getPeriod() + 1) / (float) (pwm.getPrescaler() + 1);
        auto val = division(static_cast<int>(freq * 100), 100);
        oled << f("Frequency = %d.%02d Hz\n", val.x, val.y);

        auto duty = (float) (pwm.getPulse() + 1) / (float) (pwm.getPeriod() + 1);
        val = division(static_cast<int>(duty * 100), 100);
        oled << f("Duty cycle = %d.%02d %%\n", val.x, val.y);

        int evt = event.pop(osWaitForever);
        switch (evt) {
            case EVENT_BT_ROT:
                if (optionIndex == 0) {
                    pwmIsOn = not pwmIsOn;
                    pwmIsOn ? pwm.start() : pwm.stop();
                    break;
                }
                editMode = not editMode;
                break;

            case EVENT_ROT_CW:
            case EVENT_ROT_CCW:
                if (not editMode) {
                    optionIndex += evt is EVENT_ROT_CW ? 1 : -1;
                    optionIndex = clamp(optionIndex, 0, (int) options.len() - 1);
                    break;
                }
                options[optionIndex].add(evt is EVENT_ROT_CW ? 1 : -1);

            case EVENT_NONE:
            default:
                break;
        }
    }
}

void project_init() {
    static Thread<256> thread;
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}
