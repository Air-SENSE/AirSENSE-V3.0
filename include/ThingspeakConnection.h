#include "ThingSpeak.h"

#include "ArduinoJson.h"
#include "Config.h"
#include "WiFiClient.h"

#define KEEP_ALIVE_PERIOD 45
#define THINGSPEAK_CHANNEL 1392854
#define THINGSPEAK_API_WRITE   "3QS968R51B247FA0"

#define THINGSPEAK_BUFFER_SIZE    300

enum{THINGSPEAK_STATE_CHECK_QUEUE_AND_WAIT,
    THINGSPEAK_STATE_CREATE_MESSAGE,
    THINGSPEAK_STATE_PUBLISH_MESSAGE,
    THINGSPEAK_STATE_PUSH_BACK_DATA_QUEUE,
    THINGSPEAK_STATE_INIT
    };

enum{THINGSPEAK_STATUS_CONNECTED, THINGSPEAK_STATUS_DISCONNECTED};

struct THINGSPEAKState
{
    uint8_t current_state;
	uint8_t previous_state;
	uint8_t status;
};
class THINGSPEAKConnection
{
private:
    // WiFiClient esp_wifi;
    // PubSubClient THINGSPEAKClient;
    THINGSPEAKState state;
	String messageData;
	String messageStautus;
public:
    void switchState(uint8_t _next_state);
    void init();
    bool isConnected();
    bool connect();
    bool publish( int data1 = 0, int data2 = 0, int data3 = 0);

    // bool timeToCheckQueue();
    // bool timeToCheckTHINGSPEAKConnection();
    THINGSPEAKConnection(/* args */);
    ~THINGSPEAKConnection();
};


