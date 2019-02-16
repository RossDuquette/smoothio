/********************************************************
                       SMOOTHIO
 List of configuration parameters for the carousel unit
********************************************************/

typedef enum COMM_SELECTOR {  // Decode message from master
    CW = 1,
    CCW,
    RESET = 255
} COMM_SELECTOR;

// State variables, for communication
typedef struct state_t {
    // Outputs
    uint8_t direction;
    uint8_t num_cups;

    // Inputs
    uint8_t cup_sense1;
    uint8_t cup_sense2;
} state_t;

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x05

// Misc
#define CUP_WAIT_TIME 1000;

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

#define CURR_SENS1 A7
#define CURR_SENS0 A6
#define CUP_MASS1 A5
#define CUP_MASS0 A4

#define CAROUSEL_POS A2
#define CUP_SENSE1 A1
#define CUP_SENSE0 A0
