/********************************************************
                      SMOOTHIO
 Functionality of the carousel unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

DRV8825 stepper(MOTOR_STEPS, DIR, STEP, nEN, M0, M1, M2);
char number[50];
uint8_t cup_offset = 0;

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

bool pin_setup() {}

void loop() {
    if (state_t.num_cups != 0) {
        carousel_rotate(state_t.num_cups);
        delay(state_t.num_cups * CUP_WAIT_TIME);
        state_t.num_cups = 0;
    }
}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            state_t.direction = selector;
            state_t.num_cups = data
        }
    }
}

// callback for sending data
void sendData() {
    // send back state to master
    Wire.write((const char*)&states, sizeof(state_t));
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

bool carousel_rotate(uint8_t n) {
    stepper.rotate(72 * n);  // 360 degrees / 5
    return true;
}
