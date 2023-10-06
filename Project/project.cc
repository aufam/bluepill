#include "project.h"
#include "etl/all.h"
#include "periph/all.h"
#include "oled/oled.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::etl::literals;

var &encoder = periph::encoder1;

fun mainThread() {
    var oled = Oled({.i2c=periph::i2c2});
    oled.init();

    enum { EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
    var thd = etl::this_thread::get();

    var buttonUp = periph::Exti {
        .pin=button_up_Pin,
        .callback={
            lambda (var thd) {
                thd->setFlags(1 << EVENT_BT_UP);
            }, &thd
        }
    };

    buttonUp.init();

    // periph::exti.setCallback(button_up_Pin, lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_BT_UP); }, 
    // &thd);

    // periph::exti.setCallback(button_down_Pin, lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_BT_DOWN); }, 
    // &thd);

    // periph::exti.setCallback(button_right_Pin, lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_BT_RIGHT); }, 
    // &thd);

    // periph::exti.setCallback(button_left_Pin, lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_BT_LEFT); }, 
    // &thd);

    // periph::exti.setCallback(button_rot_Pin, lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_BT_ROT); }, 
    // &thd);

    // encoder.setIncrementCB(lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_ROT_UP); }, 
    // &thd);

    // encoder.setDecrementCB(lambda (var thd) { 
    //     thd->setFlags(1 << EVENT_ROT_DOWN); }, 
    // &thd);

    var f = etl::string();
    oled << "Hello World!\n";
    oled << f("heap free sz = %lu bytes\n", (size_t)etl::heap::freeSize);

    for (;;) {
        // wait flag and print it to oled
        val catchedFlags = etl::this_thread::waitFlagsAny();
        val eventNumber = etl::count_trailing_zeros(catchedFlags.get());
        oled << f("%lu", eventNumber);
    }
}

fun project_init() -> void {
    encoder.init();

    // assign to static variable to ensure the lifetime extends the scope
    var static s = etl::thread({.function=mainThread, .stackSize=256, .prio=osPriorityAboveNormal1});
}
