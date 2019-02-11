/********************************************************
                      SMOOTHIO
 Functionality of the carousel unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

DRV8825 stepper(MOTOR_STEPS, DIR, STEP, nEN, M0, M1, M2);
char number[50];

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

void loop() { delay(100); }

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

bool carousel_rotate() {
    stepper.rotate(72); // 360 degrees / 5
    return true;
}
