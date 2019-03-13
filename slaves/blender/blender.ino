/********************************************************
                      SMOOTHIO
 Functionality of the blending unit
********************************************************/

#include <Wire.h>
#include "config.h"
#include <Servo.h>

Servo ESC;
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

    // Elevator ESC setup
    ESC.attach(ELEV_PWM,1000,2000); // (pin, min pulse width, max pulse width in milliseconds)
    elevator_move(NEUTRAL,0);

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
            home_pivot();
            break;
        case P_ROTATE_180:
            pivot_setAngle(180);
            break;
    }

    // Elevator state machine
    switch (states.elevator) {
        case E_IDLE:
            elevator_move(NEUTRAL, 0);
            break;
        case E_ASCEND:
            elevator_move(UP, ELEV_SPEED_UP);
            break;
        case E_DESCEND:
            elevator_move(DOWN, ELEV_SPEED_DOWN);
            break;
        case E_HOME:
            home_elev();
            break;
        case E_MIDPOINT:
            elevator_setHeight(ELEV_MID_HEIGHT);
            break;
        case E_MAXHEIGHT:
            elevator_setHeight(ELEV_MAX_HEIGHT);
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
    pinMode(ELEV_HALL_SENSOR, INPUT);
    pinMode(PIVOT_HALL_SENSOR, INPUT);

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
    static uint16_t duty[2] = {0,0};
    const uint16_t MAX_DUTY = 2000;
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
    } else {
#ifdef BLENDER_SOFTSTART
        if (duty[blend_id] < MAX_DUTY) { // Soft start
            digitalWrite(blender_pin, 1);
            delayMicroseconds(duty[blend_id]);
            digitalWrite(blender_pin, 0);
            delayMicroseconds(MAX_DUTY-duty[blend_id]);
            duty[blend_id]++;
        } else {
            digitalWrite(blender_pin, 1);
        }
#else
        digitalWrite(blender_pin, 1);
#endif
    }
    return true;
}

bool elevator_move(uint8_t dir, uint16_t speed) {
    speed = min(ELEV_MAX_SPEED, speed);
    if ((elev_limit_top() == 0 && dir == E_ASCEND) ||
        (elev_limit_bottom() == 0 && dir == E_DESCEND)) {
        dir = NEUTRAL;
        states.elevator = E_IDLE;
    }
    switch(dir) {
        case UP:
            ESC.writeMicroseconds(ELEV_OFF+speed);
            break;
        case DOWN:
            ESC.writeMicroseconds(ELEV_OFF-speed);
            break;
        case NEUTRAL:
        default:
            ESC.writeMicroseconds(ELEV_OFF);
            break;
    }
    return true;
}

bool elevator_setHeight(uint8_t height) {
    if (states.e_homed == 0) {
        return false;
    }
    // Check if at height, within a pulse
    if (states.elevator_height <= height &&
        states.elevator_height+ELEV_PULSE_RATIO >= height) {
        elevator_move(NEUTRAL, 0);
        states.elevator = E_IDLE;
        return true;
    }
    // Determine direction
    uint8_t dir, speed;
    if (states.elevator_height > height) {
        dir = UP;
    } else { // states.elevator_height < height
        dir = DOWN;
    }
    // Determine speed, P control
    speed = ELEV_GAIN*abs((int16_t)height-(int16_t)states.elevator_height) + ELEV_STICTION;
    elevator_move(dir, speed);
    return true;
}

bool pivot_rotate(uint8_t dir, uint8_t speed) {
    if (dir == NEUTRAL) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, LOW);
        speed = 0;
    } else if (dir == CW) {
        digitalWrite(PIVOT_IN_A, LOW);
        digitalWrite(PIVOT_IN_B, HIGH);
        speed = min(PIVOT_MAX_SPEED, speed+PIVOT_STICTION);
    } else if (dir == CCW) {
        digitalWrite(PIVOT_IN_A, HIGH);
        digitalWrite(PIVOT_IN_B, LOW);
        speed = min(PIVOT_MAX_SPEED, speed+PIVOT_STICTION);
    } else {
        return false;
    }
    analogWrite(PIVOT_PWM, speed);
    return true;
}

