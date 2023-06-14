#include "project.h"
#include "etl/all.h"
#include "periph/all.h"
#include "oled/oled.h"
#include "lcd/lcd.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::etl::literals;

var &encoder = periph::encoder1;

fun mainThread() {
    var oled = Oled(periph::i2c2);
    oled.init();

    enum { EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
    var thd = etl::threadGetCurrent();

    periph::exti.setCallback(button_up_Pin, lambda (var thd) { 
        thd->setFlags(1 << EVENT_BT_UP); }, 
    &thd);

    periph::exti.setCallback(button_down_Pin, lambda (var thd) { 
        thd->setFlags(1 << EVENT_BT_DOWN); }, 
    &thd);

    periph::exti.setCallback(button_right_Pin, lambda (var thd) { 
        thd->setFlags(1 << EVENT_BT_RIGHT); }, 
    &thd);

    periph::exti.setCallback(button_left_Pin, lambda (var thd) { 
        thd->setFlags(1 << EVENT_BT_LEFT); }, 
    &thd);

    periph::exti.setCallback(button_rot_Pin, lambda (var thd) { 
        thd->setFlags(1 << EVENT_BT_ROT); }, 
    &thd);

    encoder.setIncrementCB(lambda (var thd) { 
        thd->setFlags(1 << EVENT_ROT_UP); }, 
    &thd);

    encoder.setDecrementCB(lambda (var thd) { 
        thd->setFlags(1 << EVENT_ROT_DOWN); }, 
    &thd);

    var f = etl::string();
    oled << "Hello World!\n";
    oled << f("heap free sz = %lu bytes\n", etl::heapGetFreeSize());

    for (;;) {
        // wait evt and print it to oled
        val catchedFlags = etl::threadWaitFlagsAny();
        val eventNumber = etl::count_trailing_zeros(catchedFlags);
        oled << f("%lu", eventNumber);
    }
}

fun project_init() -> void {
    encoder.init();

    // assign to static variable to ensure the lifetime extends the scope
    var static s = etl::make_thread(mainThread, 256, osPriorityAboveNormal1);
}
