#include "UART.h"

UART::UART() {
    Serial.setRxBufferSize(BUFFER_SIZE);
    Serial.setTxBufferSize(BUFFER_SIZE);
    Serial.begin(115200);
}

bool UART::receive(State* state) {
    /* Listen on UART for a null terminated string */

    static uint8_t index = 0;
    char end_marker = '\n';
    char rc;
    while (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != end_marker) {
            this->received_string[index] = rc;
            index++;
        }
        else {
            this->received_string[index] = '\0';
            index = 0;
            state->last_receive_time = micros();
            this->enumerate_commands_from_json(this->received_string, state);
            return true;
            // Serial.flush();
        }
    }
    return false;
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

    StaticJsonDocument<BUFFER_SIZE> doc;

    DeserializationError error = deserializeJson(doc, string);

    if (error) {
        return;
    } 
    if(doc.containsKey("md")) {
        state->mode = (bool)doc["md"];
    }
    if(doc.containsKey("m")) {
        if(state->mode == 0) {
            state->motors[0].percentage = (float)doc["m"];
        } else {
            state->motors[0].set_speed = (float)doc["m"];
        }
    }
    if(doc.containsKey("l")) {
        if(state->mode == 0) {
            state->motors[1].percentage = (float)doc["l"];
        } else {
            state->motors[1].set_speed = (float)doc["l"];
        }
    }
    if(doc.containsKey("r")) {
        if(state->mode == 0) {
            state->motors[2].percentage = (float)doc["r"];
        } else {
            state->motors[2].set_speed = (float)doc["r"];
        }
    }
    if(doc.containsKey("e")) {
        state->enable = (bool)doc["e"];
    } 
}

void UART::send_state_as_json(State* state) {
    /* For testing */

    StaticJsonDocument<BUFFER_SIZE> doc;

    // doc["e"] = state->enable;
    doc["m"] = state->motors[0].percentage;
    // doc["l"] = state->motors[1].percentage;
    // doc["r"] = state->motors[2].percentage;

    // doc["Im"] = state->motors[0].current;
    // doc["Il"] = state->motors[1].current;
    // doc["Ir"] = state->motors[2].current;

    // doc["Kp"] = state->Kp;
    // doc["Ki"] = state->Ki;
    // doc["Kd"] = state->Kd;

    // doc["sm"] = state->motors[0].step;
    // doc["sl"] = state->motors[1].step;
    // doc["sr"] = state->motors[2].step;

    doc["vm"] = state->motors[0].speed;
    doc["vl"] = state->motors[1].speed;
    doc["vr"] = state->motors[2].speed;

    // doc["am"] = state->motors[0].acc;

    doc["fm"] = state->motors[0].force;
    doc["fl"] = state->motors[1].force;
    doc["fr"] = state->motors[2].force;

    // doc["vsm"] = state->motors[0].set_speed;
    // doc["vsl"] = state->motors[1].set_speed;
    // doc["vsr"] = state->motors[2].set_speed;

    doc["qkw"] = state->kite.q_kite.w;
    doc["qkx"] = state->kite.q_kite.x;
    doc["qky"] = state->kite.q_kite.y;
    doc["qkz"] = state->kite.q_kite.z;
    // doc["qtw"] = state->kite.q_tether.w;
    // doc["qtx"] = state->kite.q_tether.x;
    // doc["qty"] = state->kite.q_tether.y;
    // doc["qtz"] = state->kite.q_tether.z;

    // doc["om"] = 

    // doc["V"] = state->voltage;
    doc["t"] = (float)micros()/1000000.0; // send time in seconds

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
