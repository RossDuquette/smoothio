/********************************************************
                      SMOOTHIO
 Functionality of the carousel unit
********************************************************/

#include <Wire.h>
#include "config.h"

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

// callback for received data
void receiveData(int byteCount) {
    int i = 0;
    while (Wire.available()) {
        number[i] = Wire.read();
        i++;
    }
    number[i] = '\0';
    Serial.print(number);
}

// callback for sending data
void sendData() { Wire.write(19); }
