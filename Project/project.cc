#include "project.h"
#include "etl/all.h"
#include "periph/all.h"
#include "oled/oled.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::etl::literals;

fun mainThread() {
    var &encoder = periph::encoder1;

    var oled = Oled({.i2c=periph::i2c2});
    oled.init();

    enum { EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
    var thd = etl::this_thread::get();

    var buttonUp = periph::Exti {
        .pin=button_up_Pin,
        .callback={ lambda (var thd) { thd->setFlags(1 << EVENT_BT_UP); }, &thd}
    };
    buttonUp.init();

    var buttonDown = periph::Exti {
        .pin=button_down_Pin,
        .callback={ lambda (var thd) { thd->setFlags(1 << EVENT_BT_DOWN); }, &thd},
    };
    buttonDown.init();

    var buttonRight = periph::Exti {
        .pin=button_right_Pin,
        .callback={ lambda (var thd) { thd->setFlags(1 << EVENT_BT_RIGHT); }, &thd},
    };
    buttonRight.init();

    var buttonLeft = periph::Exti {
        .pin=button_left_Pin,
        .callback={ lambda (var thd) { thd->setFlags(1 << EVENT_BT_LEFT); }, &thd},
    };
    buttonLeft.init();

    var buttonRot = periph::Exti {
        .pin=button_rot_Pin,
        .callback={ lambda (var thd) { thd->setFlags(1 << EVENT_BT_ROT); }, &thd},
    };
    buttonRot.init();

    encoder.incrementCallback = {lambda (var thd) { thd->setFlags(1 << EVENT_ROT_UP); }, &thd};
    encoder.decrementCallback = {lambda (var thd) { thd->setFlags(1 << EVENT_ROT_DOWN); }, &thd};
    encoder.init();

    var f = etl::string();
    oled << "Hello World!\n";
    oled << f("heap free sz = %lu bytes\n", (size_t)etl::heap::freeSize);

    for (;;) {
        // wait flag and print it to oled
        val flag = etl::this_thread::waitFlagsAny();
        val eventNumber = etl::count_trailing_zeros(flag.get());
        oled << f("%lu", eventNumber);
    }
}

fun project_init() -> void {
    // assign to static variable to ensure the lifetime extends the scope
    var static s = etl::thread({.function=mainThread, .stackSize=256, .prio=osPriorityAboveNormal1});
}
