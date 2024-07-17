#include <MPU6050_6Axis_MotionApps20.h>

#include "Connect.h"
#include "Control.h"
#include "State.h"
#include "UART.h"


void setup() {
    connect.begin(&state);
    uart.begin();
    control.begin(&state);
}


void loop() {
    state.update();
    control.update(&state);
    uart.update(&state);
}

