#include "UART.h"


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
     {"m":1.0}
     {"m":0, "l":0, "r":0, "rm":0, "e":1 }
     float::m = middle motor voltage between 0 and 1
     float::l = left motor voltage between 0 and 1
     float::r = right motor voltage between 0 and 1
     bool::rm = reverse middle
     bool::rl = reverse left
     bool::rr = reverse right
     bool::e = enable true=1 or false=0
     */

    StaticJsonDocument<100> doc;

    DeserializationError error = deserializeJson(doc, string);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    if(doc["m"] != NULL) {
        state->middle_motor.percentage = doc["m"];
    }
    if(doc["l"] != NULL) {
        state->left_motor.percentage = doc["l"];
    }
    if(doc["r"] != NULL) {
        state->right_motor.percentage = doc["r"];
    }
    if(doc["e"] != NULL) {
        state->enable = doc["e"];
    }
    if(doc["rm"] != NULL) {
        state->middle_motor.reverse = doc["rm"];
    }
    if(doc["rl"] != NULL) {
        state->left_motor.reverse = doc["rl"];
    }
    if(doc["rr"] != NULL) {
        state->right_motor.reverse = doc["rr"];
    }
}

void UART::send_state_as_json(State* state) {
    /* Send system data over Serial UART in json format */

    // allocate json buffer space
    StaticJsonDocument<128> doc;

    // fill json buffer with data
    doc["id"] = "Micro";

    JsonArray position = doc.createNestedArray("position");
    position.add(state->kite.position.x);
    position.add(state->kite.position.y);
    position.add(state->kite.position.z);


    // send the prepared json string over Serial uart
    serializeJson(doc, Serial);
    Serial.println();
}

void UART::send_relevant_state_info(State* state) {
    /* For testing */

    StaticJsonDocument<128> doc;
    doc["e"] = state->enable;

    // send the prepared json string over Serial uart
    serializeJson(doc, Serial);
    Serial.println();

}

void UART::send_debug_info_msg(State* state) {
    Serial.print("motor\t");
    Serial.print(state->middle_motor.speed);
    Serial.print("\t");
    Serial.print(state->middle_motor.percentage);
    Serial.print("\t");
    Serial.print(state->middle_motor.reverse);
    Serial.print("\t\t");

    Serial.println();
}

UART uart;
