/********************************************************
                      SMOOTHIO
 Functionality of the dispense unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

DRV8825 stepper(MOTOR_STEPS, DIR, STEP, nEN, M0, M1, M2);
typedef enum DState { IDLE, DISPENSE } DState;
// States for each cup dispense +1  for the cup dispenser
DState dstates[NUM_DISPENSE + 1];

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
    Serial.println("Starting Dispense Node");

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
    food_dispense(FROZEN1_EN, HIGH);
    // Run state machines for each dispenser
    for (int i = 0; i < NUM_DISPENSE; i++) {
        switch (dstates[i]) {
            case IDLE:
                digitalWrite(22, LOW);
                digitalWrite(23, LOW);
                break;
            case DISPENSE:
                Serial.print("Dispenser ");
                Serial.print(i);
                Serial.println(" Dispensing");
                digitalWrite(22, HIGH);
                digitalWrite(23, LOW);
                break;
        };
    }
    // Run state machine for cup dispenser
    switch (dstates[NUM_DISPENSE]) {
        case IDLE:
            digitalWrite(22, LOW);
            digitalWrite(23, LOW);
            break;
        case DISPENSE:
            Serial.println("Cup Dispenser Dispensing");
            digitalWrite(22, LOW);
            digitalWrite(23, HIGH);
            break;
    };
}

// Variable for I2C daisy chain test
uint8_t i2c_data[2];

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        i2c_data[0] = Wire.read();
        if (Wire.available()) {
            i2c_data[1] = Wire.read();
        }
    }
}

void sendData() { Wire.write((const char*)i2c_data, 2); }

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
