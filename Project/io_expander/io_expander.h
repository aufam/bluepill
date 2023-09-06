#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "etl/array.h"

namespace Project {

    template <size_t NBytes>
    class IOExpander {
        static_assert(NBytes == 1 || NBytes == 2 || NBytes == 4);
        using T = etl::conditional_t<NBytes == 1, uint8_t, etl::conditional_t<NBytes == 2, uint16_t, uint32_t>>;

        inline static const uint32_t timeoutDefault = 100;
        mutable T writeMode = 0, readMode = 0;
        mutable T writeBuffer = 0, readBuffer = 0;

    public:
        periph::I2C& i2c;
        uint8_t address; ///< device 7 bit address
        
        constexpr IOExpander(periph::I2C& i2c, uint8_t address) : i2c(i2c), address(address) {}
        constexpr IOExpander(periph::I2C& i2c, bool a2, bool a1, bool a0) : i2c(i2c), address(addressReference(a2, a1, a0)) {}

        IOExpander(const IOExpander&) = delete; ///< disable copy constructor
        IOExpander& operator=(const IOExpander&) = delete; ///< disable copy assignment

        /// write value to the io expander
        /// @param value desired value
        /// @param timeout in tick. default = timeoutDefault
        void write(T value, uint32_t timeout = timeoutDefault) const { 
            auto buffer = etl::byte_array_cast_le<T>(value);
            HAL_I2C_Master_Transmit(&i2c.hi2c, address, buffer.data(), buffer.len(), timeout); 
            writeBuffer = value; // save to write buffer
        }

        /// read value from the io expander
        /// @param timeout in tick. default = timeoutDefault
        /// @return value from the io expander
        T read(uint32_t timeout = timeoutDefault) const { 
            auto buffer = etl::array<uint8_t, NBytes>();
            HAL_I2C_Master_Receive(&i2c.hi2c, address, buffer.data(), buffer.len(), timeout); 
            T res = etl::byte_array_cast_back_le<T>(buffer);
            if (res & readMode) readBuffer |= res; // only save to buffer if the pin is in read mode
            return res;
        }

        struct Pin;
        constexpr Pin pin(T pin, bool activeMode) { return Pin(*this, pin, activeMode); }

    private:
        constexpr uint8_t addressReference(bool a2, bool a1, bool a0) { return (a2 << 2 | a1 << 1 | a0) + 0x40; }
    };

    template <size_t NBytes>
    struct IOExpander<NBytes>::Pin {
        IOExpander<NBytes>& port;   ///< reference to IOExpander object
        IOExpander<NBytes>::T pin;  ///< GPIO_PIN_x
        bool activeMode;            ///< periph::GPIO::activeLow or periph::GPIO::activeHigh

        constexpr Pin(IOExpander<NBytes>& port, uint16_t pin, bool activeMode)
        : port(port)
        , pin(pin)
        , activeMode(activeMode) {}

        /// init IO and turn off
        /// @param mode GPIO_MODE_INPUT or GPIO_MODE_OUTPUT_PP
        void init(uint32_t mode) const {
            if (mode == GPIO_MODE_INPUT) {
                port.readMode |= pin;
                port.writeMode &= ~pin;
            } else {
                port.writeMode |= pin;
                port.readMode &= ~pin;
                off();
            }
        }

        /// write pin high (true) or low (false)
        void write(bool value) const {
            port.writeBuffer = value ? port.writeBuffer | pin : port.writeBuffer & (~pin);
            port.write(port.writeBuffer & port.writeMode); // only write pin that is in write mode
        }

        /// toggle pin
        void toggle() const { port.writeBuffer & pin ? write(false) : write(true); }

        /// read pin
        /// @retval high (true) or low (false)
        [[nodiscard]] bool read() const {
            if (pin & port.writeMode) // if pin is in write mode, return pin status in write buffer
                return pin & port.writeBuffer;
            port.read();
            return pin & port.readBuffer;
        }

        /// turn on
        /// @param ticks sleep for a while. default = 0
        void on(uint32_t ticks = 0) const {
            write(activeMode);
            osDelay(ticks);
        }

        /// turn off
        /// @param ticks sleep for a while. default = 0
        void off(uint32_t ticks = 0) const {
            write(!activeMode);
            osDelay(ticks);
        }

        [[nodiscard]] bool isOn() const { return !(read() ^ activeMode); }
        [[nodiscard]] bool isOff() const { return (read() ^ activeMode); }
    };

    struct IO {
        IOExpander<2>* portExpander;    ///< pointer to IOExpander object
        GPIO_TypeDef* portGPIO;         ///< pointer to GPIOx
        uint16_t pin;      ///< GPIO_PIN_x
        bool activeMode;                ///< periph::GPIO::activeLow or periph::GPIO::activeHigh

        constexpr IO(IOExpander<2>& port, uint16_t pin, bool activeMode)
            : portExpander(&port)
            , portGPIO(nullptr)
            , pin(pin)
            , activeMode(activeMode) {}

        constexpr IO(GPIO_TypeDef *port, uint16_t pin, bool activeMode)
            : portExpander(nullptr)
            , portGPIO(port)
            , pin(pin)
            , activeMode(activeMode) {}

        /// init IO and turn off
        /// @param mode GPIO_MODE_INPUT or GPIO_MODE_OUTPUT_PP
        void init(uint32_t mode) const {
            if (portExpander)
                IOExpander<2>::Pin(*portExpander, pin, activeMode).init(mode);
            if (portGPIO)
                periph::GPIO(portGPIO, pin, activeMode).init(mode);
        }

        /// write pin high (true) or low (false)
        void write(bool value) const {
            if (portExpander)
                IOExpander<2>::Pin(*portExpander, pin, activeMode).write(value);
            if (portGPIO)
                periph::GPIO(portGPIO, pin, activeMode).write(value);
        }

        /// toggle pin
        void toggle() const { 
            if (portExpander)
                IOExpander<2>::Pin(*portExpander, pin, activeMode).toggle();
            if (portGPIO)
                periph::GPIO(portGPIO, pin, activeMode).toggle(); 
        }

        /// read pin
        /// @retval high (true) or low (false)
        [[nodiscard]] bool read() const {
            if (portExpander)
                return IOExpander<2>::Pin(*portExpander, pin, activeMode).read();
            if (portGPIO)
                return periph::GPIO(portGPIO, pin, activeMode).read(); 
        }

        /// turn on
        /// @param ticks sleep for a while. default = 0
        void on(uint32_t ticks = 0) const {
            if (portExpander)
                return IOExpander<2>::Pin(*portExpander, pin, activeMode).on(ticks);
            if (portGPIO)
                return periph::GPIO(portGPIO, pin, activeMode).on(ticks); 
        }

        /// turn off
        /// @param ticks sleep for a while. default = 0
        void off(uint32_t ticks = 0) const {
            if (portExpander)
                return IOExpander<2>::Pin(*portExpander, pin, activeMode).off(ticks);
            if (portGPIO)
                return periph::GPIO(portGPIO, pin, activeMode).off(ticks); 
        }

        [[nodiscard]] bool isOn() const { return !(read() ^ activeMode); }
        [[nodiscard]] bool isOff() const { return (read() ^ activeMode); }
    };
}



#endif // IO_EXPANDER_H