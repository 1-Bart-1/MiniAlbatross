#include <Control.h>

const char DualshockMacAddress[] = "D8:13:2A:7F:8D:36";

Control::Control() {
}

void Control::begin(State* state) {
    if (state->enableMotor) {
        Serial.println("Connecting to motor...");
        Serial2.begin(115200);
        while (!Serial2) {;}
        vesc.setSerialPort(&Serial2);
        while(!vesc.getFWversion()){
            Serial.println("Failed to connect to motor, check battery and that uart is connected");    
            delay(1000);
        }

        while (!vesc.getVescValues(can_id[0])) {;}
        state->motors[0].middle_step = vesc.data.tachometer;
        state->motors[1].middle_step = state->motors[0].middle_step;
        state->motors[2].middle_step = state->motors[0].middle_step;

        for (int i=1; i<3; i++) {
            while (!vesc.getVescValues(can_id[i])) {;}
            state->motors[i].reference_offset = state->motors[i].middle_step - vesc.data.tachometer;
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
    //Serial.print("Updating control: ");

    for (int i=0; i<3; i++) {
        if (state->enableMotor) {
            if (state->mode) { // If speed mode (0)
                //Serial.print("Speed mode, ");
                vesc.setRPM(state->motors[i].set_speed*16*60, can_id[i]);
            } else { // If current mode (1)
                //Serial.print("Current mode, ");
                vesc.setCurrent(state->motors[i].set_speed, can_id[i]); 
            }
            if (vesc.getVescValues(can_id[i])) {
                //Serial.println("Got values");
                state->motors[i].speed = vesc.data.rpm/16/60;
                state->motors[i].current = vesc.data.avgInputCurrent;
                state->motors[i].percentage = vesc.data.dutyCycleNow;
                state->motors[i].step = vesc.data.tachometer;
                state->motors[i].middle_step = state->motors[0].step;

                state->motors[i].offset = state->motors[i].middle_step - state->motors[i].step;
            }
            
        } 
        else {
            vesc.setCurrent(0.0, can_id[i]);
        }
    }
}

void Control::controllerInput(State* state) {
    if (state->controller_support && PS4.isConnected()) {
        this -> LStickY = PS4.LStickY();
        this -> RStickY = PS4.RStickY();
        this -> RStickX = PS4.RStickX();

        for (int i=0; i<3; i++) {
            // Kite in/out
            if(this->LStickY >= 20 || this->LStickY <= -20){
                state->motors[i].set_speed = PS4.LStickY()*0.01;
            }
            else{
                state->motors[i].set_speed = 0.0;
            }
        }

        int middleStep = state->motors[0].step;

        this->isSteering = false;

        // Kite side to side
        if(this->RStickX <= -20){ // steer left
            state->motors[1].set_speed += this->RStickX*0.01;
            state->motors[2].set_speed -= this->RStickX*0.01;
            this->isSteering = true;
        }
        else if(this->RStickX >= 20){ //steer right
            state->motors[1].set_speed += this->RStickX*0.01;
            state->motors[2].set_speed -= this->RStickX*0.01;
            this->isSteering = true;
        }

        // Kite power/depower
        if(this->RStickY <= -20){ // power
            state->motors[1].set_speed += this->RStickY*0.01;
            state->motors[2].set_speed += this->RStickY*0.01;
            this->isSteering = true;
        }
        else if(this->RStickY >= 20){ //depower
            state->motors[1].set_speed += this->RStickY*0.01;
            state->motors[2].set_speed += this->RStickY*0.01;
            this->isSteering = true;
        }

        if(!isSteering){

            //return to set point if not steering
            for(int i=0;i<3;i++){
                if(state->motors[i].offset > state->motors[i].reference_offset && state->motors[i].offset - state->motors[i].reference_offset > 20){
                    state->motors[i].set_speed += 1;
                }
                else if(state->motors[i].offset < state->motors[i].reference_offset && state->motors[i].reference_offset - state->motors[i].offset > 20){
                    state->motors[i].set_speed -= 1;
                }
            }
        }
    }
}

Control control;
