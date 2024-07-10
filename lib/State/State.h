#ifndef State_h
#define State_h

#include <helper_3dmath.h>
#include <math.h>

class Kite{
    public:
        VectorFloat position;
        float angle; //using axis-angle representation

        Quaternion orientation;
        float wireLength = 50;
        float setpoint = 90; // wanted angle in degrees
        void update();
    private:
        void updateAngle();
        void updatePosition();
};

class Motor{
    public:
        float pid_speed;
        float left_speed;
        float right_speed;
        
        bool enable = true;
        float constant_speed = 0;

        void update();
    private:
        void update_actual_speed();
};

class State{
    public:
        Kite kite;
        Motor motor;

        struct PID_params {
            double Kp=10;
            double Ki=0;
            double Kd=0;
        } pid_params;
        void update();
};


// Global state data structure
extern State state;

#endif
