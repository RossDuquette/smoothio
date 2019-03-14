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
    E_HOME,
    E_MIDPOINT,
    E_MAXHEIGHT
} ELEVATOR;

typedef enum PIVOT {  // Pivot state
    P_IDLE = 0,
    P_CW,
    P_CCW,
    P_HOME,
    P_ROTATE_180
} PIVOT;

// State variables, for communication
typedef struct state_t {
    // Outputs
    uint8_t blender0;
    uint8_t blender1;
    uint8_t elevator;
    uint8_t pivot;

    // Inputs
    uint8_t p_homed;
    uint8_t e_homed;
    uint8_t pivot_deg;
    uint8_t elevator_height;  // in mm, measured from top
    uint8_t elev_hall;
    uint8_t elev_limit_top;
    uint8_t elev_limit_bot;
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

#define ELEV_LIMIT_TOP A1
#define ELEV_LIMIT_BOTTOM A0
#define ELEV_HALL_SENSOR 39
#define PIVOT_HALL_SENSOR A2

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
#define PIVOT_KI 0
#define PIVOT_KD 1
#define PIVOT_SS_TIME 100
#define PIVOT_STICTION 100
#define PIVOT_GEAR_RATIO (298*80/35.0) // Gear motor and external gears
#define PIVOT_PULSES_REV (PIVOT_GEAR_RATIO*6) // Pulses/rev
#define PIVOT_PULSE_RATIO (360.0/(float)PIVOT_PULSES_REV) // Degrees/pulse
#define PIVOT_SPEED 150
#define PIVOT_MAX_SPEED (200-PIVOT_STICTION)

// Elevator parameters
#define ELEV_OFF 1500
#define ELEV_GAIN 0.1
#define ELEV_STICTION 150
#define ELEV_PULSE_RATIO 1 // mm/pulse
#define ELEV_SPEED_UP 180
#define ELEV_SPEED_DOWN 220
#define ELEV_BOOST_UP 200
#define ELEV_MAX_SPEED 250
#define ELEV_MID_HEIGHT 40
#define ELEV_MAX_HEIGHT 100
