#ifndef State_h
#define State_h

#include <helper_3dmath.h>
#include <math.h>

class Kite{ // TODO: should be struct
    public:
        VectorFloat position;
        float angle; //using axis-angle representation

        Quaternion q_kite;
        Quaternion q_tether;
        float wire_length = 50;
        float setpoint = 90; // wanted angle in degrees
        void update_angle(){
            Quaternion conjugate = this->q_kite.getNormalized().getConjugate();
            VectorFloat unitVector(0, 0, -1);
            unitVector.rotate(&conjugate);

            this->angle = -atan2f(unitVector.y, unitVector.x) * RAD_TO_DEG;;
        }
        void update_position(){
            VectorFloat wire(0, 0, 1);

            q_kite.normalize();
            wire.rotate(&q_kite);

            this->position = wire;
        }
    private:
};

class Motor{
    public:
        float percentage = 0.0; // percentage of the max input voltage

        int step = 0; // number of coil steps. TODO: how many steps is one rotation? and what is the diameter?
        int last_step = 0;
        int middle_step = 0; // Always has to know step of middle motor
        int offset = 0;
        int reference_offset = 0; // offset from reference step
        bool isSaturated = false;


        
        float acc = 0.0;
        float speed = 0.0;
        float set_speed = 0.0; // wanted speed in rps or ampere
        float current = 0.0;
        bool reverse = false;
        unsigned long last_update_time = micros();
        float force = 0.0;
};

class State{
    public:
        Kite kite;
        std::array<Motor, 3> motors{Motor(), Motor(), Motor()}; // middle, left, right motor

        float voltage = 0.0;
        unsigned long start_time = millis();
        unsigned long last_receive_time = micros();
        bool enableMotor = false;
        bool mode = 1; // 0. current mode 1. speed mode
        float Kp = 0.01;
        float Ti = 1;
        float Kd = 0.0;
        bool controller_support = false; // Support for PS4 controller
        
        void update(){
            kite.update_angle();
            kite.update_position();
        }
};

#endif
