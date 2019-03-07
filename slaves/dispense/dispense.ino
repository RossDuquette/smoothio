/********************************************************
                      SMOOTHIO
 Functionality of the dispense unit
********************************************************/

#include <Wire.h>
#include "config.h"

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
// States for each cup dispense +1  for the cup dispenser
uint8_t dstates[NUM_DISPENSE + 1];

uint8_t dispensers[NUM_DISPENSE] = {FROZEN1_EN, FROZEN2_EN, FROZEN3_EN,
                                    LIQUID1_EN, LIQUID2_EN, LIQUID3_EN};

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin and state and servo setup
    pin_setup();
    state_setup();
    stepper_init();

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

bool state_setup() {
    for (int i = 0; i < NUM_DISPENSE + 1; i++) {
        dstates[i] = IDLE;
    }
    return true;
}

bool pin_setup() {
    // Inputs
    pinMode(THERMISTOR1, INPUT);
    pinMode(THERMISTOR0, INPUT);

    // Outputs
    pinMode(FROZEN1_EN, OUTPUT);
    pinMode(FROZEN2_EN, OUTPUT);
    pinMode(FROZEN3_EN, OUTPUT);
    pinMode(FROZEN4_EN, OUTPUT);
    pinMode(POWDER1_EN, OUTPUT);
    pinMode(LIQUID1_EN, OUTPUT);
    pinMode(LIQUID2_EN, OUTPUT);
    pinMode(LIQUID3_EN, OUTPUT);

    pinMode(DIR, OUTPUT);
    pinMode(STEP, OUTPUT);
    pinMode(nEN, OUTPUT);
    pinMode(M2, OUTPUT);
    pinMode(M1, OUTPUT);
    pinMode(M0, OUTPUT);

    pinMode(COOLANT_EN, OUTPUT);
    pinMode(HEATPUMP_EN, OUTPUT);

    pinMode(ADD_D25, OUTPUT);
    pinMode(ADD_D24, OUTPUT);
    pinMode(ADD_D23, OUTPUT);
    pinMode(ADD_D22, OUTPUT);
}

void loop() {
    // Run state machines for each dispenser
    for (int i = 0; i < NUM_DISPENSE; i++) {
        switch (dstates[i]) {
            case IDLE:
                food_dispense(dispensers[i], 0);
                break;
            case DISPENSE:
                food_dispense(dispensers[i], 1);
                break;
        }
    }
    // Run state machine for cup dispenser
    switch (dstates[NUM_DISPENSE]) {
        case IDLE:
            break;
        case DISPENSE:
            cup_dispense();
            break;
    }
}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        uint8_t selector = Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            if (selector == 255) {
                state_setup();
            } else {
                dstates[selector-1] = data;
            }
        }
    }
}

void sendData() {
    // Send state to master
    Wire.write((const char*)&dstates, NUM_DISPENSE+1);
}

/************************
 * Peripheral functions *
 ************************/
bool food_dispense(uint8_t pin, uint8_t on) {
    digitalWrite(pin, on);
    return true;
}

bool stepper_init() {
    stepper1.setMaxSpeed(1000.0);
    stepper1.setAcceleration(1000.0);
    stepper1.setSpeed(300);
    stepper1.moveTo(0);
    return true;
}

bool cup_dispense() {
    static uint8_t dispensing = 0;
    if (stepper1.distanceToGo() == 0) { 
        if (dispensing == 0) { // Start dispensing
            stepper1.moveTo(stepper1.currentPosition()+STEP_OFFSET);
            dispensing = 1;
        } else { // Done dispensing
            dispensing = 0;
            dstates[NUM_DISPENSE] = IDLE;
        }
    } else {
        stepper1.run();
    }
    return true;
}
