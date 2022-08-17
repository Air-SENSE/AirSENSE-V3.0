#include "Config.h"
#include "SD.h"
#include <string>
#include "WiFiMulti.h"


#define WIFI_INFO_FILE  "/WIFI.txt"
#define PREVIOUS_WIFI_FILE "/WIFIPRE.txt"

class Store
{
private:
    // String fileName;
public:
    bool init();
    bool isExist();
    bool isFileEmpty(const char* _file_name);
    bool logData(const char* _file_name, DataCore _data_core, char *_mac, TimeNow _data_time);
    void logTemperatureMessage(String _mes, const char* _file_name);
    void readData(const char* _file_name);
    void readWiFiInfo(WiFiMulti *_wifi_mul);
    void logWiFiInfo(String _ssid, String _pass_wd, const char *_file_name);
    String readLine(const char* _file_name, uint16_t line);
    void end();
    Store(/* args */);
    ~Store();
};

