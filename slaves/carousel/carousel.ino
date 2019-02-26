/********************************************************
                      SMOOTHIO
 Functionality of the carousel unit
********************************************************/

#include <Wire.h>
#include "peripherals.h"

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

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

bool pin_setup() {
    pinMode(CURR_SENSE0, INPUT);
    pinMode(CURR_SENSE1, INPUT); 
    pinMode(CUP_MASS0, INPUT);
    pinMode(CUP_MASS1, INPUT);
    pinMode(CAROUSEL_POS, INPUT);
    pinMode(CUP_SENSE0, INPUT);
    pinMode(CUP_SENSE1, INPUT);
}

void loop() {
    if (states.num_cups != 0) {
        // Not sure if time() is the right call
        rot_stop_time = millis() + CUP_WAIT_TIME*states.num_cups;
        carousel_rotate(states.c_state, states.num_cups);
    }
    switch(states.c_state) {
        case C_IDLE:
            break;
        case MOVING_CW:
            if (millis() > rot_stop_time)
                states.c_state = C_IDLE;
            break;
        case MOVING_CCW:
            if (millis() > rot_stop_time)
                states.c_state = C_IDLE; 
            break;
    }
    if (millis() > next_sens_update) {
        read_sensors();
        next_sens_update = next_sens_update + SENS_UPDATE_TIME;
    }
}

void read_sensors() {
    states.cup_sense0 = analogRead(CUP_SENSE0);
    states.cup_sense1 = analogRead(CUP_SENSE1);
    states.curr_sense0 = analogRead(CURR_SENSE0);
    states.curr_sense1 = analogRead(CURR_SENSE1);
    states.cup_mass0 = analogRead(CUP_MASS0);
    states.cup_mass1 = analogRead(CUP_MASS1);
    states.carousel_pos = analogRead(CAROUSEL_POS);
}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            states.c_state = selector;
            states.num_cups = data;
        }
    }
}

// callback for sending data
void sendData() {
    // send back state to master
    Wire.write((const char*)&states, sizeof(states));
}

bool stepper_init(uint8_t microsteps) {
    stepper.begin(RPM);
    stepper.setEnableActiveState(LOW);
    stepper.enable();
    /*
     * Microstepping mode: 1, 2, 4, 8, 16 or 32
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (at the set RPM)
     */
    stepper.setMicrostep(16);
    return true;
}

bool carousel_home() { return true; }

bool carousel_rotate(uint8_t dir, uint8_t n) {
    if (dir == CW) {
        stepper.rotate(72 * n);  // 360 degrees / 5
        return true;
    }
   stepper.rotate(-72 * n);  // 360 degrees / 5
   return true; 
}
