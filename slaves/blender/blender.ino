/********************************************************
                      SMOOTHIO
 Functionality of the blending unit
********************************************************/

#include <Wire.h>
#include "config.h"

uint32_t elev_position = 0;
uint32_t pivot_position = 0;

state_t states;

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

void loop() {
    // Debug
    // digitalWrite(ADD_D24,LOW);
    // delay(1000);
    // digitalWrite(ADD_D24,HIGH);
    // delay(1000);

    // Output state machine

    // Update sensor values

}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            switch (selector) {
                case BLEND1:
                    states.blender1 = (BLENDER)data;
                    break;
                case BLEND2:
                    states.blender2 = (BLENDER)data;
                    break;
                case PIV:
                    states.pivot = (PIVOT)data;
                    break;
                case ELEV:
                    states.elevator = (ELEVATOR)data;
                    break;
                case ROUT:
                    states.routine = (ROUTINE)data;
                    break;
                case RESET:
                default:
                    // Set all states to IDLE
                    memset(&states, 0, sizeof(state_t));
                    break;
            }
        }
    }
}

// callback for sending data
void sendData() {
    Wire.write((const char*)&states, sizeof(state_t));
}


bool pin_setup() {
    // Limit sense
    pinMode(PIVOT_ENC_A, INPUT);
    pinMode(PIVOT_ENC_B, INPUT);
    pinMode(ELEV_ENC_A, INPUT);
    pinMode(ELEV_ENC_B, INPUT);
    pinMode(LIMIT_SENSE, INPUT);
    pinMode(LIMIT_SENSE_2, INPUT);

    // Outputs
    pinMode(PIVOT_PWM, OUTPUT);
    pinMode(PIVOT_IN_A, OUTPUT);
    pinMode(PIVOT_IN_B, OUTPUT);

    pinMode(ELEV_PWM, OUTPUT);
    pinMode(ELEV_IN_A, OUTPUT);
    pinMode(ELEV_IN_B, OUTPUT);

    pinMode(BLEND_EN_0, OUTPUT);
    pinMode(BLEND_EN_1, OUTPUT);

    pinMode(ADD_D25, OUTPUT);
    pinMode(ADD_D24, OUTPUT);
    pinMode(ADD_D23, OUTPUT);
    pinMode(ADD_D22, OUTPUT);

    // ISR for encoders
    attachInterrupt(digitalPinToInterrupt(ELEV_ENC_A), elev_enc_isr_A, RISING);
    attachInterrupt(digitalPinToInterrupt(ELEV_ENC_B), elev_enc_isr_B, RISING);
}

/*******************************************
 LIMIT_SENSE: Limit switch for elev
 LIMIT_SENSE_2: Limit switch for pivot
********************************************/
bool elev_limit() {
    // Return reading of elevator limit switch
    return digitalRead(LIMIT_SENSE);
}

bool pivot_limit() {
    // Return reading of pivot limit switch
    return digitalRead(LIMIT_SENSE_2);
}

bool blender_control(uint8_t blender_pin, uint8_t on) {
    digitalWrite(blender_pin, on);
    return true;
}

/*******************************************
 ELEV_IN_A: CW input (High / Low)
 ELEV_IN_B: CCW input (High / Low)
 ELEV_PWM: Speed (Duty cycle)
********************************************/
bool move_elevator(uint8_t dir, uint8_t speed) {
    if (dir == NEUTRAL || speed == 0) {
        digitalWrite(ELEV_IN_A, LOW);
        digitalWrite(ELEV_IN_B, LOW);
        analogWrite(ELEV_PWM, 0);
    } else if (dir == UP) {
        digitalWrite(ELEV_IN_A, HIGH);
        digitalWrite(ELEV_IN_B, LOW);
        analogWrite(ELEV_PWM, speed);
    } else if (dir == DOWN) {
        digitalWrite(ELEV_IN_A, LOW);
        digitalWrite(ELEV_IN_B, HIGH);
        analogWrite(ELEV_PWM, speed);
    } else {
        return false;
    }
    return true;
}

/*******************************************
 PIVOT_IN_A: CW input (High / Low)
 PIVOT_IN_B: CCW input (High / Low)
 PIVOR_PWM: Speed (Duty cycle)
********************************************/
bool rotate_pivot(uint8_t dir, uint8_t speed) {
    if (dir == NEUTRAL || speed == 0) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, LOW);
        analogWrite(PIVOT_PWM, 0);
    } else if (dir == CW) {
        digitalWrite(PIVOT_IN_A, HIGH);
        digitalWrite(PIVOT_IN_B, LOW);
        analogWrite(PIVOT_PWM, speed);
    } else if (dir == CCW) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, HIGH);
        analogWrite(PIVOT_PWM, speed);
    } else {
        return false;
    }
    return true;
}

/*******************************************
 ELEV_ENC_A: Value of encoder A
 ELEV_ENC_B: Value of encoder B
********************************************/
void elev_enc_isr_A() {
    if (digitalRead(ELEV_ENC_B) == HIGH) {
        elev_position++;
    } else {
        elev_position--;
    }
}

void elev_enc_isr_B() {
    if (digitalRead(ELEV_ENC_A) == HIGH) {
        elev_position--;
    } else {
        elev_position++;
    }
}

/*******************************************
 PIVOT_ENC_A: Value of encoder A
 PIVOT_ENC_B: Value of encoder B
********************************************/
void pivot_enc_isr_A() {
    if (digitalRead(PIVOT_ENC_B) == HIGH) {
        pivot_position++;
    } else {
        pivot_position--;
    }
}

void pivot_enc_isr_B() {
    if (digitalRead(PIVOT_ENC_A) == HIGH) {
        pivot_position--;
    } else {
        pivot_position++;
    }
}

bool calibrate_elev() {}

bool calibrate_pivot() {}

