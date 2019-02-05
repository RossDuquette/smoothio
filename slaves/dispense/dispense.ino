/********************************************************
                      SMOOTHIO
 Functionality of the dispense unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

typedef enum DState { LED1, LED2, LED3 } DState;
DState dstate = IDLE;
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, nEN, M0, M1, M2);

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin setup
    pin_setup();

    // Initialize stepper motor driver
    stepper_init();

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

/************************
 * Peripheral functions *
 ************************/
bool food_dispense(uint8_t pin, uint8_t on) {
    digitalWrite(pin, on);
    return true;
}

bool stepper_init() {
    stepper.begin(RPM);
    stepper.setEnableActiveState(LOW);
    stepper.enable();
    stepper.setMicrostep(MICROSTEPS);
    return true;
}

bool cup_dispense() {
    stepper.rotate(60);  // 360 degrees / 6
    return true;
}

bool read_temp(uint8_t pin, float* value) { return true; }
