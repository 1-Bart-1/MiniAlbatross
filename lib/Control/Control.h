#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <VescUart.h>

#include <State.h>
#include <PS4Controller.h>


/*
Output the Motor state to the correct pins and calculate the speed of the motor
*/
class Control {
    public:
        Control();
        void begin(State* state);
        void update(State* state);
        void controllerInput(State* state);
    private:
        float current = 0.0;
        float measure = 0.0;
        uint8_t can_id[3] = {0,26,68}; //  middle, left, right
        VescUart vesc; 
        int8_t LStickY = 0;
        int8_t RStickY = 0;
        int8_t RStickX = 0;
        bool isSteering = false;


};

extern Control control;

#endif
