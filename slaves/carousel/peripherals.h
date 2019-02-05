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
* Initialize stepper motor driver
**/
bool stepper_init();

/**
* Homes the carousel, runs during initialization
**/
bool carousel_home();

/**
* Rotates the carousel exactly 1 station
**/
bool carousel_rotate();