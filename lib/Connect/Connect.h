#ifndef Connect_h
#define Connect_h

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <helper_3dmath.h>
#include <State.h>

#define CHANNEL 0
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

class Connect {
    public:
        Connect();
        void begin(State* state);
        // VectorFloat position;
        State* state;
    private:
        esp_now_peer_info_t slave;
        void initESPNow();
        void deletePeer();
        bool manageSlave();
        void initBroadcastSlave();
};

extern Connect connect;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);     

#endif