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
        void moveFromReference(State* state, int motor, int steps);

        
    private:
        float current = 0.0;
        float measure = 0.0;
        uint8_t can_id[3] = {0,26,68}; //  middle, left, right
        VescUart vesc; 
        int8_t LStickY = 0;
        int8_t RStickY = 0;
        int8_t RStickX = 0;
        const float RStickTuner = 0.5;
        bool isSteering = false;

        float K = 0.01;
        float u_p = 0.0;
        float Ti = 10.0;
        float u_i = 0.0;
        float Kd = 0.0;

        unsigned long last_update_time = micros();
        const float sampling_time = 0.1; // in seconds

        float alpha = 0.0;
        

};

extern Control control;

#endif
