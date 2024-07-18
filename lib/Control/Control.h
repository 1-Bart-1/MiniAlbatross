#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <QuickPID.h>

#include <State.h>

#define VOLTAGE_PIN 34

const uint8_t motor_pins[3] = {15, 2, 4};
const uint8_t reverse_pins[3] = {5, 18, 19};
const uint8_t current_pins[3] = {35, 33, 32};
const uint8_t pwm_channels[3] = {3, 1, 0};

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

const uint32_t sample_time_us = 100000; // 10ms


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

        volatile int middle_motor_step = 0;
        volatile int left_motor_step = 0;
        volatile int right_motor_step = 0;
        volatile bool compute_now = false;  //for counting interrupt
    private:
        float current = 0.0;
        float measure = 0.0;
        std::array<float, 3> current_offsets{2008, 2008, 2008};
        std::array<unsigned long, 3> current_loop_times{micros(), micros(), micros()};
        std::array<unsigned long, 3> last_loop_times{micros(), micros(), micros()};
        
        volatile bool computeNow = false;
        unsigned long last_compute = micros();
        std::array<QuickPID, 3> cruise_controls{QuickPID(), QuickPID(), QuickPID()};

        uint8_t i = 0;
        hw_timer_t * timer = NULL;
};

extern Control control;

void middle_motor_hall_interrupt_1();
void middle_motor_hall_interrupt_2();
void middle_motor_hall_interrupt_3();
void left_motor_hall_interrupt_1();
void left_motor_hall_interrupt_2();
void left_motor_hall_interrupt_3();
void right_motor_hall_interrupt_1();
void right_motor_hall_interrupt_2();
void right_motor_hall_interrupt_3();

void pid_interrupt();

#endif
