#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <State.h>

#define VOLTAGE_PIN 34

#define MIDDLE_MOTOR_PIN 15
#define LEFT_MOTOR_PIN 2
#define RIGHT_MOTOR_PIN 4

#define MIDDLE_MOTOR_REVERSE_PIN 5
#define LEFT_MOTOR_REVERSE_PIN 18
#define RIGHT_MOTOR_REVERSE_PIN 19

#define MIDDLE_MOTOR_CURRENT_PIN 33
#define LEFT_MOTOR_CURRENT_PIN 32
#define RIGHT_MOTOR_CURRENT_PIN 35

#define MIDDLE_MOTOR_HALL_PIN1 13
#define MIDDLE_MOTOR_HALL_PIN2 12
#define MIDDLE_MOTOR_HALL_PIN3 14

#define LEFT_MOTOR_HALL_PIN1 27
#define LEFT_MOTOR_HALL_PIN2 26
#define LEFT_MOTOR_HALL_PIN3 25

#define RIGHT_MOTOR_HALL_PIN1 21
#define RIGHT_MOTOR_HALL_PIN2 22
#define RIGHT_MOTOR_HALL_PIN3 23

// https://lastminuteengineers.com/esp32-pwm-tutorial/
#define PWM_FREQ 300000
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

        int last_middle_hall_sensor = 0;
        int last_left_hall_sensor = 0;
        int last_right_hall_sensor = 0;

        int middle_motor_step = 0;
        int left_motor_step = 0;
        int right_motor_step = 0;
    private:
        float middle_current_offset = 1832.5;
        float left_current_offset = 0.0;
        float right_current_offset = 0.0;
        float sensor_reading = 0.0;
        int i = 0;
};

extern Control control;

void middle_motor_hall_interrupt_1();
void middle_motor_hall_interrupt_2();
void middle_motor_hall_interrupt_3();

#endif
