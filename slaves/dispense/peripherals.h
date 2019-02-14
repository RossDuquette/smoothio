/********************************************************
SMOOTHIO
Prototypes for peripheral control functions
********************************************************/
#include <stdint.h>
#include "config.h"

// Library found at https://github.com/laurb9/StepperDriver
#include "DRV8825.h"
#define MOTOR_STEPS 200 // Per revolution
#define RPM 120
/**
 * Microstepping mode: 1, 2, 4, 8, 16 or 32
 * Mode 1 is full speed.
 * Mode 32 is 32 microsteps per step.
 * The motor should rotate just as fast (at the set RPM) 
**/
#define MICROSTEPS 16

/**
* Used to turn dispense motors on and off
* pin defined as FROZEN1_EN -> LIQUID3_EN
**/
bool food_dispense(uint8_t pin, uint8_t on);

/**
* Initialize stepper motor driver
**/
bool stepper_init();

/**
* Dispenses 1 cup
**/
bool cup_dispense();

/**
* Read thermistor value
* Converts ADC value to temperature in Celsius, returned in value
**/
bool read_temp(uint8_t pin, float* value);