bool pivot_setAngle(uint8_t degrees) {
    uint8_t dir, speed;
    uint16_t e;
    static uint16_t integral = 0;
    if (states.p_homed == 0) {
        return false;
    }
    // Check if at angle
    if (states.pivot_deg == degrees) {
        pivot_rotate(NEUTRAL, 0);
        delay(PIVOT_SS_TIME);
        update_sensors();
        if (states.pivot_deg == degrees) { // Double check
            states.pivot = P_IDLE;
            integral = 0;
            return true;
        }
    }
    // Determine direction
    if (states.pivot_deg > degrees) {
        dir = CW;
    } else { // states.pivot_deg < degrees
        dir = CCW;
    }
    // Determine speed, PI control
    e = abs((int16_t)degrees-(int16_t)states.pivot_deg);
    integral += e;
    speed = min(0xFF, round(e*PIVOT_KP + integral*PIVOT_KI));
    pivot_rotate(dir, speed);
    delay(1); // Don't let integral grow too quickly
    return true;
}


/*********************
*      Routines      *
**********************/
bool home_elev() {
    if (states.e_homed == 0) {
        elevator_move(DOWN, ELEV_SPEED_DOWN);
        delay(100);
        elevator_move(NEUTRAL, 0);
        delay(100);
        elevator_move(UP, ELEV_SPEED_UP);
        while (elev_limit_top());
        elevator_move(NEUTRAL, 0);
        delay(100); // Give time to stop before resetting encoder count
        states.elevator = E_IDLE;
        states.e_homed = 1;
        elev_position = 0;
    } else if (states.elevator_height < 20) { // Getting close, look for limit
        elevator_move(UP, ELEV_SPEED_UP);
        while (elev_limit_top());
        elevator_move(NEUTRAL, 0);
        delay(100);
        states.elevator = E_IDLE;
        elev_position = 0;
    } else {
        elevator_setHeight(0);
    }
    return true;
}

bool home_pivot() {
    if (states.p_homed == 0) {
        if (pivot_limit()) {
            pivot_rotate(CW, PIVOT_SPEED);
            while (pivot_limit());
            pivot_rotate(NEUTRAL, 0);
            delay(100); // Give time to stop before resetting encoder count
        }
        states.pivot = P_IDLE; // Turn off pivot
        states.p_homed = 1; // Set as homed
        pivot_position = 0; // Reset encoder counter
    } else if (states.pivot_deg < 20) { // Getting close, look for limit
        pivot_rotate(CW, PIVOT_SPEED);
        while (pivot_limit());
        pivot_rotate(NEUTRAL, 0);
        delay(100);
        states.elevator = E_IDLE;
        elev_position = 0;
    } else {
        pivot_setAngle(0);
    }
    return true;
}


/*********************
*   Sensor Updates   *
**********************/
bool update_sensors() {
    states.limit1 = (uint8_t)(elev_limit_top() && elev_limit_bottom());
    states.limit2 = (uint8_t)pivot_limit();
    states.pivot_deg = (uint8_t)round(pivot_position*PIVOT_PULSE_RATIO); // Convert to degrees
    states.elevator_height = (uint8_t)round(elev_position*ELEV_PULSE_RATIO); // Convert to mm
    states.curr_sense0 = analogRead(CURR_SENSE0);
    states.curr_sense1 = analogRead(CURR_SENSE1);
    return true;
}

bool elev_limit_top() {
    // Return reading of elevator top limit switch and hall
    return digitalRead(ELEV_HALL_SENSOR) && digitalRead(ELEV_LIMIT_TOP);
}

bool elev_limit_bottom() {
    // Return reading of elevator limit switch
    return digitalRead(ELEV_LIMIT_BOTTOM);
}

bool pivot_limit() {
    // Return reading of pivot limit switch, make it active low
    return (digitalRead(PIVOT_HALL_SENSOR) == HIGH) ? 0 : 1;
}


/*****************
*      ISRs      *
******************/
void elev_enc_isr_A() {
    if (digitalRead(ELEV_ENC_B) == LOW) {
        elev_position++;
    } else {
        elev_position--;
    }
}

void elev_enc_isr_B() {
    if (digitalRead(ELEV_ENC_A) == LOW) {
        elev_position--;
    } else {
        elev_position++;
    }
}

void pivot_enc_isr_A() {
    if (digitalRead(PIVOT_ENC_B) == LOW) {
        pivot_position++;
    } else {
        pivot_position--;
    }
}

void pivot_enc_isr_B() {
    if (digitalRead(PIVOT_ENC_A) == LOW) {
        pivot_position--;
    } else {
        pivot_position++;
    }
}
