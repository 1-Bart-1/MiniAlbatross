#include "UART.h"


UART::UART() {
}

void UART::begin(void) {
}

void UART::update(State* state) {
    // uart.send_debug_info_msg(state);
    // send_state_as_json(state);

    this->listen();
    if (this->new_package_received) {
        // Serial.println(this->received_string);
        this->enumerate_commands_from_json(this->received_string, state);
        this->new_package_received = false;
        this->send_state_as_json(state);
        state->last_receive_time = micros();
    }
}

void UART::listen(void) {
    /* Listen on UART for a null terminated string */

    static uint8_t index = 0;
    char end_marker = '\n';
    char rc;

    if (Serial.available() > 0) {
        while ((Serial.available() > 0) && (this->new_package_received == false)) {
            rc = Serial.read();

            if (rc != end_marker) {
                this->received_string[index] = rc;
                index++;
            }
            else {
                this->received_string[index] = '\0';
                index = 0;
                this->new_package_received = true;
                Serial.flush();
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
     bool::e = enable true=1 or false=0
     */

    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, string);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    } 

    if(doc.containsKey("m")) {
        state->motors[0].percentage = abs((float)doc["m"]);
        state->motors[0].reverse = (float)doc["m"] < 0.0;
    }
    if(doc.containsKey("l")) {
        state->motors[1].percentage = abs((float)doc["l"]);
        state->motors[1].reverse = (float)doc["l"] < 0.0;
    }
    if(doc.containsKey("r")) {
        state->motors[2].percentage = abs((float)doc["r"]);
        state->motors[2].reverse = (float)doc["r"] < 0.0;
    }
    if(doc.containsKey("e")) {
        state->enable = (bool)doc["e"];
    } 
}

void UART::send_state_as_json(State* state) {
    /* For testing */

    StaticJsonDocument<256> doc;
    // doc["e"] = state->enable;
    doc["m"] = state->motors[0].percentage;
    doc["l"] = state->motors[1].percentage;
    doc["r"] = state->motors[2].percentage;

    // doc["sm"] = state->motors[0].step;
    // doc["sl"] = state->motors[1].step;
    // doc["sr"] = state->motors[2].step;

    doc["Im"] = state->motors[0].current;
    doc["Il"] = state->motors[1].current;
    doc["Ir"] = state->motors[2].current;

    // doc["om"] = 

    // doc["V"] = state->voltage;
    // doc["t"] = millis() - state->start_time;

    // send the prepared json string over Serial uart
    serializeJson(doc, Serial);
    Serial.println();
}

void UART::send_debug_info_msg(State* state) {
    Serial.print("motor\t");
    Serial.print(state->motors[0].speed);
    Serial.print("\t");
    Serial.print(state->motors[0].percentage);
    Serial.print("\t");
    Serial.print(state->motors[0].reverse);
    Serial.print("\t\t");

    Serial.println();
}

UART uart;
