#ifndef State_h
#define State_h

#include <helper_3dmath.h>
#include <math.h>

class Kite{ // TODO: should be struct
    public:
        VectorFloat position;
        float angle; //using axis-angle representation

        Quaternion orientation;
        float wire_length = 50;
        float setpoint = 90; // wanted angle in degrees
        void update_angle(){
            Quaternion conjugate = this->orientation.getNormalized().getConjugate();
            VectorFloat unitVector(0, 0, -1);
            unitVector.rotate(&conjugate);

            this->angle = -atan2f(unitVector.y, unitVector.x) * RAD_TO_DEG;;
        }
        void update_position(){
            orientation = this->orientation;
            VectorFloat wire(0, 0, 1);

            // normalize the positional quaternion, so we can use it to multiply
            orientation.normalize();
            
            wire.rotate(&orientation);

            this->position = wire;
        }
    private:
};

struct Motor{
    float percentage = 0.0; // percentage of the max input voltage
    float filtered_percentage = 0.0;
    int step = 0; // number of coil steps. TODO: how many steps is one rotation? and what is the diameter?
    float rotation = 0.0; // total rotation
    float speed = 0.0; // rotational velocity
    float set_speed = 0.0; // wanted speed
    float avg_speed = 0.0;
    float current = 0.0;
    bool reverse = false;
};

class State{
    public:
        Kite kite;
        std::array<Motor, 3> motors{Motor(), Motor(), Motor()}; // middle, left, right motor

        float voltage = 0.0;
        unsigned long start_time = millis();
        unsigned long last_receive_time = micros();
        bool enable = false;
        bool mode = 1; // 0. percentage mode 1. speed mode
        float Kp = 0.05;
        float Ki = 0.0;
        float Kd = 0.0;
        
        void update(){
            kite.update_angle();
            kite.update_position();
        }
};

#endif
