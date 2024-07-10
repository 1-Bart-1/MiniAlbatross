#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <PID_v1.h>
#include <State.h>
#include <FastAccelStepper.h>

#define SPEED 100
#define BUTTON 23

class Control {
    public:
        Control();
        void begin(State* state);
        void update();
        void set_params();
    private:
        State* state;
        PID kitePID;
        FastAccelStepper *left = NULL;
        FastAccelStepper *right = NULL;
        FastAccelStepperEngine engine = FastAccelStepperEngine();
};

extern Control control;

#endif
