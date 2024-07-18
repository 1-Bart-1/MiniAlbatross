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
    uart.begin();
    control.begin(&state);
}


void loop() {
    orient.update(&state);
    state.update();
    control.update(&state);
    uart.update(&state);
}

