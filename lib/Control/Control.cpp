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

    if(last_update_time + sampling_time*1000000 <= micros()){
        last_update_time = micros();
    

        for (int i=0; i<3; i++) {
            if (state->enableMotor) {

                if (vesc.getVescValues(can_id[i])) {
                    //Serial.println("Got values");
                    state->motors[i].speed = vesc.data.rpm/16/60;
                    state->motors[i].current = vesc.data.avgInputCurrent;
                    state->motors[i].percentage = vesc.data.dutyCycleNow;
                    state->motors[i].step = vesc.data.tachometer;
                    state->motors[i].middle_step = state->motors[0].step;

                    state->motors[i].offset = state->motors[i].middle_step - state->motors[i].step;
                }

                if(i == 0 && state->motors[i].current > 20.0){
                    state->enableMotor = false;

                }

                if (state->mode) { // If speed mode (0)
                    //Serial.print("Speed mode, ");
                    vesc.setRPM(state->motors[i].set_speed*16*60, can_id[i]);
                } else { // If current mode (1)
                    //Serial.print("Current mode, ");
                    vesc.setCurrent(state->motors[i].set_speed, can_id[i]); 
                }
                
                
            }

            else {
                vesc.setCurrent(0.0, can_id[i]);
            }

            if(state->controller_support){
            controllerInput(state);
            }
        }
    }
}

// Move motor a number of steps from reference
void Control::moveFromReference(State* state, int motor, int steps) {
    int difference = state->motors[motor].offset - state->motors[motor].reference_offset;
    int e = difference - steps;
    state->motors[motor].set_speed += e*state->Kp;
    }


void Control::controllerInput(State* state) {
    if (state->controller_support && PS4.isConnected()) {
        this -> LStickY = PS4.LStickY();
        this -> RStickY = PS4.RStickY();
        this -> RStickX = PS4.RStickX();

        // Kite in/out
        for (int i=0; i<3; i++) {
            
            if(this->LStickY >= 20 || this->LStickY <= -20){
                state->motors[i].set_speed = PS4.LStickY()*0.01;
            }
            else{
                state->motors[i].set_speed = 0.0;
            }
        }
        this->isSteering = false;

        // Kite side to side
        if(abs(this->RStickX) >= 20){ 
            moveFromReference(state, 1, -int(this->RStickX * RStickTuner));
            moveFromReference(state, 2, +int(this->RStickX * RStickTuner));
            this->isSteering = true;
        }

        // Kite power/depower
        if(abs(this->RStickY) >= 20){ //steer right
            moveFromReference(state, 1, -int(this->RStickY * RStickTuner));
            moveFromReference(state, 2, -int(this->RStickY * RStickTuner));
            this->isSteering = true;
        }


        //return to reference point if not steering
        if(!isSteering){
            for(int i=1;i<3;i++){
                int e = state->motors[i].offset - state->motors[i].reference_offset;
                u_p = e*K;
                //alpha = sampling_time/Ti;
                u_i = u_i + e*K*alpha;
                state->motors[i].set_speed += u_p + u_i;
                
            }
        }
    }
}

Control control;
