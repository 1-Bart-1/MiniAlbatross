#include "UART.h"
#include "Control.h"

UART::UART() {
}

void UART::begin(void) {
}

void UART::update(void) {
    uart.send_debug_info_msg(&state);
    // uart.send_state_as_json(&state);

    this->listen();
    if (this->new_package_received) {
        this->enumerate_commands_from_json(this->received_string, &state);
        this->send_pid_params(&state);
        this->new_package_received = false;
    }
}

void UART::listen(void) {
    /* Listen on UART for a null terminated string */

    static uint8_t index = 0;
    char end_marker = '\n';
    char rc;

    if (Serial.available() > 0) {
        while ((Serial.available() > 0) && (uart.new_package_received == false)) {
            rc = Serial.read();

            if (rc != end_marker) {
                uart.received_string[index] = rc;
                index++;
            }
            else {
                uart.received_string[index] = '\0';
                index = 0;
                uart.new_package_received = true;
            }
        }
    }
}

void UART::enumerate_commands_from_json(char* string, State* state) {
    /* Try to parse 'string' into json format and execute commands
     * on the basis of the received data.
     * 
     * json example message:
     {"Kp":100}
     {"Kp":0,"Ki":0,"Kd":0, "enable":true}
     {"Kp":1,"Ki":0,"Kd":0, "enable":true}
     {"Kp":2,"Ki":0,"Kd":0, "enable":true}
     {"Kp":3,"Ki":0,"Kd":0, "enable":true}
     {"Kp":4,"Ki":0,"Kd":0, "enable":true}
     {"Kp":5,"Ki":0,"Kd":0, "enable":true}
     */

    StaticJsonDocument<100> doc;

    DeserializationError error = deserializeJson(doc, string);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        switch(string[0]){
            case 'q':
                state->pid_params.Kp += 1;
                break;
            case 'a':  
                state->pid_params.Kp -= 1;
                break;
            case 'w':
                state->pid_params.Ki += 0.1;
                break;
            case 's':
                state->pid_params.Ki -= 0.1;
                break;
            case 'e':
                state->pid_params.Kd += 0.1;
                break;
            case 'd':  
                state->pid_params.Kd -= 0.1;
                break;
            case 'o': // move out
                state->motor.constant_speed = (state->motor.constant_speed != 100) ? 100 : 0;
                break;
            case 'i': // move in
                state->motor.constant_speed = (state->motor.constant_speed != -100) ? -100 : 0;
                break;
            case 'm':
                state->motor.enable = !state->motor.enable;
                break;
        }
        return;
    }

    if(doc["Kp"] != NULL) {
        state->pid_params.Kp = doc["Kp"];
    }
    if(doc["Ki"] != NULL) {
        state->pid_params.Ki = doc["Ki"];
    }
    if(doc["Kd"] != NULL) {
        state->pid_params.Kd = doc["Kd"];
    }
    if(doc["enable"] != NULL) {
        state->motor.enable = doc["enable"];
    }
    if(doc["constant_speed"] != NULL) {
        state->motor.constant_speed = doc["constant_speed"];
    }
    if(doc["constant_speed"] != NULL) {
        state->motor.constant_speed = doc["constant_speed"];
    }
}

void UART::send_state_as_json(State* state) {
    /* Send system data over Serial UART in json format */

    // allocate json buffer space
    StaticJsonDocument<128> doc;

    // fill json buffer with data
    doc["id"] = "Boat";

    JsonArray position = doc.createNestedArray("position");
    position.add(state->kite.position.x);
    position.add(state->kite.position.y);
    position.add(state->kite.position.z);


    // send the prepared json string over Serial uart
    serializeJson(doc, Serial);
    Serial.println();
}

void UART::send_pid_params(State* state) {
    /* For testing */

    StaticJsonDocument<128> doc;
    doc["Kp"] = state->pid_params.Kp;
    doc["Ki"] = state->pid_params.Ki;
    doc["Kd"] = state->pid_params.Kd;
    doc["enable"] = state->motor.enable;

    // send the prepared json string over Serial uart
    serializeJson(doc, Serial);
    Serial.println();

}

void UART::send_debug_info_msg(State* state) {
    Serial.print("speed\t");
    Serial.print(state->motor.left_speed);
    Serial.print("\t");
    Serial.print(state->kite.setpoint);
    Serial.print("\t");
    Serial.print(state->kite.angle);
    Serial.print("\t\t");

    Serial.print("pid\t");
    Serial.print(state->pid_params.Kp);
    Serial.print("\t");
    Serial.print(state->pid_params.Ki);
    Serial.print("\t");
    Serial.print(state->pid_params.Kd);
    Serial.print("\t\t");

    Serial.print("angle\t");
    Serial.print(state->kite.angle);
    Serial.print("\t\t");

    Serial.println();
}

UART uart;
