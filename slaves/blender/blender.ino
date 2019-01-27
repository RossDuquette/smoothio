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
    Wire.write(19)
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

bool blend(int *args)
{

    // move_to_position(sealed)
    //
    return true;
}
