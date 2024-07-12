#include <MPU6050_6Axis_MotionApps20.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "Connect.h"
#include "Control.h"
#include "State.h"
#include "UART.h"


void setup() {
    Serial.begin(115200);
    connect.begin(&state);
    uart.begin();
    control.begin(&state);
}


void loop() {
    state.update();
    control.update(&state);
    uart.update(&state);

    // delay(10);
}

