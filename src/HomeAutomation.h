#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include <DNSServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <string.h>

#include <regex>
#include <sstream>

#include "uuid.h"
class HomeAutomation : public WebServer {
   public:
    HomeAutomation(char *device_name, char *device_type);
    void networkLoop();
    void setup();

   protected:
    static std::string to_string(float f);
    void loopWifiAP();
    void loopWifi();
    void loadConfig();
    void saveConfig();
    String getWifiItems();

    char *device_name;
    char *uuid;
    char *networking_ssid;
    char *networking_password;
    char *device_type;
    long networking_offline_time = millis();
    int8_t networking_reconection_attempts = 0;

    uint8_t server_type;
    DNSServer dnsServer;
    WiFiServer wifiServer;
    std::string wifiHeader;
};