/**
 * @file DeveceManager.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */

#ifndef _WiFi_CONNECTION_MANAGER_H_
#define _WiFi_CONNECTION_MANAGER_H_

#include <WiFi.h>
#include <FS.h>
#include <WiFiClient.h>
// #include <WiFiServer.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
// #include <AsyncUDP.h>
#include "time.h"
// #include <string>
#include "Config.h"
// #include "DeviceManager.h"

#define WiFi_RSSI_THRESHOLD     10         // Difference in dB between current network and scanned network
#define GMT_OFFSET_SECOND		25200
#define DAY_LIGHR_OFFSET_SECOND	3600
#define TIME_TO_RECONNECT_WIFI_DEFAULT  (60000) //one minute
#define TIME_TO_RECONNECT_WIFI_MAX      (1920000) //32 minutess
#define NTP_SERVER	"pool.ntp.org"

enum {WIFI_STATE_CONNECT_OLD_CONFIG,
	WIFI_STATE_STORE_WIFI_INFO,
	WIFI_STATE_GET_WIFI_INFO_FROM_SD,
	WIFI_STATE_CONNECT_BEST_WIFI, 	
	WIFI_STATE_SMARTCONFIG,
	WIFI_STATE_RECONNECT,
	WIFI_STATE_CHECK_AND_WAIT,
	WIFI_STATE_INIT
	};

enum {WIFI_STATUS_NO_WIFI_CONNECTED, 
	WIFI_STATUS_WIFI_CONNECTED_BUT_NO_INTERNET, 
	WIFI_STATUS_INTERNET_CONNECTED};

struct WiFiConnectionState
{
	uint8_t current_state;
	uint8_t previous_state;
	uint8_t status;
	uint8_t entry;
};
class WiFiConnectionManager {
private:
	WiFiConnectionState state;
public:
	WiFiMulti wifi_mul;
	void init();
	void switchState(uint8_t _next_state);
	void setMode(wifi_mode_t _mode);
	void connect(uint32_t connectTimeout = 5000);
	void reconnect();
	bool isConnected();
	bool isIntenetConnected();
	void removeCurrentWiFi();
	void smartConfig();
	bool isSmartConfigDone();
	void reconnectSequence();
	void startAP();

	void connectBestWiFi(uint32_t connectTimeout = 0);
	void getWiFiListFromStore();
	void logCurrentWiFiInfoToStore();
	uint32_t updateUnixTime();
	void waitAndCheckEvent();

	// bool isInternetConnected();
	bool isOTAupdateAvailable();
	void updateOTA();
	WiFiMulti *getWiFiMul();
	WiFiConnectionState getState();
	WiFiConnectionManager();
	~WiFiConnectionManager();
};


#endif