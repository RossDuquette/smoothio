#include "peripherals.h"

bool food_dispense(uint8_t pin, uint8_t on)
{
    digitalWrite(pin,on);
    return true;
}

bool cup_dispense()
{
    return true;
}

bool read_temp(uint8_t pin, float* value)
{
    return true;
}