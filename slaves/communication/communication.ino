/*
  I2C Pinouts
  SDA -> A4
  SCL -> A5
*/

// Import the library required
#include <Wire.h>

// Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char number[50];
int state = 0;

// Code Initialization
void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    Serial.println("Serial init'd");
}

void loop() { delay(100); }

// callback for received data
void receiveData(int byteCount) {
    int number;
    Serial.print("byte count=");
    Serial.println(byteCount);

    while (Wire.available()) {
        number = Wire.read();
        Serial.println((int)number);
    }
}

// callback for sending data
void sendData() { Wire.write(19); }
