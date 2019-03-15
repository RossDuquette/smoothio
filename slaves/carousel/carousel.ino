/********************************************************
                      SMOOTHIO
 Functionality of the carousel unit
********************************************************/

#include <Wire.h>
#include "config.h"

DRV8825 stepper(MOTOR_STEPS, DIR, STEP, nEN, M0, M1, M2);
float rot_stop_time = 0;
float next_sens_update = 0; 

state_t states;

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin setup
    pin_setup();

    // initialize stepper
    stepper_init();

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    // Init states
    state_setup();
}

bool pin_setup() {
    pinMode(CUP_MASS0, INPUT);
    pinMode(CUP_MASS1, INPUT);
    pinMode(CAROUSEL_POS, INPUT);
    pinMode(CUP_SENSE0, INPUT);
    pinMode(CUP_SENSE1, INPUT);
    pinMode(CLEAN_EN, OUTPUT);
}

bool state_setup() {
    memset(&states, 0, sizeof(state_t));
    states.c_state = IDLE;
    return true;
}

void loop() {
    // Carousel state machine
    switch (states.c_state) {
        case CW:
        case CCW:
            carousel_rotate(states.c_state, states.num_cups);
            break;
        case HOME:
            carousel_home();
            break;
        case DISABLE:
            stepper.disable();
            states.c_state = IDLE;
            break;
        case JIGGLE:
            carousel_jiggle();
            states.c_state = IDLE;
            break;
        case RESET:
            state_setup();
            states.c_state = IDLE;
            break;
        case IDLE:
        default:
            break;
    }
    // Cleaning state machine
    switch (states.clean) {
        case C_IDLE:
            digitalWrite(CLEAN_EN, LOW);
            break;
        case C_ON:
            digitalWrite(CLEAN_EN, HIGH);
            break;
    }
    if (millis() > next_sens_update) {
        read_sensors();
        next_sens_update = next_sens_update + SENS_UPDATE_TIME;
    }
}

void read_sensors() {
    states.cup_sense0 = digitalRead(CUP_SENSE0);
    states.cup_sense1 = digitalRead(CUP_SENSE1);
    states.cup_mass0 = analogRead(CUP_MASS0);
    states.cup_mass1 = analogRead(CUP_MASS1);
    states.carousel_pos = digitalRead(CAROUSEL_POS);
}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            if (selector == 255) {
                state_setup();
            } else if (selector == CLEAN) {
                states.clean = data;
            } else {
                states.c_state = selector;
                states.num_cups = data;
            }
        }
    }
}

// callback for sending data
void sendData() {
    // send back state to master
    Wire.write((const char*)&states, sizeof(states));
}

bool stepper_init() {
    stepper.begin(RPM);
    stepper.setEnableActiveState(LOW);
    stepper.setSpeedProfile(stepper.LINEAR_SPEED, 1000, 1000);
    /*
     * Microstepping mode: 1, 2, 4, 8, 16 or 32
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (at the set RPM)
     */
    stepper.setMicrostep(MICROSTEPS);
    return true;
}

bool carousel_home() {
    stepper.enable();
    stepper.setRPM(HOMING_RPM);
    stepper.startRotate(720);
    while (digitalRead(CAROUSEL_POS) == 1) {
        if (stepper.nextAction() == 0)
            break;
    }
    stepper.stop();
    stepper.setRPM(RPM);
    states.c_state = IDLE;
    read_sensors();
    if (states.carousel_pos == 0) {
        states.homed = 1;
        return true;
    }
    return false;
}

bool carousel_rotate(uint8_t dir, uint8_t n) {
    stepper.enable();
    if (dir == CW) {
        stepper.rotate(DEG_PER_SLOT * n);
    } else if (dir == CCW) {
        stepper.rotate(-DEG_PER_SLOT * n);
    }
    states.c_state = IDLE;
    states.num_cups = 0;
    return true; 
}

bool carousel_jiggle() {
    stepper.enable();
    stepper.rotate(1*GEAR_RATIO);
    stepper.rotate(-2*GEAR_RATIO);
    stepper.rotate(1*GEAR_RATIO);
}
