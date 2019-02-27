/********************************************************
                       SMOOTHIO
 List of configuration parameters for the carousel unit
********************************************************/

typedef enum COMM_SELECTOR {  // Decode message from master
    CW = 1,
    CCW,
    HOME,
    DISABLE, // Disables the stepper motor, re-enabled on next motion
    IDLE = 255
} COMM_SELECTOR;

// State variables, for communication
typedef struct state_t {
    // Outputs
    uint8_t c_state;
    uint8_t num_cups;

    // Inputs
    uint8_t cup_sense0;
    uint8_t cup_sense1;
    uint8_t cup_mass0;
    uint8_t cup_mass1;
    uint8_t carousel_pos;
} state_t;

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x05

// Library found at https://github.com/laurb9/StepperDriver
#include "DRV8825.h"
#define MOTOR_STEPS 200 // Per revolution
#define RPM 120
#define HOMING_RPM 30
/**
 * Microstepping mode: 1, 2, 4, 8, 16 or 32
 * Mode 1 is full speed.
 * Mode 32 is 32 microsteps per step.
 * The motor should rotate just as fast (at the set RPM) 
**/
#define MICROSTEPS 16

// Misc
#define CUP_WAIT_TIME 1000
#define SENS_UPDATE_TIME 100
#define GEAR_RATIO 6.7
#define DEG_PER_SLOT (360/5*GEAR_RATIO)

// Pin Layout
#define DIR 12
#define STEP 13
#define nEN 33
#define M2 32
#define M1 31
#define M0 30

#define ADD_D25 25
#define ADD_D24 24
#define ADD_D23 23
#define ADD_D22 22

#define ADD_A15 A15
#define ADD_A14 A14
#define ADD_A13 A13
#define ADD_A12 A12

#define CUP_MASS1 A5
#define CUP_MASS0 A4

#define CAROUSEL_POS A2
#define CUP_SENSE1 A1
#define CUP_SENSE0 A0
