#ifndef State_h
#define State_h

#include <helper_3dmath.h>
#include <math.h>

class Kite{ // TODO: should be struct
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

struct Motor{
    float percentage = 0.0; // percentage of the max input voltage
    float rotation = 0.0; // total rotation
    float speed = 0.0; // rotational velocity
    bool reverse = false;
};

class State{
    public:
        Kite kite;
        Motor middle_motor;
        Motor left_motor;
        Motor right_motor;
        bool enable = false;

        void update();
};


// Global state data structure
extern State state;

#endif
