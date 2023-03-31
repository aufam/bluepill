#include "project.h"
#include "etl/all.h"
#include "oled/oled.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;
using namespace Project::etl::literals;

var f = string();

fun mainThread(void *) {
    adc1.init();

    usb.setRxCallback(lambda (val, val buf, val len) {
        val &str = string_cast<64>(buf);
        if (str is "adc")
            usb << f("%ul\n%ul\n%ul\n", adc1[0], adc1[1], adc1[2]);
        else
            usb.transmit(buf, len);
    });

    for (;;) {
        usb << "Test\n\r";
        sleep(2s);
    }
}

fun project_init() -> void {
    var static thread = Thread<256>();
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}
