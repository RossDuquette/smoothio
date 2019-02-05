/********************************************************
                      SMOOTHIO
 Functionality of the dispense unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

typedef enum DState { LED1, LED2, LED3 } DState;

DState dstate = IDLE;

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin setup
    pin_setup();

    // Set stepper mode
    stepper_init(1);  // Full steps

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
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
    // cup_dispense();
    // delay(100);
    food_dispense(FROZEN1_EN, HIGH);
    switch (dstate) {
        case LED1:
            digitalWrite(22, LOW);
            digitalWrite(23, HIGH);
            break;
        case LED2:
            digitalWrite(22, HIGH);
            digitalWrite(23, LOW);
            break;
    };
}

// callback for received data
void receiveData(int byteCount) {
    int number;
    int idx = 0;
    int data[2];
    while (Wire.available()) {
        number = (int)Wire.read();
        Serial.println(number);
        data[idx] = number idx++;
    }
    switch (data[1]) {
        case 1:
            dstate = LED1;
            break;
        case 2:
            dstate = LED2;
            break;
        case 3:
            dstate = LED3;
            break;
    }
}

void sendData() {
    // Polled by master
    Wire.write(19);
}

/***********************
 * Peripheral functions *
 ************************/
bool food_dispense(uint8_t pin, uint8_t on) {
    digitalWrite(pin, on);
    return true;
}

bool stepper_init(uint8_t microsteps) {
    if (microsteps == 0) {
        return false;
    }
    uint8_t mode = 0;
    while ((microsteps & 0x01) == 0) {
        mode++;
        microsteps = microsteps >> 1;
    }
    digitalWrite(M2, (mode >> 2) & 0x01);
    digitalWrite(M1, (mode >> 1) & 0x01);
    digitalWrite(M0, (mode >> 0) & 0x01);
    digitalWrite(nEN, HIGH);
    digitalWrite(DIR, HIGH);
    digitalWrite(STEP, LOW);
    return true;
}

bool cup_dispense() {
    const float STEPS = 200 / 6.0;
    digitalWrite(nEN, LOW);

    // Delay to make sure driver is enabled before sending pulses
    uint8_t mode = 0, microsteps = 1;
    mode |= digitalRead(M2) << 2;
    mode |= digitalRead(M1) << 1;
    mode |= digitalRead(M0);
    while (mode != 0) {
        microsteps = microsteps << 1;
        mode--;
    }
    microsteps = max(microsteps, 32);

    // Send pulses
    uint8_t num_steps = 0, total_steps = ceil(STEPS * microsteps);
    while (num_steps < total_steps) {
        digitalWrite(STEP, HIGH);
        digitalWrite(STEP, LOW);
        num_steps++;
    }

    // Disable driver
    digitalWrite(nEN, HIGH);
    return true;
}

bool read_temp(uint8_t pin, float* value) { return true; }
