#include "HomeAutomation.h"

HomeAutomation::HomeAutomation(char* device_name) {
    this->device_name = device_name;
    preferences.begin("home-auto", false);  // Start preferences with Read/Write
    strcpy(this->uuid, preferences.getString("uuid", "").c_str());
    strcpy(this->networking_ssid, preferences.getString("ssid", "").c_str());
    strcpy(this->networking_password, preferences.getString("password", "").c_str());
    this->networking_reconection_attempts = preferences.getChar("rec_attmp", 0);

    if (strcmp(this->uuid, "") == 0) {
        uuid_generate(this->uuid);
        preferences.putString("uuid", this->uuid);
    }

    if (strcmp(this->networking_ssid, "") == 0 || this->networking_reconection_attempts > 20) {
        WiFi.softAP(this->device_name, "123456");
        this->server_type = 1;
        server.begin();
    } else {
        WiFi.begin(this->networking_ssid, this->networking_password);
        Serial.print("Connecting to: ");
        Serial.print(this->networking_ssid);
        uint8_t tryes = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            tryes++;
            Serial.print(".");
            if (tryes > 20) {
                this->networking_reconection_attempts++;
                preferences.putChar("rec_attmp", this->networking_reconection_attempts);
                ESP.restart();
                return;
            }
        }

        this->networking_reconection_attempts = 0;
        preferences.putChar("rec_attmp", 0);

        // Print local IP address and start web server
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        server.begin();

        this->server_type = 0;
    }

    server.on("/info", HomeAutomation::handleDeveceInformation);
};

void HomeAutomation::networkLoop() {
    if (this->server_type == 1) {
        this->loopWifiAP();
    } else {
        this->loopWifi();
    }
}

void HomeAutomation::loopWifiAP() {
    server.handleClient();
}
void HomeAutomation::loopWifi() {
    if (WiFi.status() != WL_CONNECTED && (this->networking_offline_time + 10000L) > millis()) {
        ESP.restart();
        return;
    }
    this->networking_offline_time = millis();
    server.handleClient();
}

void HomeAutomation::handleDeveceInformation() {
    server.send(200, "text/json", "{\"success\": true}");
}

void HomeAutomation::registerNetworkingRoute(String uri, WebServer::THandlerFunction handler) {
    Serial.print("Registering route: ");
    Serial.print(uri);
    Serial.println("");
    server.on(uri, handler);
}