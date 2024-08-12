#include <SparkFunMPU9250-DMP.h>

#include "Orient.h"
#include "Connect.h"
#include "Control.h"
#include "State.h"
#include "UART.h"

MPU9250_DMP imu;
State state;

void setup() {
    orient.begin();
    connect.begin(&state);
    control.begin(&state);
}


void loop() {
    orient.update(&state);
    state.update();
    control.update(&state);
    if (connect.received) {
        connect.received = false;
        // uart.send_state_as_json(&state); // when we get the updated quaternion from the kite, we send state info over serial
    }
    if (uart.receive(&state)) { // if new message is received over uart, we send out a ping to the kite sensor to indicate that we want information back
        uart.send_state_as_json(&state); // when we get the updated quaternion from the kite, we send state info over serial
        connect.sendPing();
    }
}
