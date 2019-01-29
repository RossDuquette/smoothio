/********************************************************
                      SMOOTHIO
 Functionality of the dispense unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include "peripherals.h"

char number[50];

void setup()
{
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin setup
    pin_setup();

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

bool pin_setup()
{
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

void loop()
{
    delay(100);
}

// callback for received data
void receiveData(int byteCount)
{
    int i = 0;
    while (Wire.available())
    {
        number[i] = Wire.read();
        i++;
    }
    number[i] = '\0';
    Serial.print(number);
}

// callback for sending data
void sendData()
{
    Wire.write(19);
}

