#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <VescUart.h>

#include <State.h>


/*
Output the Motor state to the correct pins and calculate the speed of the motor
*/
class Control {
    public:
        Control();
        void begin(State* state);
        void update(State* state);
    private:
        float current = 0.0;
        float measure = 0.0;
        uint8_t can_id[3] = {26,52,0};
        VescUart vesc; // left right middle

};

extern Control control;

#endif
