/********************************************************
                      SMOOTHIO
 Functionality of the blending unit
********************************************************/

#include <Wire.h>

char number[50];

// Code Initialization
void setup()
{
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    // calibration
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
    blend();
    Wire.write(19)
}

bool blend(int *args)
{
    return true;
}

bool run_blend_motor()
{
}

bool calibrate_vertical()
{
}

bool calibrate_yaw()
{
}

bool move_to_position()
{
}
