#include "project.h"
#include "etl/all.h"
#include "oled/oled.h"
#include "etl/python_keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;
using namespace Project::etl::literals;

enum { EVENT_CLEAR, EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
auto event = Queue<int, 1> {};
auto oled = Oled { i2c2 };
auto &encoder = encoder1;
auto f = String {};

void mainThread(void *) {
    event.init();
    oled.init();

    exti.setCallback(button_up_Pin, [](auto) { event << EVENT_BT_UP; });
    exti.setCallback(button_down_Pin, [](auto) { event << EVENT_BT_DOWN; });
    exti.setCallback(button_right_Pin, [](auto) { event << EVENT_BT_RIGHT; });
    exti.setCallback(button_left_Pin, [](auto) { event << EVENT_BT_LEFT; });
    exti.setCallback(button_rot_Pin, [](auto) { event << EVENT_BT_ROT; });
    encoder.init([](auto) { event << EVENT_ROT_UP; }, nullptr,
                 [](auto) { event << EVENT_ROT_DOWN; });

    oled << "Hello World!\n";
    for (;;) {
        // wait event and print it to oled
        oled << f("%d", event.pop(osWaitForever));
    }
}

void project_init() {
    static Thread<256> thread;
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}
