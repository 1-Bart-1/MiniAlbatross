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

    pinMode(MIDDLE_MOTOR_REVERSE_PIN, OUTPUT);
    pinMode(LEFT_MOTOR_REVERSE_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_REVERSE_PIN, OUTPUT);

    pinMode(MIDDLE_MOTOR_HALL_PIN1, INPUT_PULLDOWN);
    pinMode(MIDDLE_MOTOR_HALL_PIN2, INPUT_PULLDOWN);
    pinMode(MIDDLE_MOTOR_HALL_PIN3, INPUT_PULLDOWN);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN1, middle_motor_hall_interrupt_1, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN2, middle_motor_hall_interrupt_2, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN3, middle_motor_hall_interrupt_3, RISING);
}


void Control::update(State* state){
    ledcWrite(0, (int)(state->middle_motor.percentage*MAX_DUTY_CYCLE));
    ledcWrite(1, (int)(state->left_motor.percentage*MAX_DUTY_CYCLE));
    ledcWrite(2, (int)(state->right_motor.percentage*MAX_DUTY_CYCLE));
    
    if (state->middle_motor.reverse) digitalWrite(MIDDLE_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(MIDDLE_MOTOR_REVERSE_PIN, LOW);
    if (state->left_motor.reverse) digitalWrite(LEFT_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(LEFT_MOTOR_REVERSE_PIN, LOW);
    if (state->left_motor.reverse) digitalWrite(LEFT_MOTOR_REVERSE_PIN, HIGH);
    else digitalWrite(LEFT_MOTOR_REVERSE_PIN, LOW);
    
    state->middle_motor.step = middle_motor_step;
}

Control control;

void IRAM_ATTR middle_motor_hall_interrupt_1() {
    if (control.last_hall_sensor == 2) {
        control.middle_motor_step--;
    } else if (control.last_hall_sensor == 3) {
        control.middle_motor_step++;
    }
    if (control.last_hall_sensor != 1){
        control.last_hall_sensor = 1;
    }
}
void IRAM_ATTR middle_motor_hall_interrupt_2() {
    if (control.last_hall_sensor == 1) {
        control.middle_motor_step++;
    } else if (control.last_hall_sensor == 3) {
        control.middle_motor_step--;
    }
    control.last_hall_sensor = 2;
}
void IRAM_ATTR middle_motor_hall_interrupt_3() {
    if (control.last_hall_sensor == 1) {
        control.middle_motor_step--;
    } else if (control.last_hall_sensor == 2) {
        control.middle_motor_step++;
    }
    control.last_hall_sensor = 3;
}