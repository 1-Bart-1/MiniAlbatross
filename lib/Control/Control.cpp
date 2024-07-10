#include <Control.h>

Control::Control() {
    

}

void Control::begin(State* state) {
    this->state = state;

    kitePID.begin(&this->state->kite.angle,
        &this->state->motor.pid_speed, 
        &this->state->kite.setpoint, 
        &this->state->pid_params.Kp,
        &this->state->pid_params.Ki,
        &this->state->pid_params.Kd,
        P_ON_E, DIRECT);
    kitePID.SetMode(AUTOMATIC);
    kitePID.SetOutputLimits(-100000, 100000);

    engine.init();

    left = engine.stepperConnectToPin(18);
    left->setAutoEnable(true);
    left->setDirectionPin(19);
    left->setAcceleration(1000);

    right = engine.stepperConnectToPin(2);
    right->setAutoEnable(true);
    right->setDirectionPin(4);
    right->setAcceleration(1000);
}


void Control::update(){
    kitePID.SetTunings(this->state->pid_params.Kp, this->state->pid_params.Ki, this->state->pid_params.Kd);
    kitePID.Compute();

    left->setSpeedInHz(abs(this->state->motor.left_speed));
    right->setSpeedInHz(abs(this->state->motor.right_speed));
    
    if(this->state->motor.left_speed > 0){
        left->runForward();
    } else if(this->state->motor.left_speed < 0){
        left->runBackward();
    } else {
        left->stopMove();
    }
    
    if(this->state->motor.right_speed > 0){
        right->runForward();
    } else if(this->state->motor.right_speed < 0){
        right->runBackward();
    } else {
        right->stopMove();
    }
}

Control control;
