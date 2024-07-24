#include <Control.h>

Control::Control() {

}

void Control::begin(State* state) {
    timer = timerBegin(0, 80, true);           	// timer 0, prescalar: 80, UP counting
    timerAttachInterrupt(timer, &pid_interrupt, true); 	// Attach interrupt
    timerAlarmWrite(timer, sample_time_us, true);  		// Match value= 1000000 for 1 sec. delay.
    timerAlarmEnable(timer);           			// Enable Timer with interrupt (Alarm Enable)
    for(int i=0; i<=2; i++){
        cruise_controls[i] = QuickPID(&state->motors[i].filtered_speed, &state->motors[i].percentage, &state->motors[i].set_speed);
        cruise_controls[i].SetTunings(state->Kp, state->Ki, state->Kd);
        cruise_controls[i].SetMode(1);
        cruise_controls[i].SetOutputLimits(-1.0, 1.0);
        cruise_controls[i].SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);
        cruise_controls[i].SetSampleTimeUs(sample_time_us);
    }

    // pinMode(VOLTAGE_PIN, INPUT);

    for(int i=0; i<=2; i++) {
        Serial.println(i);
        ledcSetup(pwm_channels[i], PWM_FREQ, PWM_RESOLUTION);
        ledcAttachPin(motor_pins[i], pwm_channels[i]);
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

    state->motors[0].step = middle_motor_step;
    state->motors[1].step = left_motor_step;
    state->motors[2].step = right_motor_step;

    if (compute_now == true) {
        compute_now = false;
        if (state->mode == 1) {
            for(uint8_t j=0; j<=2; j++){
                if (cruise_controls[j].GetKp() != state->Kp || cruise_controls[j].GetKi() != state->Ki || cruise_controls[j].GetKd() != state->Kd) {
                    cruise_controls[j].SetTunings(state->Kp, state->Ki, state->Kd);
                }
                cruise_controls[j].Compute();
            }
        }
    }
    
    // state->voltage = analogRead(VOLTAGE_PIN);
    measure = analogRead(current_pins[i]);
    if (state->enable) {
        // write 0 if mode==1, -0.05 < set_speed < 0.05 and speed == 0.0
        // write 0 if mode==0 and -0.05 < percentage < 0.05
        if (-0.05 < state->motors[i].percentage && state->motors[i].percentage < 0.05 && -0.05 < state->motors[i].filtered_speed && state->motors[i].filtered_speed < 0.05) {
            ledcWrite(pwm_channels[i], 0);
        } else {
            ledcWrite(pwm_channels[i], (int)((abs(state->motors[i].percentage)*0.55+0.45)*MAX_DUTY_CYCLE));
        }
        if (state->motors[i].percentage < 0.0) digitalWrite(reverse_pins[i], HIGH);
        else digitalWrite(reverse_pins[i], LOW);
    } else {
        ledcWrite(pwm_channels[i], 0);
        current_offsets[i] = current_offsets[i]*0.999 + measure*0.001;
    }

    current = -(measure - current_offsets[i]) * 0.014541622760800843;
    state->motors[i].current = current * 0.05 + state->motors[i].current * 0.95;

    analogRead(current_pins[i==2?0:i+1]); // read next pin to setup analogread
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
}

Control control;

void IRAM_ATTR middle_motor_hall_interrupt_1() {
    if (micros() - control.last_interrupts[0][0] > 6666) {
        if (control.last_middle_hall_sensor == 2) control.middle_motor_step--;
        else if (control.last_middle_hall_sensor == 3) control.middle_motor_step++;
        if (control.last_middle_hall_sensor != 1) control.last_middle_hall_sensor = 1;
    }
    control.last_interrupts[0][0] = micros();
}
void IRAM_ATTR middle_motor_hall_interrupt_2() {
    if (micros() - control.last_interrupts[0][1] > 6666) {
        if (control.last_middle_hall_sensor == 1) control.middle_motor_step++;
        else if (control.last_middle_hall_sensor == 3) control.middle_motor_step--;
        control.last_middle_hall_sensor = 2;
    }
    control.last_interrupts[0][1] = micros();
}
void IRAM_ATTR middle_motor_hall_interrupt_3() {
    if (micros() - control.last_interrupts[0][2] > 6666) {
        if (control.last_middle_hall_sensor == 1) control.middle_motor_step--;
        else if (control.last_middle_hall_sensor == 2) control.middle_motor_step++;
        control.last_middle_hall_sensor = 3;
    }
    control.last_interrupts[0][2] = micros();
}

void IRAM_ATTR left_motor_hall_interrupt_1() {
    if (micros() - control.last_interrupts[1][0] > 6666) {
        if (control.last_left_hall_sensor == 2) control.left_motor_step--;
        else if (control.last_left_hall_sensor == 3) control.left_motor_step++;
        if (control.last_left_hall_sensor != 1) control.last_left_hall_sensor = 1;
    }
    control.last_interrupts[1][0] = micros();
}
void IRAM_ATTR left_motor_hall_interrupt_2() {
    if (micros() - control.last_interrupts[1][1] > 6666) {
        if (control.last_left_hall_sensor == 1) control.left_motor_step++;
        else if (control.last_left_hall_sensor == 3) control.left_motor_step--;
        control.last_left_hall_sensor = 2;
    }
    control.last_interrupts[1][1] = micros();
}
void IRAM_ATTR left_motor_hall_interrupt_3() {
    if (micros() - control.last_interrupts[1][2] > 6666) {
        if (control.last_left_hall_sensor == 1) control.left_motor_step--;
        else if (control.last_left_hall_sensor == 2) control.left_motor_step++;
        control.last_left_hall_sensor = 3;
    }
    control.last_interrupts[1][2] = micros();
}

void IRAM_ATTR right_motor_hall_interrupt_1() {
    if (micros() - control.last_interrupts[2][0] > 6666) {
        if (control.last_right_hall_sensor == 2) control.right_motor_step--;
        else if (control.last_right_hall_sensor == 3) control.right_motor_step++;
        if (control.last_right_hall_sensor != 1) control.last_right_hall_sensor = 1;
    }
    control.last_interrupts[2][0] = micros();
}
void IRAM_ATTR right_motor_hall_interrupt_2() {
    if (micros() - control.last_interrupts[2][1] > 6666) {
        if (control.last_right_hall_sensor == 1) control.right_motor_step++;
        else if (control.last_right_hall_sensor == 3) control.right_motor_step--;
        control.last_right_hall_sensor = 2;
    }
    control.last_interrupts[2][1] = micros();
}
void IRAM_ATTR right_motor_hall_interrupt_3() {
    if (micros() - control.last_interrupts[2][2] > 6666) {
    if (control.last_right_hall_sensor == 1) control.right_motor_step--;
        else if (control.last_right_hall_sensor == 2) control.right_motor_step++;
        control.last_right_hall_sensor = 3;
    }
    control.last_interrupts[2][2] = micros();
}

void IRAM_ATTR pid_interrupt() {
    control.compute_now = true;
}