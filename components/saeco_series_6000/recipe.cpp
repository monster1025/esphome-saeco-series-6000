#include "saeco_series_6000.h"
#include <vector>

namespace esphome {
namespace saeco_series_6000 {

std::vector<uint8_t> SaecoSeries6000::build_coffee_recipe(uint8_t type, uint8_t bean, uint8_t cups, uint16_t vol, uint16_t milk) {
    static const uint8_t CoffeePattern[7][4] = {
        {0,1,1,2}, // Espresso
        {1,2,3,2}, // Americano
        {2,2,1,2}, // Cappuccino
        {3,2,2,2}, // Latte
        {4,0,0,1}, // Coffee with milk
        {5,1,0,1}, // Milk foam
        {6,0,0,1}  // Hot water
    };
    uint8_t rec[10] = {0};
    if (type >= 7) type = 0;
    rec[0] = CoffeePattern[type][0];
    if (rec[0] == 5 || rec[0] == 4) {
        rec[1] = 3; // всегда молотый для молока и milk foam
    } else {
        rec[1] = bean;
    }
    if (cups > CoffeePattern[type][3]) {
        rec[2] = CoffeePattern[type][3];
    } else {
        rec[2] = cups;
    }
    rec[3] = CoffeePattern[type][1];
    rec[4] = CoffeePattern[type][2];
    if (rec[0] == 1) { // Americano
        rec[6] = 40;
        rec[7] = 0;
        rec[8] = (uint8_t)vol;
        rec[9] = vol / 0x100;
        rec[5] = 0;
    } else {
        rec[6] = (uint8_t)vol;
        rec[7] = vol / 0x100;
        if (rec[0] == 0 || rec[0] == 1 || rec[0] == 4 || rec[0] == 5) {
            if (rec[0] == 5) {
                rec[5] = 2;
            } else {
                rec[5] = 0;
            }
            rec[8] = 0;
            rec[9] = 0;
        } else {
            rec[5] = 2;
            rec[8] = (uint8_t)milk;
            rec[9] = milk / 0x100;
        }
    }
    return std::vector<uint8_t>(rec, rec + 10);
}

} // namespace saeco_series_6000
} // namespace esphome 