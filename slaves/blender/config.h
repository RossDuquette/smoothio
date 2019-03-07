/********************************************************
                       SMOOTHIO
 List of configuration parameters for the blending unit
********************************************************/

// Define enumerations
typedef enum COMM_SELECTOR {  // Decode message from master
    BLEND0 = 1,
    BLEND1,
    PIV,
    ELEV,
    ROUT,
    RESET = 255
} COMM_SELECTOR;

typedef enum BLENDER {  // Blender state
    B_IDLE = 0,
    B_ON
} BLENDER;

typedef enum ELEVATOR {  // Elevator state
    E_IDLE = 0,
    E_ASCEND,
    E_DESCEND,
    E_HOME
} ELEVATOR;

typedef enum PIVOT {  // Pivot state
    P_IDLE = 0,
    P_CW,
    P_CCW,
    P_HOME,
    P_ROTATE_180
} PIVOT;

typedef enum ROUTINE {  // Complex commands
    R_IDLE = 0,
    R_BLEND_AND_CLEAN,
    R_CLEAN,
    R_BLEND
} ROUTINE;

// State variables, for communication
typedef struct state_t {
    // Outputs
    uint8_t blender0;
    uint8_t blender1;
    uint8_t elevator;
    uint8_t pivot;
    uint8_t routine;

    // Inputs
    uint8_t p_homed;
    uint8_t e_homed;
    uint8_t pivot_deg;
    uint8_t elevator_height;  // in mm, measured from top
    uint8_t limit1;
    uint8_t limit2;
    uint8_t curr_sense0;
    uint8_t curr_sense1;
} state_t;

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

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

#define CURR_SENSE1 A7
#define CURR_SENSE0 A6

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

// Pivot parameters
#define PIVOT_KP 1
#define PIVOT_KI 0.01
#define PIVOT_STICTION 0
#define PIVOT_GEAR_RATIO (298*80/35.0) // Gear motor and external gears
#define PIVOT_PULSES_REV (PIVOT_GEAR_RATIO*6) // Pulses/rev
#define PIVOT_PULSE_RATIO (360.0/(float)PIVOT_PULSES_REV) // Degrees/pulse
#define PIVOT_SPEED 35
#define PIVOT_MAX_SPEED 255
#define PIVOT_OFFSET 10 // Reduces the chances of wrap-around

// Elevator parameters
#define ELEV_GAIN 1
#define ELEV_STICTION 0
#define ELEV_PULSES_REV 123 // TO DO: Pulses/rev
#define ELEV_PULSE_RATIO 1 // TO DO: mm/pulse
#define ELEV_SPEED 255
#define ELEV_MAX_SPEED 255
#define ELEV_MAX_HEIGHT 200

