
#include "MQTT.h"
#include "ArduinoJson.h"
#include "Config.h"
#include "WiFiClient.h"

#define KEEP_ALIVE_PERIOD 45
#define MQTT_BROKER "mqtt.airsense.vn"
#define MQTT_PORT   (1884)
#define MQTT_USER   "sparc"
#define MQTT_PASSWORD   "sparcXZAairsenseATU"

#define MQTT_BUFFER_SIZE    300

enum{MQTT_STATE_CHECK_QUEUE_AND_WAIT,
    MQTT_STATE_CREATE_MESSAGE,
    MQTT_STATE_PUBLISH_MESSAGE,
    MQTT_STATE_PUSH_BACK_DATA_QUEUE,
    MQTT_STATE_INIT
    };
enum{MQTT_STATUS_CONNECTED, MQTT_STATUS_DISCONNECTED};
struct MQTTInfo
{
    String broker;
    uint16_t port;
    // String authenString;
	String mqttTopicPub;
	String mqttTopicSub;
	String mqttUser;	
	String mqttPWD;
	String mqtt_ID;
};
struct MQTTState
{
    uint8_t current_state;
	uint8_t previous_state;
	uint8_t status;
};
class MQTTConnection
{
private:
    // WiFiClient esp_wifi;
    // PubSubClient mqttClient;
    MQTTInfo info;
    MQTTState state;
	String messageData;
	String messageStautus;
public:
    void switchState(uint8_t _next_state);
    void init();
    bool isConnected();
    bool connect();
    void reconnect();
    void disconnect();
    void subcribe(const char *topic);
    void unSubcribe(const char *topic);
    bool publish(bool _retained, String _mes = "", String _topic = "");
    void keepConnection();
    void setTopic(const char *topic);
    void setServer(String _server);
    // void MQTTCallBack(char* _topic, byte* _mes, unsigned int _len);
    void encryptDataToJSON(DataCore _data_core, char* _mac);
    void encryptStatusToJSon(uint32_t _unix_time, bool _sd = false, bool _pms7003 = false, bool _bme280 = false, bool _ds3231 = false, bool _sds011 = false, bool _htu21d = false);
    void decryptJSON(DataCore* _data_core, String _payload);
    void setStatusMessage(String _mes);
    // void setDataMessage(String _mes);
    MQTTState getState();
    MQTTInfo *getInfo();
    void publishFailed();

    // bool timeToCheckQueue();
    // bool timeToCheckMqttConnection();
    void mqttCheckAndWait(uint8_t _is_internet, uint8_t _is_queue);
    MQTTConnection(/* args */);
    ~MQTTConnection();
};


