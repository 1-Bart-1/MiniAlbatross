
//#include <SparkFunMPU9250-DMP.h>

//#include "Orient.h"
//#include "Connect.h"
#include "Control.h"
#include "State.h"
#include "UART.h"



//MPU9250_DMP imu;
State state;



void setup() {
    //orient.begin();
    //connect.begin(&state);
    Serial.begin(9600);
    Serial.println("Startup...");
    state.enable = true;
    state.mode = 1; // 0 for current, 1 for speed
    control.begin(&state);
    
}


void loop() {
    //orient.update(&state);
    //state.update();
    for(int i = 0; i < 3; i++) {
        state.motors[i].set_speed = 2;
         control.update(&state);
    }
    
    //Serial.println(state.motors[0].set_speed);
    for(int i = 0; i < 3; i++) {
        state.motors[i].set_speed = 0;
        control.update(&state);
    }
   
    //Serial.println(state.motors[0].set_speed);
    //Serial.print(vesc.getFWversion(0))
    /*if (connect.received) {
        connect.received = false;
        // uart.send_state_as_json(&state); // when we get the updated quaternion from the kite, we send state info over serial
    }
    if (uart.receive(&state)) { // if new message is received over uart, we send out a ping to the kite sensor to indicate that we want information back
        uart.send_state_as_json(&state); // when we get the updated quaternion from the kite, we send state info over serial
        connect.sendPing();
        
    }*/
}


