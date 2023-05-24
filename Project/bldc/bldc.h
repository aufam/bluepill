#ifndef PROJECT_BLDC_H
#define PROJECT_BLDC_H

#include "periph/uart.h"
#include "periph/can.h"
#include "bldc/types.h"

namespace Project::BLDC {

    struct Comm : public Values {
        periph::CAN* can;
        periph::UART* uart;

        constexpr Comm(periph::CAN* can, periph::UART* uart = nullptr) : Values{}, can(can), uart(uart) {} 
        constexpr Comm(periph::UART* uart, periph::CAN* can = nullptr) : Values{}, can(can), uart(uart) {}

        /// init can and/or uart
        void init();
        
        /// encode data and write to buffer
        /// @param[out] buffer encoded data buffer
        /// @param[in] data input data
        /// @param len data length
        /// @param packet COMM_PACKET_ID
        /// @return number of bytes written to buffer
        static size_t encode(uint8_t* buffer, const uint8_t* data, uint16_t len, uint8_t packet);

        /// encode data and transmit via uart non blocking
        /// @param[in] data input data
        /// @param len data length
        /// @param packet COMM_PACKET_ID
        void uartTransmit(const uint8_t* data, uint16_t len, uint8_t packet);

        /// transmit data via CAN bus
        /// @param[in] data input data
        /// @param len data length, max 8 bytes
        /// @param packet CAN_PACKET_ID
        void canTransmit(const uint8_t* data, uint16_t len, uint8_t packet);

        /// request BLDC values
        void request();

        /// set duty cycle
        /// @param value in percent
        void setDuty(float value);

        /// set output current
        /// @param value in ampere
        void setCurrent(float value);

        /// set output current
        /// @param value the relative current value in percent of max current, range [-100.0, 100.0]
        void setCurrentRelative(float value);

        /// set brake current
        /// @param value in ampere
        void setCurrentBrake(float value);

        /// set motor speed
        /// @param value in rpm
        void setRPM(float value);

    private:
        static void uartRxCallback(Comm* self, const uint8_t* data, size_t len);
        static void canRxCallback(Comm* self, periph::CAN::Message& msg);
    };
}

#endif // PROJECT_BLDC_H