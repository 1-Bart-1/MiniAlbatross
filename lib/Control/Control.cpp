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
}


void Control::update(State* state){
    ledcWrite(MIDDLE_MOTOR_PIN, (int)state->left_motor.percentage*MAX_DUTY_CYCLE);
    ledcWrite(LEFT_MOTOR_PIN, (int)state->left_motor.percentage*MAX_DUTY_CYCLE);
    ledcWrite(RIGHT_MOTOR_PIN, (int)state->left_motor.percentage*MAX_DUTY_CYCLE);
}

Control control;
