#include "saeco_series_6000.h"
#include <vector>

namespace esphome {
namespace saeco_series_6000 {

uint32_t SaecoSeries6000::calc_crc(const std::vector<uint8_t>& data, size_t start, size_t end) {
    uint32_t crc = 0xFFFFFFFF;
    const uint32_t POLY = 0x04C11DB7;
    if (start >= end) return 0;
    // start
    uint8_t val = data[start];
    ((uint8_t*)(&crc))[3] ^= BitReverse[val];
    for (uint8_t j = 0; j < 8; j++) {
        if (crc & 0x80000000) {
            crc = (crc << 1) ^ POLY;
        } else {
            crc <<= 1;
        }
    }
    // остальные байты
    for (size_t i = start + 1; i < end; i++) {
        uint8_t v = data[i];
        ((uint8_t*)(&crc))[3] ^= BitReverse[v];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    uint32_t t32 = 0;
    ((uint8_t*)(&t32))[0] = BitReverse[((uint8_t*)(&crc))[3]];
    ((uint8_t*)(&t32))[1] = BitReverse[((uint8_t*)(&crc))[2]];
    ((uint8_t*)(&t32))[2] = BitReverse[((uint8_t*)(&crc))[1]];
    ((uint8_t*)(&t32))[3] = BitReverse[((uint8_t*)(&crc))[0]];
    crc = t32 ^ 0xFFFFFFFF;
    return crc;
}

} // namespace saeco_series_6000
} // namespace esphome 