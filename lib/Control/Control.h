#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <State.h>

#define SPEED 100
#define BUTTON 23


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
