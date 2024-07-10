#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <State.h>

#define MIDDLE_MOTOR_PIN 15
#define MIDDLE_MOTOR_HALL_PIN 2
#define MIDDLE_MOTOR_REVERSE_PIN 4
#define LEFT_MOTOR_PIN 16
#define LEFT_MOTOR_HALL_PIN 17
#define LEFT_MOTOR_REVERSE_PIN 5
#define RIGHT_MOTOR_PIN 18
#define RIGHT_MOTOR_HALL_PIN 19
#define RIGHT_MOTOR_REVERSE_PIN 21
#define PWM_FREQ 300000 // because of this high frequency, max resolution of 8
#define PWM_RESOLUTION 8
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

/*
Output the Motor state to the correct pins and calculate the speed of the motor
*/
class Control {
    public:
        Control();
        void begin(State* state);
        void update(State* state);
    private:
};

extern Control control;

#endif
