#include <Control.h>

Control::Control() {
    

}

void Control::begin(State* state) {
    // pinMode(VOLTAGE_PIN, INPUT);

    for(int i=0; i<2; i++) {
        ledcSetup(i, PWM_FREQ, PWM_RESOLUTION);
        ledcAttachPin(motor_pins[i], i);
        pinMode(reverse_pins[i], OUTPUT);
        pinMode(current_pins[i], INPUT);
    }

    pinMode(MIDDLE_MOTOR_HALL_PIN1, INPUT);
    pinMode(MIDDLE_MOTOR_HALL_PIN2, INPUT);
    pinMode(MIDDLE_MOTOR_HALL_PIN3, INPUT);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN1, middle_motor_hall_interrupt_1, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN2, middle_motor_hall_interrupt_2, RISING);
    attachInterrupt(MIDDLE_MOTOR_HALL_PIN3, middle_motor_hall_interrupt_3, RISING);
    pinMode(LEFT_MOTOR_HALL_PIN1, INPUT);
    pinMode(LEFT_MOTOR_HALL_PIN2, INPUT);
    pinMode(LEFT_MOTOR_HALL_PIN3, INPUT);
    attachInterrupt(LEFT_MOTOR_HALL_PIN1, left_motor_hall_interrupt_1, RISING);
    attachInterrupt(LEFT_MOTOR_HALL_PIN2, left_motor_hall_interrupt_2, RISING);
    attachInterrupt(LEFT_MOTOR_HALL_PIN3, left_motor_hall_interrupt_3, RISING);
    pinMode(RIGHT_MOTOR_HALL_PIN1, INPUT);
    pinMode(RIGHT_MOTOR_HALL_PIN2, INPUT);
    pinMode(RIGHT_MOTOR_HALL_PIN3, INPUT);
    attachInterrupt(RIGHT_MOTOR_HALL_PIN1, right_motor_hall_interrupt_1, RISING);
    attachInterrupt(RIGHT_MOTOR_HALL_PIN2, right_motor_hall_interrupt_2, RISING);
    attachInterrupt(RIGHT_MOTOR_HALL_PIN3, right_motor_hall_interrupt_3, RISING);
}


void Control::update(State* state){
    if (i<2) i++;
    else i=0;

    ledcWrite(i, (int)(state->motors[i].percentage*MAX_DUTY_CYCLE));

    measure = analogRead(current_pins[i]);
    if (state->enable) {
        if (state->motors[i].reverse) digitalWrite(reverse_pins[i], HIGH);
        else digitalWrite(reverse_pins[i], LOW);
    } else {
        current_offsets[i] = current_offsets[i]*0.999 + measure*0.001;
        // Serial.println(middle_current_offset);
    }

    // state->voltage = analogRead(VOLTAGE_PIN);

    current_loop_times[i] = micros();
    if (last_loop_times[i] > state->last_receive_time){
        current = -(measure - current_offsets[i]) * 0.014541622760800843;
        state->motors[i].current = (current * (current_loop_times[i] - last_loop_times[i]) + state->motors[i].current * (last_loop_times[i] - state->last_receive_time)) / (last_loop_times[i] - state->last_receive_time);
    } else {
        state->motors[i].current = -(measure - current_offsets[i]) * 0.014541622760800843;
    }
    last_loop_times[i] = current_loop_times[i];

    analogRead(current_pins[i==2?0:i+1]); // read next pin to setup analogread
    // Serial.println(state->right_motor.current);
    // Serial.println(current_offset);

    /*
    multiplier: -5/(min-mid)/2 + 5/(max-mid)/2
    offset: mid
    middle motor
    offset -5A: 2600
    offset 0A: 2925
    offset 5A: 3290

    left motor
    offset -5A: 2621
    offset 0A: 2940
    offset 5A: 3306

    right motor
    offset -5A: 2617
    offset 0A: 2947.5
    offset 5A: 3329
    */
    
    state->motors[0].step = middle_motor_step;
    state->motors[1].step = left_motor_step;
    state->motors[2].step = right_motor_step;
}

Control control;

void IRAM_ATTR middle_motor_hall_interrupt_1() {
    if (control.last_middle_hall_sensor == 2) control.middle_motor_step--;
    else if (control.last_middle_hall_sensor == 3) control.middle_motor_step++;
    if (control.last_middle_hall_sensor != 1) control.last_middle_hall_sensor = 1;
}
void IRAM_ATTR middle_motor_hall_interrupt_2() {
    if (control.last_middle_hall_sensor == 1) control.middle_motor_step++;
    else if (control.last_middle_hall_sensor == 3) control.middle_motor_step--;
    control.last_middle_hall_sensor = 2;
}
void IRAM_ATTR middle_motor_hall_interrupt_3() {
    if (control.last_middle_hall_sensor == 1) control.middle_motor_step--;
    else if (control.last_middle_hall_sensor == 2) control.middle_motor_step++;
    control.last_middle_hall_sensor = 3;
}

void IRAM_ATTR left_motor_hall_interrupt_1() {
    if (control.last_left_hall_sensor == 2) control.left_motor_step--;
    else if (control.last_left_hall_sensor == 3) control.left_motor_step++;
    if (control.last_left_hall_sensor != 1) control.last_left_hall_sensor = 1;
}
void IRAM_ATTR left_motor_hall_interrupt_2() {
    if (control.last_left_hall_sensor == 1) control.left_motor_step++;
    else if (control.last_left_hall_sensor == 3) control.left_motor_step--;
    control.last_left_hall_sensor = 2;
}
void IRAM_ATTR left_motor_hall_interrupt_3() {
    if (control.last_left_hall_sensor == 1) control.left_motor_step--;
    else if (control.last_left_hall_sensor == 2) control.left_motor_step++;
    control.last_left_hall_sensor = 3;
}

void IRAM_ATTR right_motor_hall_interrupt_1() {
    if (control.last_right_hall_sensor == 2) control.right_motor_step--;
    else if (control.last_right_hall_sensor == 3) control.right_motor_step++;
    if (control.last_right_hall_sensor != 1) control.last_right_hall_sensor = 1;
}
void IRAM_ATTR right_motor_hall_interrupt_2() {
    if (control.last_right_hall_sensor == 1) control.right_motor_step++;
    else if (control.last_right_hall_sensor == 3) control.right_motor_step--;
    control.last_right_hall_sensor = 2;
}
void IRAM_ATTR right_motor_hall_interrupt_3() {
    if (control.last_right_hall_sensor == 1) control.right_motor_step--;
    else if (control.last_right_hall_sensor == 2) control.right_motor_step++;
    control.last_right_hall_sensor = 3;
}
