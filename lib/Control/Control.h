#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <State.h>

#define VOLTAGE_PIN 34

const uint8_t motor_pins[] = {15, 2, 4};
const uint8_t reverse_pins[] = {5, 18, 19};
const uint8_t current_pins[] = {35, 33, 32};

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
        float current = 0.0;
        float measure = 0.0;
        std::array<float, 3> current_offsets{2008, 2008, 2008};
        std::array<unsigned long, 3> current_loop_times{micros(), micros(), micros()};
        std::array<unsigned long, 3> last_loop_times{micros(), micros(), micros()};
        
        // float sensor_reading = 0.0;
        uint8_t i = 0;
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

#endif
