#include <Control.h>

Control::Control() {
}

void Control::begin(State* state) {
    Serial2.begin(115200);
    while (!Serial2) {;}
    vesc.setSerialPort(&Serial2);
    while(!vesc.getFWversion()){
        Serial.println("Failed to connect to VESC");    
        delay(1000);
    }
    Serial.println("Motor Initialized");
}


void Control::update(State* state){
    Serial.print("Updating control: ");

    for (int i=0; i<3; i++) {
        if (state->enable) {
            if (state->mode) { // If speed mode (0)
                Serial.print("Speed mode, ");
                vesc.setRPM(state->motors[i].set_speed*16*60, can_id[i]);
            } else { // If current mode (1)
                Serial.print("Current mode, ");
                vesc.setCurrent(state->motors[i].set_speed, can_id[i]); // left
            }
            if (vesc.getVescValues(can_id[i])) {
                Serial.println("Got values");
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
