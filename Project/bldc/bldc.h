#ifndef PROJECT_BLDC_H
#define PROJECT_BLDC_H

#include "periph/uart.h"
#include "periph/can.h"
#include "bldc/types.h"

namespace Project::BLDC {

    /// BLDC communication class
    /// @note in CAN mode, BLDC has to broadcast its values by actavating CAN_PACKET_STATUS level 5
    /// @note in UART mode, you have to invoke request() method periodically
    /// @note activating both modes is not necessary
    struct Comm : public Values {
        using PacketProcess = etl::Function<void(uint8_t packet, const uint8_t* data, size_t len), void*>;

        periph::CAN* can;
        periph::UART* uart;
        etl::Array<uint8_t, 64> txBuffer = {};
        PacketProcess packetProcess = {};

        /// construct CAN mode
        constexpr Comm(periph::CAN& can) : Values{}, can(&can), uart(nullptr) {} 

        /// construct UART mode
        constexpr Comm(periph::UART& uart) : Values{}, can(nullptr), uart(&uart) {}

        /// init CAN and/or UART mode
        void init();

        /// detach rx callback from CAN rx callback list
        void deinit();
        
        /// encode data and write to buffer
        /// @param[out] buffer encoded data buffer
        /// @param[in] data input data
        /// @param len data length
        /// @param packet COMM_PACKET_ID
        /// @return number of bytes written to buffer
        static size_t encode(uint8_t* buffer, const uint8_t* data, size_t len, uint8_t packet);

        /// decode data
        /// @param[in] data input data
        /// @param[in, out] len data (in) and decoded data (out) length 
        /// @param[out] packet COMM_PACKET_ID
        /// @return pointer to the decoded data or null
        static const uint8_t* decode(const uint8_t* data, size_t& len, uint8_t& packet);

        /// encode data and transmit via uart non blocking
        /// @param[in] data input data
        /// @param len data length
        /// @param packet COMM_PACKET_ID
        void uartTransmit(const uint8_t* data, size_t len, uint8_t packet);

        /// encode data and transmit via uart non blocking
        /// @param[in] data input data
        /// @param len data length
        /// @param packet COMM_PACKET_ID
        void uartTransmit(const char* text);

        /// transmit data via CAN bus
        /// @param[in] data input data
        /// @param len data length, max 8 bytes
        /// @param packet CAN_PACKET_ID
        void canTransmit(const uint8_t* data, size_t len, uint8_t packet);

        /// request BLDC values
        void request();

        /// set duty cycle
        /// @param value in range [-1.0, 1.0]
        void setDuty(float value);

        /// set output current
        /// @param value in ampere
        void setCurrent(float value);

        /// set output current
        /// @param value the relative current value of max current, range [-1.0, 1.0]
        void setCurrentRelative(float value);

        /// set brake current
        /// @param value in ampere
        void setCurrentBrake(float value);

        /// set motor speed
        /// @param value in rpm 
        void setRPM(float value);

        template <typename Fn>
        void setPacketProcess(Fn&& fn) { packetProcess = etl::forward<Fn>(fn); }

        template <typename Fn, typename Ctx>
        void setPacketProcess(Fn&& fn, Ctx* ctx) { packetProcess = PacketProcess(etl::forward<Fn>(fn), ctx); }

    private:
        static void uartRxCallback(Comm* self, const uint8_t* data, size_t len);
        static void canRxCallback(Comm* self, periph::CAN::Message& msg);
    };
}

#endif // PROJECT_BLDC_H