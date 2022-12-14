#include "project.h"

using namespace Project;
using namespace Project::Periph;
using namespace Project::etl;

/// override operator new with malloc from @p heap_4.c
void *operator new(size_t size) { return pvPortMalloc(size); }
/// override operator delete with free from @p heap_4.c
void operator delete(void *ptr) { vPortFree(ptr); }

void mainThread(void *);
void project_init() {
    static Thread<256> thread;
    thread.init(mainThread, nullptr, osPriorityAboveNormal, "Main Thread");
}

auto &adc = adc1;
auto &uart = uart1;
auto &encoder = encoder1;
auto obd = OBD2 { can };
auto oled = Oled { i2c2 };
auto f = String<128> {};

enum { EVENT_CLEAR, EVENT_BT_UP, EVENT_BT_DOWN, EVENT_BT_RIGHT, EVENT_BT_LEFT, EVENT_BT_ROT, EVENT_ROT_UP, EVENT_ROT_DOWN };
Queue<int, 1> event;
uint8_t pid = 0;
uint32_t page = 0;

void mainThread(void *) {
    obd.init();
    oled.init();
    rtc.init();
    adc.init();
    event.init();

    exti.setCallback(button_up_Pin, [](auto) { event << EVENT_BT_UP; });
    exti.setCallback(button_down_Pin, [](auto) { event << EVENT_BT_DOWN; });
    exti.setCallback(button_right_Pin, [](auto) { event << EVENT_BT_RIGHT; });
    exti.setCallback(button_left_Pin, [](auto) { event << EVENT_BT_LEFT; });
    exti.setCallback(button_rot_Pin, [](auto) { event << EVENT_BT_ROT; });
    encoder.init([](auto) { event << EVENT_ROT_UP; }, nullptr,
                 [](auto) { event << EVENT_ROT_DOWN; });

    uart.init([](void *, size_t len) {
        const auto &buf = uart.rxBuffer;
        if (len < 2) return;
        page = buf[0];
        if (page % 4 == 0) pid = buf[1];
    });

    usb.setRxCallback([](auto, auto len) { usb.transmit(usb.rxBuffer.data(), len); });

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
        auto num = page % 4;
        if (num == 0) {
            oled << f("%02X %s\n", pid, OBD2::pidNames[pid]);
            uart << f.data();

            auto msg = obd.request(pid);
            if (msg.errorStr) // error message
                oled << f("%s\n", msg.errorStr);
            else if (msg.str) // string message
                oled << f("%s\n", msg.str);
            else if (isnanf(msg.val)) // raw data
                oled << f("RAW: x%8lX\n", msg.raw);
            else // actual data
                oled << f("%.2f %s\n", msg.val, OBD2::pidUnits[pid]);

            uart << f.data();
        } else if (num == 1) {
            f("VIN: ");
            obd.requestVin(OBD2::VEHICLE_ID_NUMBER, f.end());
            f += '\n';
            oled << f.data();
            uart << f.data();

            f("ECU: ");
            obd.requestVin(OBD2::ECU_NAME, f.end());
            f += '\n';
            oled << f.data();
            uart << f.data();
        } else if (num == 2) {
            auto msg = obd.request(OBD2::FUEL_TANK_LEVEL_INPUT);
            oled << f("Fuel tank = %.2f %s\n", msg.val, OBD2::pidUnits[OBD2::FUEL_TANK_LEVEL_INPUT]);
            msg = obd.request(OBD2::ENGINE_FUEL_RATE);
            oled << f("Fuel rate = %.2f %s\n", msg.val, OBD2::pidUnits[OBD2::ENGINE_FUEL_RATE]);
            msg = obd.request(OBD2::MAX_FUEL_AIR_EQUIV_RATIO);
            oled << f("Max fuel air = %.2f %s\n", msg.val);
            msg = obd.request(OBD2::FUEL_TYPE);
            oled << f("Fuel type = %s\n", msg.str, OBD2::pidUnits[OBD2::MAX_FUEL_AIR_EQUIV_RATIO]);
            msg = obd.request(OBD2::FUEL_INJECTION_TIMING);
            oled << f("Fuel type = %.2f %s\n", msg.val, OBD2::pidUnits[OBD2::FUEL_INJECTION_TIMING]);
        } else if (num == 3) {
            oled << "Sniffing Mode\n";
            CAN::Message canMsg = {};
            auto res = obd.rxQueue.pop(canMsg, OBD2::waitResponseMs);
            if (res == osOK) {
                auto id = can.isUsingExtId() ? canMsg.rxHeader.ExtId : canMsg.rxHeader.StdId;
                auto len = (uint8_t) canMsg.rxHeader.IDE;
                auto data = canMsg.data;

                f("%08X %02X ", id, len);
                for (uint8_t i = 0; i < len; i++) f += String(" %02X", data[i]);
                f += '\n';
                oled << f.data();
                uart << f.data();
            } else if (res != osErrorTimeout) {
                oled << f("%s\n", OBD2::errorCode[-res - 1]);
            }
        }
        oled.clearRemainingRows();
    }
}
