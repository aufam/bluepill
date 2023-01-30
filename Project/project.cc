#include "project.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;

/// override operator new with malloc from @p heap_4.c
void *operator new(size_t size) { return pvPortMalloc(size); }
/// override operator delete with free from @p heap_4.c
void operator delete(void *ptr) { vPortFree(ptr); }

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
        int evt = event.pop(osWaitForever);
        oled << f("%d", evt);
    }
}

void project_init() {
    static Thread<256> thread;
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}
