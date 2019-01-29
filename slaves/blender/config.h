/********************************************************
                       SMOOTHIO
 List of configuration parameters for the blending unit
********************************************************/

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

// List of I2C messages
#define FINISHED_COMMAND 0x01

// Pin layout
#define PIVOT_PWM 4
#define PIVOT_ENC_A 2
#define PIVOT_ENC_B 3
#define PIVOT_IN_A 15
#define PIVOT_IN_B 14

#define ELEV_PWM 6
#define ELEV_IN_A 7
#define ELEV_IN_B 8
#define ELEV_ENC_A 19
#define ELEV_ENC_B 18

#define BLEND_EN_0 41
#define BLEND_EN_1 40

#define LIMIT_SENSE 39
#define LIMIT_SENSE_2 A2

#define ADD_D25 25
#define ADD_D24 24
#define ADD_D23 23
#define ADD_D22 22

#define ADD_A15 A15
#define ADD_A14 A14
#define ADD_A13 A13
#define ADD_A12 A12

// Directions
#define NEUTRAL 0
#define CW 1
#define CCW 2
#define UP 3
#define DOWN 4