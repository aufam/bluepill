#ifndef PROJECT_BLDC_H
#define PROJECT_BLDC_H

#include "bldc/types.h"
#include "periph/uart.h"
#include "periph/can.h"
#include "etl/getter_setter.h"
#include "etl/function.h"

namespace Project::bldc {

    struct BLDCValues {
        int id;                     ///< controller id
        float voltageIn;            ///< in volt
        float currentIn;            ///< in ampere
        float mosfetTemperature;    ///< in C
        float dutyCycle;            ///< in range [-1.0, 1.0]
        float current;              ///< in ampere
        float speed;                ///< in rpm
        int position;               ///< relative position in step
        MC_FAULT_CODE faultCode;
    };

    /// BLDC communication class
    /// @note in CAN mode, the BLDC controller has to broadcast its values by
    /// activating CAN_PACKET_STATUS, CAN_PACKET_STATUS_4, and CAN_PACKET_STATUS_5
    /// @note in UART mode, you have to invoke request() method periodically
    /// @note activating both modes is not necessary
    class BLDC {
        template <typename T>
        using GetterSetter = etl::GetterSetter<T, etl::Function<T(), BLDC*>, etl::Function<void(T), BLDC*>>;

        template <typename T>
        using Setter = etl::Setter<T, etl::Function<void(T), BLDC*>>;

        BLDCValues& values;

    public:
        using PacketProcess = etl::Function<void(uint8_t packet, const uint8_t* data, size_t len), void*>;

        periph::CAN* can;
        periph::UART* uart;
        etl::Array<uint8_t, 64> txBuffer = {};
        PacketProcess packetProcess = {};

        /// construct CAN mode
        constexpr BLDC(BLDCValues& values, periph::CAN& can) : values(values), can(&can), uart(nullptr) {}

        /// construct UART mode
        constexpr BLDC(BLDCValues& values, periph::UART& uart) : values(values), can(nullptr), uart(&uart) {}

        /// init CAN and/or UART mode
        void init();

        /// detach rx callback from CAN rx callback list
        void deinit();

        /// setter and getter controller id
        int& id = values.id;

        /// getter input voltage, value is in Voltage
        const float& voltageIn = values.voltageIn;

        /// getter input current, value is in Ampere
        const float& currentIn = values.currentIn;

        /// getter MOSFET temperature, value is in degree Celcius
        const float& mosfetTemperature = values.mosfetTemperature;

        /// getter and setter duty cycle, value is in range [-1.0, 1.0]
        GetterSetter<float> dutyCycle = {
            {+[] (BLDC* self) { return self->values.dutyCycle; }, this},
            etl::bind<&BLDC::setDutyCycle>(this)
        };

        /// getter and setter current, value is in Ampere
        GetterSetter<float> current = {
            {+[] (BLDC* self) { return self->values.current; }, this},
            etl::bind<&BLDC::setCurrent>(this)
        };

        /// setter current relative, value is in range [-1.0, 1.0]
        Setter<float> currentRelative = {
            etl::bind<&BLDC::setCurrentRelative>(this)
        };

        /// setter current brake, value is in Ampere
        Setter<float> currentBrake = {
            etl::bind<&BLDC::setCurrentBrake>(this)
        };

        /// getter and setter speed, value is in rpm
        GetterSetter<float> speed = {
            {+[] (BLDC* self) { return self->values.speed; }, this},
            etl::bind<&BLDC::setSpeed>(this)
        };

        /// getter position, value is in unit step
        const int& position = values.position;

        /// getter fault code, value is FAULT_CODE_XXX
        const MC_FAULT_CODE& faultCode = values.faultCode;
        
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

        template <typename Fn>
        void setPacketProcess(Fn&& fn) { packetProcess = etl::forward<Fn>(fn); }

        template <typename Fn, typename Ctx>
        void setPacketProcess(Fn&& fn, Ctx* ctx) { packetProcess = PacketProcess(etl::forward<Fn>(fn), ctx); }

    private:
        void setDutyCycle(float value);
        void setCurrent(float value);
        void setCurrentRelative(float value);
        void setCurrentBrake(float value);
        void setSpeed(float value);

        static void uartRxCallback(BLDC* self, const uint8_t* data, size_t len);
        static void canRxCallback(BLDC* self, periph::CAN::Message& msg);
    };
}

#endif // PROJECT_BLDC_H