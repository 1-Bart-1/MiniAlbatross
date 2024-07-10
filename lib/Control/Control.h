#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <State.h>

#define MIDDLE_MOTOR_PIN 15
#define MIDDLE_MOTOR_REVERSE_PIN 2
#define MIDDLE_MOTOR_HALL_PIN1 13
#define MIDDLE_MOTOR_HALL_PIN2 12
#define MIDDLE_MOTOR_HALL_PIN3 14

#define LEFT_MOTOR_PIN 4
#define LEFT_MOTOR_REVERSE_PIN 5
#define LEFT_MOTOR_HALL_PIN1 27
#define LEFT_MOTOR_HALL_PIN2 26
#define LEFT_MOTOR_HALL_PIN3 25

#define RIGHT_MOTOR_PIN 18
#define RIGHT_MOTOR_REVERSE_PIN 19
#define RIGHT_MOTOR_HALL_PIN1 33
#define RIGHT_MOTOR_HALL_PIN2 32
#define RIGHT_MOTOR_HALL_PIN3 35

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
        int middle_motor_step = 0;
        int last_hall_sensor = 0;
    private:
};

extern Control control;

void middle_motor_hall_interrupt_1();
void middle_motor_hall_interrupt_2();
void middle_motor_hall_interrupt_3();

#endif
