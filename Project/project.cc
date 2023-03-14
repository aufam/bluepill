#include "project.h"
#include "etl/all.h"
#include "oled/oled.h"
#include "etl/python_keywords.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;
using namespace Project::etl::literals;

auto f = String {};

void mainThread(void *) {
    adc1.init();

    usb.setRxCallback([](auto, auto buf, auto len) {
        auto& str = string_cast<16>((const char *)buf);
        if (str == "adc") {
            usb << f("%ul\n%ul\n%ul\n", adc1[0], adc1[1], adc1[3]);
        } else
            usb.transmit(buf, len);
    });

    for (;;) {
        usb << "Test\n\r";
        sleep(2s);
    }
}

void project_init() {
    static Thread<256> thread;
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}
