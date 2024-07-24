#ifndef _UART_H
#define _UART_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Control.h"
#include "State.h"

#define BUFFER_SIZE 512

class UART
{
public:
    UART();
    bool new_package_received;
    char received_string[100];

    bool receive(State* state); // returns true when new message is received.
    void send_state_as_json(State* state);
    
private:
    void send_debug_info_msg(State* state);
    void send_relevant_state_info(State* state);
    void enumerate_commands_from_json(char* string, State* state);

};

extern UART uart;

#endif
