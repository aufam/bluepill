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
    var evt = etl::event();

    periph::exti.setCallback(button_up_Pin, +lambda (decltype(evt)* evt) { 
        evt->setFlags(1 << EVENT_BT_UP); }, &evt);
    periph::exti.setCallback(button_down_Pin, +lambda (decltype(evt)* evt) { 
        evt->setFlags(1 << EVENT_BT_DOWN); }, &evt);
    periph::exti.setCallback(button_right_Pin, +lambda (decltype(evt)* evt) { 
        evt->setFlags(1 << EVENT_BT_RIGHT); }, &evt);
    periph::exti.setCallback(button_left_Pin, +lambda (decltype(evt)* evt) { 
        evt->setFlags(1 << EVENT_BT_LEFT); }, &evt);
    periph::exti.setCallback(button_rot_Pin, +lambda (decltype(evt)* evt) { 
        evt->setFlags(1 << EVENT_BT_ROT); }, &evt);

    encoder.setIncrementCB(+lambda (decltype(evt)* evt) { evt->setFlags(1 << EVENT_ROT_UP); }, &evt);
    encoder.setDecrementCB(+lambda (decltype(evt)* evt) { evt->setFlags(1 << EVENT_ROT_DOWN); }, &evt);

    var f = etl::string();
    oled << "Hello World!\n";
    oled << f("heap free sz = %lu bytes\n", etl::heapGetFreeSize());

    for (;;) {
        // wait evt and print it to oled
        val allEventFlags = (1 << (EVENT_ROT_DOWN + 1)) - 1;
        val catchedFlags = evt.waitFlags(allEventFlags);
        val eventNumber = etl::count_trailing_zeros(catchedFlags);
        oled << f("%lu", eventNumber);
    }
}

fun project_init() -> void {
    encoder.init();

    // assign to static variable to ensure the lifetime extends the scope
    var static s = etl::thread(mainThread, 256, osPriorityAboveNormal1);
}
