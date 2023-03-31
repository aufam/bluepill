#include "project.h"
#include "etl/all.h"
#include "oled/oled.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;
using namespace Project::etl::literals;

enum { EVENT_CLEAR, EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
var event = Queue<int, 1>();
var oled = Oled(i2c2);
var &encoder = encoder1;
var f = string();

fun mainThread(void *) {
    event.init();
    oled.init();

    exti.setCallback(button_up_Pin,     lambda (val) { event << EVENT_BT_UP; });
    exti.setCallback(button_down_Pin,   lambda (val) { event << EVENT_BT_DOWN; });
    exti.setCallback(button_right_Pin,  lambda (val) { event << EVENT_BT_RIGHT; });
    exti.setCallback(button_left_Pin,   lambda (val) { event << EVENT_BT_LEFT; });
    exti.setCallback(button_rot_Pin,    lambda (val) { event << EVENT_BT_ROT; });

    encoder.init();
    encoder.setIncrementCB(lambda (val) { event << EVENT_ROT_UP; });
    encoder.setDecrementCB(lambda (val) { event << EVENT_ROT_DOWN; });

    oled << "Hello World!\n";
    for (;;) {
        // wait event and print it to oled
        oled << f("%d", event.pop(osWaitForever));
    }
}

fun project_init() -> void {
    var static thread = Thread<256>();
    thread.init(mainThread, null, osPriorityAboveNormal, "Main Thread");
}
