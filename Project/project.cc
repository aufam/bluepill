#include "project.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::OS;
extern char blink_symb[16];

void mainThread(void *arg);
void project_init() {
    static ThreadStatic<128> thread;
    thread.init("Main Thread", osPriorityAboveNormal, mainThread);
    strcpy(blink_symb, "");
}

auto &adc = adc1;
auto &uart = uart1;
auto &encoder = encoder1;
auto &i2c = i2c2;
OBD2 obd { can };
Oled oled { i2c2 };
String<128> f;

enum { EVENT_CLEAR, EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
QueueStatic<int, 1> event;
uint8_t pid = 0;
uint32_t page = 0;

void mainThread(void *arg) {
    (void) arg;

    obd.init();
    oled.init();
    rtc.init();
    adc.init();
    event.init();

    exti.setCallback(button_up_Pin, [](void *) { event << EVENT_BT_UP; });
    exti.setCallback(button_down_Pin, [](void *) { event << EVENT_BT_DOWN; });
    exti.setCallback(button_right_Pin, [](void *) { event << EVENT_BT_RIGHT; });
    exti.setCallback(button_left_Pin, [](void *) { event << EVENT_BT_LEFT; });
    exti.setCallback(button_rot_Pin, [](void *) { event << EVENT_BT_ROT; });
    encoder.init([](void *) { event << EVENT_ROT_UP; }, nullptr,
                 [](void *) { event << EVENT_ROT_DOWN; });

    uart.init([](void *, size_t len) {
        auto *buf = uart.rxBuffer.data();
        if (len >= 2) {
            page = buf[0];
            if (page % 2 == 0) pid = buf[1];
        }
    });

    usb.setRxCallback([](void *, size_t len) { usb.transmit(usb.rxBuffer.data(), len); });

    for(;;) {
        int evt = EVENT_CLEAR;
        event.pop(evt, 300);
        switch (evt) {
            case EVENT_ROT_UP:
            case EVENT_BT_UP: if (pid < OBD2::nPid - 1) pid++; break;
            case EVENT_ROT_DOWN:
            case EVENT_BT_DOWN: if (pid > 0) pid--; break;
            case EVENT_BT_ROT:
            case EVENT_BT_RIGHT: page++; break;
            case EVENT_BT_LEFT: page--; break;
            default: break;
        }

        oled.setCursor(0, 0);
        if (page % 2 == 0) {
            oled << f("%02X %s\n", pid, OBD2::pidNames[pid]);
            uart.writeBlocking((uint8_t *) f.data(), f.len());

            auto msg = obd.request(pid);
            if (msg.errorStr)
                oled << f("%s\n", msg.errorStr);
            else if (msg.str)
                oled << f("%s\n", msg.str);
            else if (msg.u != 0x80000000)
                oled << f("%ld.%02ld %s\n", msg.val / 100, msg.val % 100, OBD2::pidUnits[pid]);
            else
                oled << f("RAW: x%8lX\n", msg.raw);

            uart.writeBlocking((uint8_t *) f.data(), f.len());
        }
        else {
            f("VIN: ");
            obd.requestVin(OBD2::VEHICLE_ID_NUMBER, f.end());
            f += '\n';
            oled << f.data();
            uart.writeBlocking((uint8_t *) f.data(), f.len());

            f("ECU: ");
            obd.requestVin(OBD2::ECU_NAME, f.end());
            f += '\n';
            oled << f.data();
            uart.writeBlocking((uint8_t *) f.data(), f.len());
        }
        oled.clear(oled.column, oled.row); // clear remaining space
    }
}
