/********************************************************
                      SMOOTHIO
 Functionality of the blending unit
********************************************************/

#include <Wire.h>
#include "config.h"

volatile int32_t elev_position = 0; // Count elevator encoder pulses
volatile int32_t pivot_position = 0; // Count pivot encoder pulses

state_t states; // Store all system states


/********************
*   Main Functions  *
*********************/
void setup() {
    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

    // pin setup
    pin_setup();

    // define callbacks for i2c communication
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    // Init states
    memset(&states, 0, sizeof(state_t));
}

void loop() {
    // Blender state machines
    switch (states.blender0) {
        case B_IDLE:
            blender_control(BLEND_EN_0, LOW);
            break;
        case B_ON:
            blender_control(BLEND_EN_0, HIGH);
            break;
    }
    switch (states.blender1) {
        case B_IDLE:
            blender_control(BLEND_EN_1, LOW);
            break;
        case B_ON:
            blender_control(BLEND_EN_1, HIGH);
            break;
    }

    // Pivot state machine
    switch (states.pivot) {
        case P_IDLE:
            pivot_rotate(NEUTRAL, 0);
            break;
        case P_CW:
            pivot_rotate(CW, PIVOT_SPEED);
            break;
        case P_CCW:
            pivot_rotate(CCW, PIVOT_SPEED);
            break;
        case P_HOME:
            if (states.p_homed == 0) {
                home_pivot();
            } else {
                pivot_setAngle(0);
            }
            break;
        case P_ROTATE_180:
            pivot_setAngle(180);
            break;
    }

    // Elevator state machine
    switch (states.elevator) {
        case E_IDLE:
            elevator_move(NEUTRAL, ELEV_SPEED);
            break;
        case E_ASCEND:
            elevator_move(UP, ELEV_SPEED);
            break;
        case E_DESCEND:
            elevator_move(DOWN, ELEV_SPEED);
            break;
        case E_HOME:
            if (states.e_homed == 0) {
                home_elev();
            } else {
                elevator_setHeight(ELEV_MAX_HEIGHT);
            }
            break;
    }

    // Routine state machine
    switch (states.routine) {
        case R_IDLE:
            break;
        case R_BLEND_AND_CLEAN:
            break;
        case R_CLEAN:
            break;
        case R_BLEND:
            break;
    }

    // Update sensor readings
    update_sensors();
}


/************************
*     I2C Functions     *
*************************/
void receiveData(int byteCount) {
    if (byteCount != 2) { // Faulty message, empty buffer
        while (Wire.available()) {
            Wire.read();
        }
        return;
    }
    if (Wire.available()) {
        COMM_SELECTOR selector = (COMM_SELECTOR)Wire.read();
        if (Wire.available()) {
            uint8_t data = Wire.read();
            switch (selector) {
                case BLEND0:
                    states.blender0 = data;
                    break;
                case BLEND1:
                    states.blender1 = data;
                    break;
                case PIV:
                    states.pivot = data;
                    break;
                case ELEV:
                    states.elevator = data;
                    break;
                case ROUT:
                    states.routine = data;
                    break;
                case RESET:
                default:
                    state_setup();
                    break;
            }
        }
    }
}

void sendData() { 
    update_sensors();
    Wire.write((const char*)&states, sizeof(state_t)); 
}


/************************
*       Pin Setup       *
*************************/
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

bool state_setup() {
    memset(&states, 0, sizeof(state_t));
    return true;
}


/********************
*   Motor Control   *
*********************/
bool blender_control(uint8_t blender_pin, uint8_t on) {
    static uint32_t duty[2] = {0,0};
    const uint32_t MAX_DUTY = 2000;
    uint8_t blend_id;
    // Find blender ID
    if (blender_pin == BLEND_EN_0) {
        blend_id = 0;
    } else {
        blend_id = 1;
    }
    // Write to blender
    if (on == 0) {
        digitalWrite(blender_pin, 0);
        duty[blend_id] = 0;
        return true;
    } else {
        if (duty[blend_id] < MAX_DUTY) { // Soft start
            digitalWrite(blender_pin, 1);
            delayMicroseconds(duty[blend_id]);
            digitalWrite(blender_pin, 0);
            delayMicroseconds(MAX_DUTY-duty[blend_id]);
            duty[blend_id]++;
        } else {
            digitalWrite(blender_pin, 1);
        }
    }
    return true;
}

