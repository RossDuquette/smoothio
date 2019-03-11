/********************************************************
                       SMOOTHIO
 List of configuration parameters for the dispense unit
********************************************************/

typedef enum DState { 
    IDLE = 0, 
    DISPENSE 
} DState;


#include "AccelStepper.h"
#define HALFSTEP 8
#define STEP_OFFSET 679

// Configurations
#define NUM_DISPENSE 6
#define DEG_PER_CUP 360/6

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x06

// Pin layout
#define FROZEN1_EN 5
#define FROZEN2_EN 44
#define FROZEN3_EN 45
#define FROZEN4_EN 46
#define POWDER1_EN 43
#define LIQUID1_EN 47
#define LIQUID2_EN 48
#define LIQUID3_EN 49

#define DIR 12
#define STEP 13
#define nEN 33
#define M2 32
#define M1 31
#define M0 30

#define COOLANT_EN 36
#define HEATPUMP_EN 35

#define THERMISTOR1 A1
#define THERMISTOR0 A0

#define ADD_D25 25
#define ADD_D24 24
#define ADD_D23 23
#define ADD_D22 22

#define ADD_A15 A15
#define ADD_A14 A14
#define ADD_A13 A13
#define ADD_A12 A12

#define motorPin1  ADD_A12     // IN1 on the ULN2003 driver 1
#define motorPin2  ADD_A13     // IN2 on the ULN2003 driver 1
#define motorPin3  ADD_A14     // IN3 on the ULN2003 driver 1
#define motorPin4  ADD_A15     // IN4 on the ULN2003 driver 1

