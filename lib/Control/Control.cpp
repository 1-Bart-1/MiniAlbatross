#include <Control.h>

const char DualshockMacAddress[] = "D8:13:2A:7F:8D:36";

Control::Control() {
}

void Control::begin(State* state) {
    if (state->enableMotor) {
        Serial2.begin(115200);
        while (!Serial2) {;}
        vesc.setSerialPort(&Serial2);
        while(!vesc.getFWversion()){
            Serial.println("Failed to connect to VESC");    
            delay(1000);
        }
        Serial.println("Motor Initialized");
    }

    if(state->controller_support){
        PS4.begin(DualshockMacAddress);
        Serial.println("Connecting to controller...");
        while(true){
            if(PS4.isConnected()){
                Serial.println("Controller connected, press options to start");
                break;
            }
        }
        while(true){
            if(PS4.isConnected() && PS4.Options()){
                break;
            }
        }
        Serial.println("Controller Initialized");
        
    }

}


void Control::update(State* state){
    Serial.print("Updating control: ");

    for (int i=0; i<3; i++) {
        if (state->enableMotor) {
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

void Control::controllerInput(State* state) {
    if (state->controller_support && PS4.isConnected()) {
    
        if (PS4.Cross()){
            state->motors[0].set_speed = 1.0;
            Serial.println("Cross");
        }
        else (state->motors[0].set_speed = 0.0);

        if (PS4.L1()){
            state->motors[1].set_speed = 1.0;
            Serial.println("L1");
        }
        else (state->motors[1].set_speed = 0.0);

        if (PS4.R1()){
            state->motors[2].set_speed = 1.0;
            Serial.println("R1");
        }
        else (state->motors[2].set_speed = 0.0);
   

  }
  
}

Control control;
