#include <Control.h>

Control::Control() {
    

}

void Control::begin(State* state) {
    ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(2, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MIDDLE_MOTOR_PIN, 0);
    ledcAttachPin(LEFT_MOTOR_PIN, 1);
    ledcAttachPin(RIGHT_MOTOR_PIN, 2);

    pinMode(VOLTAGE_PIN, INPUT);

    pinMode(MIDDLE_MOTOR_REVERSE_PIN, OUTPUT);
    pinMode(LEFT_MOTOR_REVERSE_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_REVERSE_PIN, OUTPUT);

    pinMode(MIDDLE_MOTOR_CURRENT_PIN, INPUT);
    pinMode(LEFT_MOTOR_CURRENT_PIN, INPUT);
    pinMode(RIGHT_MOTOR_CURRENT_PIN, INPUT);

    pinMode(MIDDLE_MOTOR_HALL_PIN1, INPUT);
    pinMode(MIDDLE_MOTOR_HALL_PIN2, INPUT);
    pinMode(MIDDLE_MOTOR_HALL_PIN3, INPUT);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN1, middle_motor_hall_interrupt_1, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN2, middle_motor_hall_interrupt_2, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN3, middle_motor_hall_interrupt_3, RISING);
}


void Control::update(State* state){
    ledcWrite(0, (int)(state->middle_motor.filtered_percentage*MAX_DUTY_CYCLE));
    ledcWrite(1, (int)(state->left_motor.percentage*MAX_DUTY_CYCLE));
    ledcWrite(2, (int)(state->right_motor.percentage*MAX_DUTY_CYCLE));

    if (state->middle_motor.reverse) digitalWrite(MIDDLE_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(MIDDLE_MOTOR_REVERSE_PIN, LOW);
    if (state->left_motor.reverse) digitalWrite(LEFT_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(LEFT_MOTOR_REVERSE_PIN, LOW);
    if (state->left_motor.reverse) digitalWrite(RIGHT_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(RIGHT_MOTOR_REVERSE_PIN, LOW);

    state->voltage = analogRead(VOLTAGE_PIN);

    // if (this->i < 1000) {
    //     right_current_offset += analogRead(RIGHT_MOTOR_CURRENT_PIN)*0.001;
    //     this->i++;
    // }
    
    state->middle_motor.current = (analogRead(MIDDLE_MOTOR_CURRENT_PIN) - 1832.5) * 0.01901175471312378;
    state->left_motor.current = (analogRead(LEFT_MOTOR_CURRENT_PIN) - 1834.5) * 0.018141429100333208;
    state->right_motor.current = (analogRead(RIGHT_MOTOR_CURRENT_PIN) - 1808.3) * 0.018605814403539776;
    // Serial.println(state->right_motor.current);
    // Serial.println(state->middle_motor.current);

    /*
    multiplier: -4/(min-mid)/2 + 4/(max-mid)/2
    offset: mid
    middle motor
    offset -4A: 1621.2
    offset 0A: 1832.5
    offset 4A: 2042.15

    left motor
    offset -4A: 1615.5
    offset 0A: 1834.5
    offset 4A: 2056.5

    right motor
    offset -4A: 1595.0
    offset 0A: 1808.3
    offset 4A: 2025.0
    */
    
    state->middle_motor.step = middle_motor_step;
    state->right_motor.step = right_motor_step;
    state->left_motor.step = left_motor_step;
}

Control control;

void IRAM_ATTR middle_motor_hall_interrupt_1() {
    if (control.last_middle_hall_sensor == 2) control.middle_motor_step--;
    else if (control.last_middle_hall_sensor == 3) control.middle_motor_step++;
    if (control.last_middle_hall_sensor != 1) control.last_middle_hall_sensor = 1;
}
void IRAM_ATTR middle_motor_hall_interrupt_2() {
    if (control.last_middle_hall_sensor == 1) control.middle_motor_step++;
    else if (control.last_middle_hall_sensor == 3) control.middle_motor_step--;
    control.last_middle_hall_sensor = 2;
}
void IRAM_ATTR middle_motor_hall_interrupt_3() {
    if (control.last_middle_hall_sensor == 1) control.middle_motor_step--;
    else if (control.last_middle_hall_sensor == 2) control.middle_motor_step++;
    control.last_middle_hall_sensor = 3;
}

void IRAM_ATTR left_motor_hall_interrupt_1() {
    if (control.last_left_hall_sensor == 2) control.left_motor_step--;
    else if (control.last_left_hall_sensor == 3) control.left_motor_step++;
    if (control.last_left_hall_sensor != 1) control.last_left_hall_sensor = 1;
}
void IRAM_ATTR left_motor_hall_interrupt_2() {
    if (control.last_left_hall_sensor == 1) control.left_motor_step++;
    else if (control.last_left_hall_sensor == 3) control.left_motor_step--;
    control.last_left_hall_sensor = 2;
}
void IRAM_ATTR left_motor_hall_interrupt_3() {
    if (control.last_left_hall_sensor == 1) control.left_motor_step--;
    else if (control.last_left_hall_sensor == 2) control.left_motor_step++;
    control.last_left_hall_sensor = 3;
}

void IRAM_ATTR right_motor_hall_interrupt_1() {
    if (control.last_right_hall_sensor == 2) control.right_motor_step--;
    else if (control.last_right_hall_sensor == 3) control.right_motor_step++;
    if (control.last_right_hall_sensor != 1) control.last_right_hall_sensor = 1;
}
void IRAM_ATTR right_motor_hall_interrupt_2() {
    if (control.last_right_hall_sensor == 1) control.right_motor_step++;
    else if (control.last_right_hall_sensor == 3) control.right_motor_step--;
    control.last_right_hall_sensor = 2;
}
void IRAM_ATTR right_motor_hall_interrupt_3() {
    if (control.last_right_hall_sensor == 1) control.right_motor_step--;
    else if (control.last_right_hall_sensor == 2) control.right_motor_step++;
    control.last_right_hall_sensor = 3;
}