bool elevator_move(uint8_t dir, uint8_t speed) {
    static uint8_t prev_speed = 0;
    const uint8_t ACCEL_DELAY = 40, DECEL_DELAY = 1;
    const uint8_t ACCEL_INC = 1, DECEL_DEC = 10;
    // Add stiction, ramp speed
    speed = min(ELEV_MAX_SPEED, speed+ELEV_STICTION);
//    if (speed > prev_speed) {
//        prev_speed += ACCEL_INC;
//        speed = prev_speed;
//        delay(ACCEL_DELAY);
//    }
//    else if (speed < prev_speed && prev_speed > DECEL_DEC) {
//        prev_speed -= DECEL_DEC;
//        speed = prev_speed;
//        delay(DECEL_DELAY);
//    }
    // Write to pins
    if (dir == NEUTRAL || speed == 0) {
        digitalWrite(ELEV_IN_A, LOW);
        digitalWrite(ELEV_IN_B, LOW);
        analogWrite(ELEV_PWM, 0);
    } else if (dir == UP) { // CW
        digitalWrite(ELEV_IN_A, HIGH);
        digitalWrite(ELEV_IN_B, LOW);
        analogWrite(ELEV_PWM, speed);
    } else if (dir == DOWN) { // CCW
        digitalWrite(ELEV_IN_A, LOW);
        digitalWrite(ELEV_IN_B, HIGH);
        analogWrite(ELEV_PWM, speed);
    } else {
        return false;
    }
    return true;
}

bool elevator_setHeight(uint8_t height) {
    if (states.e_homed == 0) {
        return false;
    }
    // Check if at height
    if (height == states.elevator_height) {
        states.elevator = E_IDLE;
        elevator_move(NEUTRAL, 0);
        return true;
    }
    // Determine direction
    uint8_t dir, speed;
    if (height < states.elevator_height) {
        dir = UP;
    } else { // height > states.elevator_height
        dir = DOWN;
    }
    // Determine speed, P control
    speed = ELEV_GAIN*abs((int16_t)height-(int16_t)states.elevator_height);
    elevator_move(dir, speed);
    return true;
}

bool pivot_rotate(uint8_t dir, uint8_t speed) {
    speed = min(PIVOT_MAX_SPEED, speed+PIVOT_STICTION);
    if (dir == NEUTRAL) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, LOW);
        speed = 0;
    } else if (dir == CW) {
        digitalWrite(PIVOT_IN_A, HIGH);
        digitalWrite(PIVOT_IN_B, LOW);
    } else if (dir == CCW) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, HIGH);
    } else {
        return false;
    }
    analogWrite(PIVOT_PWM, speed);
    return true;
}

bool pivot_setAngle(uint8_t degrees) {
    if (states.p_homed == 0) {
        return false;
    }
    // Check if at angle
    if (degrees == states.pivot_deg) {
        states.pivot = P_IDLE;
        pivot_rotate(NEUTRAL, 0);
        return true;
    }
    // Determine direction
    uint8_t dir, speed;
    if (degrees < states.pivot_deg) {
        dir = CW;
    } else { // degrees > states.pivot_deg
        dir = CCW;
    }
    // Determine speed, P control
    speed = PIVOT_GAIN*abs((int16_t)degrees-(int16_t)states.pivot_deg);
    pivot_rotate(dir, speed);
    return true;
}


/*********************
*      Routines      *
**********************/
bool home_elev() {
    if (digitalRead(LIMIT_SENSE)) {
        elevator_move(NEUTRAL, ELEV_SPEED);
        states.elevator = E_IDLE;
        states.e_homed = 1;
        elev_position = 0;
    } else {
        elevator_move(UP, ELEV_SPEED);
    }
}

bool home_pivot() {
    if (digitalRead(LIMIT_SENSE_2)) {
        pivot_rotate(NEUTRAL, 0);
        states.pivot = P_IDLE; // Turn off pivot
        states.p_homed = 1; // Set as homed
        pivot_position = 0; // Reset encoder counter
    } else {
        pivot_rotate(CW, PIVOT_SPEED); // Check homing direction
    }
}


/*********************
*   Sensor Updates   *
**********************/
bool update_sensors() {
    // Read limit switches
    states.limit1 = (uint8_t)elev_limit();
    states.limit2 = (uint8_t)pivot_limit();
    // Convert pivot encoder to angle
    // Map from (0,0) to (PIVOT_PULSES,180)
    states.pivot_deg = (uint8_t)(pivot_position*PIVOT_PULSE_RATIO);
    states.elevator_height = (uint8_t)(-8.7);
    states.curr_sense0 = analogRead(CURR_SENSE0);
    states.curr_sense1 = analogRead(CURR_SENSE1);
}

bool elev_limit() {
    // Return reading of elevator limit switch
    return digitalRead(LIMIT_SENSE);
}

bool pivot_limit() {
    // Return reading of pivot limit switch
    return digitalRead(LIMIT_SENSE_2);
}


/*****************
*      ISRs      *
******************/
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




