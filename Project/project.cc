#include "project.h"
#include "etl/python_keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;

/// override operator new with malloc from @p heap_4.c
void *operator new(size_t size) { return pvPortMalloc(size); }
/// override operator delete with free from @p heap_4.c
void operator delete(void *ptr) { vPortFree(ptr); }

enum { EVENT_NONE, EVENT_BT_ROT, EVENT_ROT_CW, EVENT_ROT_CCW };
auto event = Queue<int, 1> {};
auto oled = Oled { i2c2 };
auto &encoder = encoder1;
auto f = String {};
auto &pwm = pwm3channel1;
bool pwmIsOn = false;
const int inc = 100;

struct Option {
    char* (* text)();
    void (* add)(int);
    void print(bool invertColor) const { oled.print(text(), invertColor); }
};

const Array<Option, 4> options = {
        Option {
            []() { return f("PWM is %s", pwmIsOn ? "on" : "off"); },
            [](int val) { }
            },
        Option {
            []() { return f("PSC = %u", pwm.getPrescaler()); },
            [](int val) { pwm.setPrescaler(pwm.getPrescaler() + val); }
            },
        Option {
            []() { return f("ARR = %u", pwm.getPeriod()); },
            [](int val) { pwm.setPeriod(pwm.getPeriod() + val); }
            },
        Option {
            []() { return f("CCR = %u", pwm.getPulse()); },
            [](int val) { pwm.setPulse(pwm.getPulse() + val); }
            }
};

void mainThread(void *) {
    event.init();
    oled.init();
    pwm.init(SystemCoreClock / 10'000 - 1, 10'000 - 1, 5'000 - 1);
    encoder.init();

    exti.setCallback(button_rot_Pin, [](auto) { event << EVENT_BT_ROT; });
    encoder.setIncrementCB([](auto) { event << EVENT_ROT_CW; });
    encoder.setDecrementCB([](auto) { event << EVENT_ROT_CCW; });

    size_t optionIndex = 0;
    bool editMode = false;
    for (;;) {
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
                    pwmIsOn = !pwmIsOn;
                    pwmIsOn ? pwm.start() : pwm.stop();
                    break;
                }
                editMode = not editMode;
                break;

            case EVENT_ROT_CW:
            case EVENT_ROT_CCW:
                if (not editMode) {
                    optionIndex += evt is EVENT_ROT_CW ? 1 : -1;
                    optionIndex = clamp(optionIndex, 0u, options.len() - 1);
                    break;
                }
                options[optionIndex].add(evt is EVENT_ROT_CW ? inc : -inc);

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
