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
int32_t pivot_absolute_deg = 0;
int8_t pivot_offset = 4;

state_t states; // Store all system states


/********************
*   Main Functions  *
*********************/
void setup() {
    // pin setup
    pin_setup();

    // Elevator ESC setup
    ESC.attach(ELEV_PWM,1000,2000); // (pin, min pulse width, max pulse width in milliseconds)
    elevator_move(NEUTRAL,0);

    // initialize i2c as slave
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);

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
            delay(500);
            pivot_rotate(NEUTRAL, 0); 
            states.pivot = P_IDLE;
            break;
        case P_CCW:
            pivot_rotate(CCW, PIVOT_SPEED);
            delay(500);
            pivot_rotate(NEUTRAL, 0);
            states.pivot = P_IDLE;
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
            elevator_move(UP, ELEV_BOOST_UP);
            delay(50);
            elevator_move(NEUTRAL, 0);
            delay(50);
            elevator_move(UP, ELEV_SPEED_UP);
            break;
        case E_DESCEND:
            elevator_move(DOWN, ELEV_SPEED_DOWN);
            break;
        case E_HOME:
            home_elev();
            break;
        case E_MIDPOINT:
            if (states.e_homed) {
                elevator_move(DOWN, 140);
                delay(200);
                elevator_move(NEUTRAL, 0);
            }
            states.elevator = E_IDLE;
            break;
        case E_MAXHEIGHT:
            elevator_setHeight(ELEV_MAX_HEIGHT);
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
                case PIV_OFFSET:
                    if (data >= 128) {
                        pivot_offset = data-256;
                    } else {
                        pivot_offset = data;
                    }
                    // no break to also enter reset state
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
    if ((elev_limit_top() == 0 && dir == UP) ||
        (elev_limit_bottom() == 0 && dir == DOWN)) {
        dir = NEUTRAL;
        states.elevator = E_IDLE;
    }
    switch(dir) {
        case UP:
            ESC.writeMicroseconds(ELEV_OFF-speed);
            break;
        case DOWN:
            ESC.writeMicroseconds(ELEV_OFF+speed);
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
    if (states.elevator_height-ELEV_PULSE_RATIO <= height &&
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
    speed = ELEV_GAIN*abs(height-states.elevator_height) + ELEV_STICTION;
    speed = min(ELEV_MAX_SPEED, speed);
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
    uint8_t dir;
    int16_t e, derivative, speed;
    static int16_t integral = 0, prev_e = 0;
    if (states.p_homed == 0) {
        return false;
    }
    // Check if at angle
    if (pivot_absolute_deg == degrees) {
        pivot_rotate(NEUTRAL, 0);
        delay(PIVOT_SS_TIME);
        update_sensors();
        if (pivot_absolute_deg == degrees) { // Double check
            states.pivot = P_IDLE;
            integral = 0;
            return true;
        }
    }

    // Controller
    e = pivot_absolute_deg - degrees;
    integral += e;
    derivative = e - prev_e;
    prev_e = e;
    speed = PIVOT_KP*e + PIVOT_KI*integral + PIVOT_KD*derivative;
    if (speed > 0) {
        dir = CCW;
    } else {
        dir = CW;
        speed = -1*speed;
    }
    speed = min(PIVOT_MAX_SPEED, speed);
    pivot_rotate(dir, (uint8_t)speed);
    delay(10); // Don't let integral grow too quickly
    return true;
}


/*********************
*      Routines      *
**********************/
bool home_elev() {
    // Perform Homing Routine
    elevator_move(UP, ELEV_BOOST_UP);
    delay(100);
    elevator_move(NEUTRAL, 0);
    delay(100);
    elevator_move(UP, ELEV_SPEED_UP);
    while (elev_limit_top());
    elevator_move(NEUTRAL, 0);
    delay(100);
    elev_position = 0;

    // Descend slightly
    elevator_move(DOWN, 150); 
    delay(35);
    elevator_move(NEUTRAL, 0); 
    delay(100);
    states.elevator = E_IDLE;
    states.e_homed = 1;
    return true;
}

bool home_pivot() {
    if (states.p_homed == 0) {
        if (pivot_limit()) {
            pivot_rotate(CCW, PIVOT_SPEED);
            while (pivot_limit());
            pivot_rotate(NEUTRAL, 0);
            pivot_position = pivot_offset / PIVOT_PULSE_RATIO; // Reset encoder counter
            delay(100); // Give time to stop before resetting encoder count
        }
        pivot_setAngle(0); // Return pivot to home position
        states.p_homed = 1; // Set as homed
    } else {
        pivot_setAngle(0);
    }
    return true;
}


/*********************
*   Sensor Updates   *
**********************/
bool update_sensors() {
    states.elev_hall = (uint8_t)digitalRead(ELEV_HALL_SENSOR);
    states.elev_limit_top = (uint8_t)digitalRead(ELEV_LIMIT_TOP);
    states.elev_limit_bot = (uint8_t)digitalRead(ELEV_LIMIT_BOTTOM);
    states.limit2 = (uint8_t)pivot_limit();
    states.pivot_deg = (uint8_t)round(pivot_position*PIVOT_PULSE_RATIO); // Convert to degrees
    pivot_absolute_deg = round(pivot_position*PIVOT_PULSE_RATIO);
    states.elevator_height = (uint8_t)round(elev_position*ELEV_PULSE_RATIO); // Convert to mm
    states.curr_sense0 = analogRead(CURR_SENSE0);
    states.curr_sense1 = analogRead(CURR_SENSE1);
    return true;
}

bool elev_limit_top() {
    // Return reading of elevator top limit switch and hall
    return digitalRead(ELEV_LIMIT_TOP);
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
