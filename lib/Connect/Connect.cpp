#include "Connect.h"
#include <Arduino.h>


Connect::Connect(){

}

void Connect::begin(State* state){
	this->state = state;

    WiFi.mode(WIFI_STA);
	Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
	// Init ESPNow with a fallback logic
	initESPNow();
	esp_now_register_recv_cb(OnDataRecv);

	// add a broadcast peer
	initBroadcastSlave();
}

// Init ESP Now with fallback
void Connect::initESPNow() {
	if (esp_now_init() == ESP_OK) {
		Serial.println("ESPNow Init Success");
	}
	else {
		Serial.println("ESPNow Init Failed");
		ESP.restart();
	}
}


void Connect::deletePeer() {
	const esp_now_peer_info_t *peer = &slave;
	const uint8_t *peer_addr = slave.peer_addr;
	esp_err_t delStatus = esp_now_del_peer(peer_addr);
	Serial.print("Slave Delete Status: ");
	if (delStatus == ESP_OK) {
		// Delete success
		Serial.println("Success");
	}
	else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
		// How did we get so far!!
		Serial.println("ESPNOW Not Init");
	}
	else if (delStatus == ESP_ERR_ESPNOW_ARG) {
		Serial.println("Invalid Argument");
	}
	else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
		Serial.println("Peer not found.");
	}
	else {
		Serial.println("Not sure what happened");
	}
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool Connect::manageSlave() {
	if (slave.channel == CHANNEL) {
		if (DELETEBEFOREPAIR) {
			deletePeer();
		}

		Serial.print("Slave Status: ");
		const esp_now_peer_info_t *peer = &slave;
		const uint8_t *peer_addr = slave.peer_addr;
		// check if the peer exists
		bool exists = esp_now_is_peer_exist(peer_addr);
		if (exists) {
			// Slave already paired.
			Serial.println("Already Paired");
			return true;
		}
		else {
			// Slave not paired, attempt pair
			esp_err_t addStatus = esp_now_add_peer(peer);
			if (addStatus == ESP_OK) {
				// Pair success
				Serial.println("Pair success");
				return true;
			}
			else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
				// How did we get so far!!
				Serial.println("ESPNOW Not Init");
				return false;
			}
			else if (addStatus == ESP_ERR_ESPNOW_ARG) {
				Serial.println("Invalid Argument");
				return false;
			}
			else if (addStatus == ESP_ERR_ESPNOW_FULL) {
				Serial.println("Peer list full");
				return false;
			}
			else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
				Serial.println("Out of memory");
				return false;
			}
			else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
				Serial.println("Peer Exists");
				return true;
			}
			else {
				Serial.println("Not sure what happened");
				return false;
			}
		}
	}
	else {
		// No slave found to process
		Serial.println("No Slave found to process");
		return false;
	}
}


void Connect::initBroadcastSlave() {
	// clear slave data
	memset(&slave, 0, sizeof(slave));
	for (int ii = 0; ii < 6; ++ii) {
		slave.peer_addr[ii] = (uint8_t)0xff;
	}
	slave.channel = CHANNEL; // pick a channel
	slave.encrypt = 0; // no encryption
	manageSlave();
}

void Connect::sendPing() {
	const uint8_t *peer_addr = slave.peer_addr;
	uint8_t one = 1;
    esp_err_t result = esp_now_send(peer_addr, &one, 1);
}

Connect connect;


// callback when data is recv from Master
void IRAM_ATTR OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
	if (sizeof(data) == sizeof(connect.state->kite.q_kite)) {
		memcpy(&connect.state->kite.q_kite, data, sizeof(connect.state->kite.q_kite));
	}
	connect.received = true;
}