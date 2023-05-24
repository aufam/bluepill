#include "bldc/bldc.h"
#include "etl/array.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::BLDC;

val static crc16_tab = etl::array<uint16_t>(
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
);

fun static crc16(const uint8_t *data, uint8_t len) {
    uint16_t res = 0;
    for (val _ in etl::range(len))
        res = crc16_tab[(((res >> 8) ^ *data++) & 0xFF)] ^ (res << 8);
    return res;
}

fun Comm::init() -> void {
    if (uart) uart->init<Comm, void>(&Comm::uartRxCallback, this, nullptr, nullptr);
    if (can) can->init(true, &Comm::canRxCallback, this);
}

fun Comm::uartTransmit(const uint8_t* data, uint16_t len, uint8_t packet) -> void {
    if (!uart) return;
    var encoded = etl::array<uint8_t, 32>();
    val n = encode(encoded.data(), data, len, packet);
    uart->transmit(encoded.data(), n);
}

fun Comm::canTransmit(const uint8_t* data, uint16_t len, uint8_t packet) -> void {
    if (!can) return;
    can->transmit(true, id | packet << 8, data, len);
}

fun Comm::encode(uint8_t* buffer, const uint8_t* data, uint16_t len, uint8_t packet) -> size_t {
    size_t index = 0;
    buffer[index++] = 0x02;
    buffer[index++] = len + 1; // including packet id
    buffer[index++] = packet;
    memcpy(buffer + index, data, len);
    index += len;

    val crc = crc16(buffer + 2, len + 1);
    buffer[index++] = (uint8_t) (crc >> 8);
    buffer[index++] = (uint8_t) (crc & 0xFF);
    buffer[index++] = 0x03;
    buffer[index] = '\0';
    return index;
}

fun Comm::request() -> void { 
    val constexpr mask = MASK_ID | MASK_IN_VOLTAGE | 
                         MASK_MOSFET_TEMP | MASK_OUT_CURRENT | 
                         MASK_IN_CURRENT | MASK_ERPM | MASK_DUTY | 
                         MASK_TACHOMETER | MASK_FAULT_CODE;
    val data = etl::byte_array_cast_be(mask);
    uartTransmit(data.data(), data.len(), COMM_GET_VALUES_SELECTIVE);
    // in CAN mode, BLDC has to broadcast its values
}

