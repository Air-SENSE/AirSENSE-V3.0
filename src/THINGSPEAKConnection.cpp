#include "THINGSPEAKConnection.h"
#include "WiFi.h"

#define _DB_LOG_

static unsigned long last_check_queue = 0;
static unsigned long last_check_THINGSPEAK_connection = 0;

WiFiClient speak_wifi;


THINGSPEAKConnection::THINGSPEAKConnection()
{

}
THINGSPEAKConnection::~THINGSPEAKConnection()
{
    
}
void THINGSPEAKConnection::init()
{
   ThingSpeak.begin(speak_wifi);
}

bool THINGSPEAKConnection::isConnected()
{
    return ThingSpeak.begin(speak_wifi);
}

bool THINGSPEAKConnection::connect()
{
    bool ret = ThingSpeak.begin(speak_wifi);
    if(ret == true)
    {
        this->state.status = THINGSPEAK_STATUS_CONNECTED;
    }
    else
    {
        this->state.status = THINGSPEAK_STATUS_DISCONNECTED;
    }
    return ret;
}


bool THINGSPEAKConnection::publish( int data1, int data2, int data3)
{
    if(data1 > 0)
    {
        return ( ThingSpeak.writeField(THINGSPEAK_CHANNEL, 1, data1, THINGSPEAK_API_WRITE)
                && ThingSpeak.writeField(THINGSPEAK_CHANNEL, 2, data2, THINGSPEAK_API_WRITE)
                && ThingSpeak.writeField(THINGSPEAK_CHANNEL, 3, data3, THINGSPEAK_API_WRITE))
        ;
    }
    else
    {
        return false;
    }
    
}
