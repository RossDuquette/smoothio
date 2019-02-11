/********************************************************
                      SMOOTHIO
 Functionality of the blending unit
********************************************************/

#include <Wire.h>
#include "config.h"

int32_t elev_position = 0;
int32_t pivot_position = 0;

state_t states;

void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
    Serial.println("Starting Blender Module");

    // pin setup
    pin_setup();

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

void loop() {
    // Blender state machines
    switch (states.blender0) {
        case B_IDLE:
            blender_control(BLEND_EN_0, LOW);
            break;
        case B_ON:
            blender_control(BLEND_EN_0, HIGH);
            Serial.println("Blender 0 on");
            break;
    };
    switch (states.blender1) {
        case B_IDLE:
            blender_control(BLEND_EN_1, LOW);
            break;
        case B_ON:
            blender_control(BLEND_EN_1, HIGH);
            Serial.println("Blender 1 on");
            break;
    };

    // Pivot state machine
    switch (states.pivot) {
        case P_IDLE:
            rotate_pivot(NEUTRAL, PIVOT_SPEED);
            break;
        case P_CW:
            rotate_pivot(CW, PIVOT_SPEED);
            Serial.println("Turning CW");
            break;
        case P_CCW:
            rotate_pivot(CCW, PIVOT_SPEED);
            Serial.println("Turning blender CCW");
            break;
        case P_HOME:
            home_pivot();
            Serial.println("Homing pivot");
            break;
    };

    // Elevator state machine
    switch (states.elevator) {
        case E_IDLE:
            move_elevator(NEUTRAL, ELEV_SPEED);
            break;
        case E_ASCEND:
            move_elevator(UP, ELEV_SPEED);
            Serial.println("Ascending elevator");
            break;
        case E_DESCEND:
            move_elevator(DOWN, ELEV_SPEED);
            Serial.println("Descending elevator");
            break;
        case E_HOME:
            home_elev();
            Serial.println("Homing elevator");
            break;
    };

    // Routine state machine
    switch (states.routine) {
        case R_IDLE:
            break;
        case R_BLEND_AND_CLEAN:
            Serial.println("Blending and cleaning");
            break;
        case R_CLEAN:
            Serial.println("Cleaning blender");
            break;
        case R_BLEND:
            Serial.println("Blending");
            break;
    }

    // Update sensor readings
    update_sensors();
}

// callback for received data
void receiveData(int byteCount) {
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            switch (selector) {
                case BLEND0:
                    states.blender0 = (BLENDER)data;
                    break;
                case BLEND1:
                    states.blender1 = (BLENDER)data;
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
void sendData() { Wire.write((const char*)&states, sizeof(state_t)); }

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
    attachInterrupt(digitalPinToInterrupt(PIVOT_ENC_A), pivot_enc_isr_A, RISING);
    attachInterrupt(digitalPinToInterrupt(PIVOT_ENC_B), pivot_enc_isr_B, RISING);
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

bool home_elev() {
    if (states.limit1) {
        move_elevator(NEUTRAL, ELEV_SPEED);
        states.elevator = E_IDLE;
        elev_position = 0;
    } else {
        move_elevator(UP, ELEV_SPEED);
    }
}

bool home_pivot() {
    if (states.limit2) {
        rotate_pivot(NEUTRAL, PIVOT_SPEED);
        states.pivot = P_IDLE;
        pivot_position = 0;
    } else {
        rotate_pivot(CW, PIVOT_SPEED); // Check homing direction
    }
}

bool update_sensors() {
    // Read limit switches
    states.limit1 = (uint8_t)elev_limit();
    states.limit2 = (uint8_t)pivot_limit();
    // TO DO: Convert pivot encoder to angle
    // TO DO: Convert elevator encoder to height
}
