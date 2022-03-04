#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>

#include "uuid.h"
#define haNETWORK_PORT 7858
WebServer server(haNETWORK_PORT);
Preferences preferences;

class HomeAutomation {
   public:
    HomeAutomation(char *device_name);
    void networkLoop();
    void registerNetworkingRoute(String uri, WebServer::THandlerFunction handler);

   protected:
    static void handleDeveceInformation();
    void loopWifiAP();
    void loopWifi();

    char *device_name;
    char *uuid;
    char *networking_ssid;
    char *networking_password;
    long networking_offline_time = millis();
    int8_t networking_reconection_attempts = 0;

    long online_since = millis();
    uint8_t server_type;
};