fun Comm::setDuty(float value) -> void {
    val data = etl::byte_array_cast_be(int(value * 100'000.f));
    uartTransmit(data.data(), data.len(), COMM_SET_DUTY);
    canTransmit(data.data(), data.len(), CAN_PACKET_SET_DUTY);
}

fun Comm::setCurrent(float value) -> void {
    val data = etl::byte_array_cast_be(int(value * 1'000.f));
    uartTransmit(data.data(), data.len(), COMM_SET_CURRENT);
    canTransmit(data.data(), data.len(), CAN_PACKET_SET_CURRENT);
}

fun Comm::setCurrentRelative(float value) -> void {
    val data = etl::byte_array_cast_be(int(value * 100'000.f));
    uartTransmit(data.data(), data.len(), COMM_SET_CURRENT_REL);
    canTransmit(data.data(), data.len(), CAN_PACKET_SET_CURRENT_REL);
}

fun Comm::setCurrentBrake(float value) -> void {
    val data = etl::byte_array_cast_be(int(value * 1'000.f));
    uartTransmit(data.data(), data.len(), COMM_SET_CURRENT_BRAKE);
    canTransmit(data.data(), data.len(), CAN_PACKET_SET_CURRENT_BRAKE);
}

fun Comm::setRPM(float value) -> void {
    val data = etl::byte_array_cast_be(int(value));
    uartTransmit(data.data(), data.len(), COMM_SET_RPM);
    canTransmit(data.data(), data.len(), CAN_PACKET_SET_RPM);
}

/// cast buffer to int32_t big endian
fun static cast_back4(const uint8_t* buf) {
    return etl::byte_array_cast_back_be<int32_t>(*reinterpret_cast<const etl::Array<uint8_t, 4>*>(buf));
}

/// cast buffer to int16_t big endian
fun static cast_back2(const uint8_t* buf) {
    return etl::byte_array_cast_back_be<int16_t>(*reinterpret_cast<const etl::Array<uint8_t, 2>*>(buf));
}

fun Comm::uartRxCallback(Comm* self, const uint8_t* data, size_t len) -> void {
    if (len < 5 or data[0] != 0x02 or data[len - 1] != 0x03) 
        return; // error frame

    val crc = cast_back2(data + (len - 3));
    if (crc != crc16(data + 2, data[1])) 
        return; // error crc

    val packet = data[2];
    if (packet != COMM_GET_VALUES or packet != COMM_GET_VALUES_SELECTIVE) 
        return; // only support COMM_GET_VALUES

    var buf = data + 3;
    var mask = 0xFFFFFFFF;
    if (packet == COMM_GET_VALUES_SELECTIVE) {
        mask = cast_back4(buf);
        buf += 4;
    }

    if (mask & 1 << 0) {
        self->mosfetTemp = (float) cast_back2(buf) / 10.f;
        buf += 2;
    } if (mask & 1 << 1) {
        // motor temperature
        buf += 2;
    } if (mask & 1 << 2) {
        self->outCurrent = (float) cast_back4(buf) / 100.f;
        buf += 4;
    } if (mask & 1 << 3) {
        self->inCurrent = (float) cast_back4(buf) / 100.f;
        buf += 4;
    } if (mask & 1 << 4) {
        // avg id
        buf += 4;
    } if (mask & 1 << 5) {
        // avg iq
        buf += 4;
    } if (mask & 1 << 6) {
        self->duty = (float) cast_back2(buf) / 1000.f;
        buf += 2;
    } if (mask & 1 << 7) {
        self->erpm = (float) cast_back4(buf);
        buf += 4;
    } if (mask & 1 << 8) {
        self->inVoltage = (float) cast_back2(buf) / 10.f;
        buf += 2;
    } if (mask & 1 << 9) {
        // ampere hours
        buf += 4;
    } if (mask & 1 << 10) {
        // ampere hours charged
        buf += 4;
    } if (mask & 1 << 11) {
        // watt hours
        buf += 4;
    } if (mask & 1 << 12) {
        // watt hours charged
        buf += 4;
    } if (mask & 1 << 13) {
        self->tachometer = cast_back4(buf);
        buf += 4;
    } if (mask & 1 << 14) {
        // tacho absolute
        buf += 4;
    } if (mask & 1 << 15) {
        self->faultCode = MC_FAULT_CODE(*buf);
        buf++;
    } if (mask & 1 << 16) {
        // pid pos
        buf += 4;
    } if (mask & 1 << 17) {
        self->id = *buf;
        buf++;
    } if (mask & 1 << 18) {
        // mosfet 1, 2, 3 temperature
        buf += 6;
    } if (mask & 1 << 19) {
        // avg vd
        buf += 4;
    } if (mask & 1 << 20) {
        // avg vq
        buf += 4;
    } if (mask & 1 << 21) {
        // status timeout
        buf ++;
    }
}

fun Comm::canRxCallback(Comm* self, periph::CAN::Message& msg) -> void {
    if (msg.IDE == CAN_ID_STD) 
        return;
    
    val [id, packet] = etl::pair<uint8_t>(msg.ExtId & 0xff, msg.ExtId >> 8);
    if (id != self->id) 
        return;
    
    switch (packet) {
        case CAN_PACKET_STATUS: 
            self->erpm          = (float) cast_back4(msg.data);
            self->outCurrent    = (float) cast_back2(msg.data + 4) / 10.f;
            self->duty          = (float) cast_back2(msg.data + 6) / 1000.f;
            break;
        case CAN_PACKET_STATUS_4:
            self->mosfetTemp    = (float) cast_back2(msg.data) / 10.f; 
            self->inCurrent     = (float) cast_back2(msg.data + 4) / 10.f;
            break;
        case CAN_PACKET_STATUS_5:
            self->tachometer    = cast_back4(msg.data); 
            self->inVoltage     = (float) cast_back2(msg.data + 4) / 10.f;
            break;
        default:
            break;
    }
}
