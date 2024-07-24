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
        float filtered_percentage = 0.0;
        int step = 0; // number of coil steps. TODO: how many steps is one rotation? and what is the diameter?
        int last_step = 0;
        float filtered_acc = 0.0;
        float filtered_speed = 0.0;
        float set_speed = 0.0; // wanted speed
        float current = 0.0;
        bool reverse = false;
        unsigned long last_update_time = micros();

        float force = 0.0;

        void calc_speed(){
            delta_time = (float)(micros() - last_update_time) / 1000000.0;
            if (last_step != step) {
                speed = (float)(step - last_step) / 48.0f / delta_time; // speed in rev/second
                acc = (speed - last_speed) / delta_time;
                last_speed = speed;
                last_step = step;
                last_update_time = micros();
            }
            if (speed > 0.0f) {
                intermediate_speed = 1.0f / 48.0f / delta_time; // speed if the next step would be now
                if (intermediate_speed < speed) {
                    speed = intermediate_speed;
                    acc = (speed - last_speed) / delta_time;
                }
            } else {
                intermediate_speed = -1.0f / 48.0f / delta_time;
                if (intermediate_speed > speed) {
                    speed = intermediate_speed;
                    acc = (speed - last_speed) / delta_time;
                }
            }
            filtered_speed = filtered_speed * 0.95 + speed * 0.05;
            filtered_acc = filtered_acc * 0.95 + acc * 0.05;
        }

        void calc_force(){
            /*
            dspeed_dt = (force*r - b * theta_dot + K * i) / J
            dspeed_dt * J = force*r - b * theta_dot + K * i
            force*r = dspeed_dt * J + b * theta_dot - K * i
            force = (dspeed_dt * J + b * theta_dot - K * i) / r
            */
            force = filtered_acc*params[0] + filtered_speed/params[1] + current/params[2];
        }
    private:
        const float params[3] = {-0.07793932,  0.08304421,  0.02154289};
        float speed = 0.0; // rotational velocity
        float acc = 0.0;
        float last_speed = 0.0;
        float delta_time = 0.0;
        float intermediate_speed = 0.0;
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
        float Ki = 0.5;
        float Kd = 0.0;
        
        void update(){
            kite.update_angle();
            kite.update_position();
            for (int i=0; i<=2; i++){
                motors[i].calc_speed();
                motors[i].calc_force();
            }
        }
};

#endif
