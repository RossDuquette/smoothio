/******************************************************** SMOOTHIO
Prototypes for peripheral control functions
********************************************************/
#include <stdint.h>

/**
* Used to turn dispense motors on and off
* pin defined as FROZEN1_EN -> LIQUID3_EN
**/
bool food_dispense(uint8_t pin, uint8_t on);

/**
* Configures stepper driver
* Sets number of microsteps per full step
* Must be a power of 2; 1 <= steps <= 32
**/
bool stepper_init(uint8_t microsteps);

/**
* Dispenses 1 cup
**/
bool cup_dispense();

/**
* Read thermistor value
* Converts ADC value to temperature in Celsius, returned in value
**/
bool read_temp(uint8_t pin, float* value);