#include "project.h"
#include "etl/thread.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::etl::literals;

var f = etl::string();

fun mainThread() {
    periph::adc1.init();

    periph::usb.rxCallbackList.push(lambda (auto buf, auto len) {
        *const_cast<uint8_t*>(buf + len) = '\0'; // manually add null terminator
        val &str = etl::string_cast<64>(buf);
        if (str is "adc")
            periph::usb << f("%ul\n%ul\n%ul\n", periph::adc1[0], periph::adc1[1], periph::adc1[2]);
        else
            periph::usb.transmit(buf, len);
    });

    for (;;) {
        periph::usb << "Test\n\r";
        etl::time::sleep(2s);
    }
}

fun project_init() -> void {
    var static thread = etl::Thread<256>();
    thread.init({.function=mainThread, .prio=osPriorityAboveNormal, .name="Main Thread"});
}
