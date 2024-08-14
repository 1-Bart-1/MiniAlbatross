#include <Control.h>

Control::Control() {
}

void Control::begin(State* state) {
    Serial2.begin(115200);
    while (!Serial2) {;}
    vesc.setSerialPort(&Serial2);
}


void Control::update(State* state){
    for (int i=0; i<3; i++) {
        if (state->enable) {
            if (state->mode) {
                vesc.setRPM(state->motors[i].set_speed*16*60, can_id[i]);
            } else {
                vesc.setCurrent(state->motors[i].set_speed, can_id[i]); // left
            }
            if (vesc.getVescValues(can_id[i])) {
                state->motors[i].speed = vesc.data.rpm/16/60;
                state->motors[i].current = vesc.data.avgInputCurrent;
                state->motors[i].percentage = vesc.data.dutyCycleNow;
            }
        } else {
            vesc.setCurrent(0.0, can_id[i]);
        }
    }
}

Control control;
