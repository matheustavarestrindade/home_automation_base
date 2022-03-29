#include "HomeAutomation.h"
#define NETWORK_API_PORT 7858
#define FORMAT_DRIVE false
#define HTML_TOP "<!DOCTYPE html><html lang=\"en\"> <head> <meta charset=\"UTF-8\"/> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/> <title>Configure o seu aparelho</title> <style>*{box-sizing: border-box;}body{display: flex; align-items: center; justify-content: center; flex-direction: column; background-color: #101010; color: white; padding: 0; border: 0; margin: 0;}.container{display: flex; align-items: center; justify-content: center; flex-direction: column; width: 80%; height: 100vh; max-width: 80%;}.container.disabled{display: none;}h1{margin-bottom: 1rem;}ul{list-style: none; height: 60vh; overflow-y: auto; background-color: #e1e1e180; padding: 1rem; width: 100%; display: flex; flex-direction: column;}li{border-bottom: 1px solid white; padding: 1rem; cursor: pointer; color: white; font-weight: bold; display: flex; align-items: center; justify-content: space-between; position: relative;}li:hover{background: #4287f5;}li.selected{background: #4287f5;}input{width: 100%; margin-bottom: 1rem; padding: 1rem; border: 0;}button{padding: 1rem; width: 100%; margin: 0; border: 0; background-color: #4287f5; cursor: pointer; color: white; font-weight: bold;}.wave-container{transform: scale(0.6); position: absolute; top: 0; right: 0;}.wave{display: inline-block; border: 2px solid transparent; border-top-color: #eee; border-radius: 50%; border-style: solid; margin: 5px;}.waveStrength-3 .wv4.wave, .waveStrength-2 .wv4.wave, .waveStrength-2 .wv3.wave, .waveStrength-1 .wv4.wave, .waveStrength-1 .wv3.wave, .waveStrength-1 .wv2.wave{border-top-color: #202020;}.spinner-container{display: flex; align-items: center; justify-content: center; height: 100vh;}.spinner-container.disabled{display: none;}.spinner-box{width: 430px; height: 430px; display: flex; justify-content: center; align-items: center;}.circle-border{width: 250px; height: 250px; padding: 7px; display: flex; justify-content: center; align-items: center; border-radius: 50%; background: linear-gradient(0deg, rgb(0 0 0 / 0%) 31%, #062e83 49%, #4566ad 50%); animation: spin 0.9s linear infinite;}.circle-border.success{background: linear-gradient(0deg, rgb(0 0 0 / 0%) 31%, #038564 49%, #45ad64 50%);}.circle-border.error{background: linear-gradient(0deg, rgb(0 0 0 / 0%) 31%, #e42d2d 49%, #ce0000 50%);}.circle-core{width: 100%; height: 100%; background: black; border-radius: 50%;}em{position: absolute; font-size: 22px;}@keyframes spin{0%{transform: rotate(0deg);}100%{transform: rotate(360deg);}}</style> </head> <body> <div class=\"spinner-container disabled\"> <div class=\"spinner-box\"> <div class=\"circle-border\"> <di class=\"circle-core\"></di> </div><em id=\"spinner-text\">Aguarde ...</em> </div></div><div class=\"container\"> <h1>Selecione sua rede</h1> <ul>";
#define HTML_BOTTOM " </ul> <input type=\"text\" hidden id=\"wifi_ssid\" name=\"wifi_ssid\" placeholder=\"Digite a senha de sua rede\"/> <input type=\"text\" id=\"wifi_password\" name=\"wifi_password\" placeholder=\"Digite a senha de sua rede\"/> <button onclick=\"handleConfig()\">Conectar</button> </div></body> <script>function hs(clicked){const li=[...document.querySelectorAll(\"li\")]; const ssid=document.querySelector(\"#wifi_ssid\"); ssid.value=clicked.getAttribute(\"ssid\"); li.forEach((l)=> l.classList.remove(\"selected\")); clicked.classList.add(\"selected\");}async function handleConfig(){const s=document.querySelector(\"#wifi_ssid\"); const p=document.querySelector(\"#wifi_password\"); const container=document.querySelector(\".container\"); const spinner_container=document.querySelector(\".spinner-container\"); const spinner_border=document.querySelector(\".circle-border\"); const spinner_text=document.querySelector(\"#spinner-text\"); container.classList.add(\"disabled\"); spinner_container.classList.remove(\"disabled\"); spinner_border.classList.remove(\"success\", \"error\"); spinner_text.textContent=\"Aguarde...\"; try{const header=new Headers(); header.append(\"ssid\", s.value); header.append(\"password\", p.value); const data=await fetch(\"/configuration\",{method: \"POST\", headers: header,}); console.log(data); const jsonRes=await data.json(); if (jsonRes.error){spinner_border.classList.add(\"error\"); spinner_text.textContent=\"Error de Conexão\"; setTimeout(()=>{location.reload();}, 10000);}else if (jsonRes.success){spinner_border.classList.add(\"success\"); spinner_text.textContent=\"Contactado!\";}console.log(jsonRes);}catch (err){spinner_border.classList.add(\"error\"); spinner_text.textContent=\"Error ao conectar...\"; setTimeout(()=>{location.reload();}, 10000);}}function loadStrn(){const is=[...document.querySelectorAll(\"i\")]; is.map((i)=>{const strength=Number(i.getAttribute(\"value\")); console.log(strength); console.log(i.parentElement); i.parentElement.replaceChild(gl(strength), i);});}function gl(strength){return getWl(strength >=-50 ? 4 : strength >=-70 ? 3 : strength >=-80 ? 2 : 1);}function getWl(len){const e=document.createElement(\"div\"); e.classList.add(\"waveStrength-\" + len, \"wave-container\"); const d1=document.createElement(\"div\"); d1.classList.add(\"wv4\", \"wave\"); const d2=document.createElement(\"div\"); d2.classList.add(\"wv3\", \"wave\"); const d3=document.createElement(\"div\"); d3.classList.add(\"wv2\", \"wave\"); const d4=document.createElement(\"div\"); d4.classList.add(\"wv1\", \"wave\"); e.appendChild(d1); d1.appendChild(d2); d2.appendChild(d3); d3.appendChild(d4); return e;}loadStrn(); </script></html>";
const long online_since = millis();

HomeAutomation::HomeAutomation(char* device_name, char* device_type) : WebServer(NETWORK_API_PORT) {
    this->device_name = device_name;
    this->device_type = device_type;
    this->uuid = nullptr;
    this->networking_ssid = nullptr;
    this->networking_password = nullptr;
    this->networking_reconection_attempts = 0;
};

void HomeAutomation::setup() {
    this->loadConfig();

    if (this->uuid == nullptr) {
        Serial.println("[INFO] UUID Not set, generating a new one.");
        Serial.print("[INFO] Generated UUID:");
        uuid_generate(this->uuid);
        Serial.print(this->uuid);
        this->saveConfig();
    }

    Serial.println("");
    Serial.println("=============DEVICE=============");
    Serial.print("UUID: ");
    Serial.println(this->uuid);
    Serial.print("NAME: ");
    Serial.println(this->device_name);
    Serial.print("TYPE: ");
    Serial.println(this->device_type);
    Serial.println("=============DEVICE=============");
    Serial.println("");

    if (this->networking_ssid == nullptr || this->networking_reconection_attempts > 10) {
        Serial.println("[INFO] No networking configuration found, entering setup mode.");

        const byte DNS_PORT = 53;
        IPAddress apIP(192, 168, 1, 1);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(this->device_name, NULL);
        WiFi.disconnect(true, true);
        this->dnsServer.start(DNS_PORT, "*", apIP);
        this->server_type = 1;
        this->wifiServer.begin();
        this->networking_reconection_attempts = 0;

    } else {
        if (this->networking_password == nullptr || sizeof(this->networking_password) < 3) {
            WiFi.begin(this->networking_ssid);
        } else {
            WiFi.begin(this->networking_ssid, this->networking_password);
        }
        Serial.print("[INFO] Connecting to: ");
        Serial.print(this->networking_ssid);
        uint8_t tryes = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            tryes++;
            Serial.print(".");
            if (tryes > 30) {
                this->networking_reconection_attempts++;
                this->saveConfig();
                ESP.restart();
                return;
            }
        }
        Serial.println("");

        this->networking_reconection_attempts = 0;
        this->saveConfig();

        // Print local IP address and start web server
        Serial.println("[INFO] Wifi connection stablished");
        Serial.print("  -> IP ADDRS: ");
        Serial.println(WiFi.localIP());
        this->server_type = 0;
    }

    this->on("/info", [&]() {
        std::string uuidStr(uuid);
        std::string typeStr(device_type);
        std::string jsonStr;
        jsonStr += "{";
        jsonStr += "\"uuid\":\"" + uuidStr + "\",";
        jsonStr += "\"uptime\":" + to_string((millis() - online_since)) + ",";
        jsonStr += "\"type\":\"" + typeStr + "\"";
        jsonStr += "}";
        char* loadValues = new char[jsonStr.length() + 1];
        strcpy(loadValues, jsonStr.c_str());
        send(200, "text/json", loadValues);
    });

    this->on("/reset_network", [&]() {
        Serial.println("[INFO] Reseting network settings by request!");
        this->networking_ssid = nullptr;
        this->networking_password = nullptr;
        this->networking_reconection_attempts = 0;
        this->saveConfig();
        send(200, "text/json", "{ \"success\": true }");
        ESP.restart();
    });

    if (this->server_type == 0)
        this->begin(NETWORK_API_PORT);
}

void HomeAutomation::loadConfig() {
    if (FORMAT_DRIVE) {
        Serial.println("[INFO] Formating FS...");
        SPIFFS.format();
    }

    if (!SPIFFS.begin(true)) {
        Serial.println("[ERROR] Cannot mount SPIFFS");
        return;
    }

    File configFile = SPIFFS.open("/device_configuration.txt");

    if (!configFile) {
        Serial.println("[ERROR] Cannot open file to load configuration");

        return;
    }
    Serial.println("[INFO] Loaded device configuration file");

    std::vector<String> fileContent;
    while (configFile.available()) {
        fileContent.push_back(configFile.readStringUntil('\n'));
    }
    configFile.close();
    if (fileContent.size() == 4) {
        String file_uuid = fileContent[0];
        String file_networking_ssid = fileContent[1];
        String file_networking_password = fileContent[2];
        String file_networking_reconection_attempts = fileContent[3];

        this->uuid = new char[strlen(file_uuid.c_str()) + 1];
        this->networking_ssid = new char[strlen(file_networking_ssid.c_str()) + 1];
        this->networking_password = new char[strlen(file_networking_password.c_str()) + 1];

        strcpy(this->uuid, file_uuid.c_str());
        strcpy(this->networking_ssid, file_networking_ssid.c_str());
        strcpy(this->networking_password, file_networking_password.c_str());
        this->networking_reconection_attempts = atoi(file_networking_reconection_attempts.c_str());
    }
}

void HomeAutomation::saveConfig() {
    File configFile = SPIFFS.open("/device_configuration.txt", FILE_WRITE);

    if (!configFile) {
        Serial.println("[ERROR] Cannot opening file to load configuration");
        return;
    }

    Serial.println("[INFO] Loaded device configuration file");

    if (this->uuid != nullptr && this->networking_ssid != nullptr) {
        configFile.print(this->uuid);
        configFile.print('\n');
        configFile.print(this->networking_ssid);
        configFile.print('\n');
        if (this->networking_password != nullptr) {
            configFile.print(this->networking_password);
        }
        // Wifi with no password
        configFile.print("\n");
        configFile.print(this->networking_reconection_attempts, 10);
        configFile.print("\n");
        configFile.close();
        Serial.println("[INFO] Device configuration file has been saved");
    }
}

void HomeAutomation::networkLoop() {
    if (this->server_type == 1) {
        this->loopWifiAP();
    } else {
        this->loopWifi();
    }
}

int networks;
long last_scan = millis();

String HomeAutomation::getWifiItems() {
    String responseHTMLWifiItems =
        "<li onclick=\"hs(this)\" class=\"\" ssid=\"\">"
        "<span> Scaning networks...</span>"
        "</li> ";

    if (networks > 0) {
        responseHTMLWifiItems = "";
        for (int i = 0; i < networks; i++) {
            responseHTMLWifiItems += "<li onclick=\"hs(this)\" class=\"\" ssid=\"";
            responseHTMLWifiItems += WiFi.SSID(i);
            responseHTMLWifiItems += "\">";
            responseHTMLWifiItems += "<span>";
            responseHTMLWifiItems += WiFi.SSID(i);
            responseHTMLWifiItems += "</span>";
            responseHTMLWifiItems += "<i value=\"";
            responseHTMLWifiItems += WiFi.RSSI(i);
            responseHTMLWifiItems += "\"></i>";
            responseHTMLWifiItems += "</li>";
        }
    }
    return responseHTMLWifiItems;
}

void HomeAutomation::loopWifiAP() {
    WiFiClient client = this->wifiServer.available();  // listen for incoming clients

    if (last_scan < millis()) {
        Serial.println("[INFO] Starting network scan...");
        last_scan = millis() + 60000;  // every 60 sec
        networks = WiFi.scanNetworks();
        Serial.print("[INFO] Finishing network scan with ");
        Serial.print(networks);
        Serial.println(" Networks found.");
    }

    if (client) {
        String currentLine = "";
        std::string ssid = "";
        std::string password = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                this->wifiHeader += c;
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        if (ssid.length() > 0) {
                            Serial.println(" ");
                            Serial.println("[INFO] Checking network connection");
                            Serial.print("SSID: ");
                            Serial.println(ssid.c_str());
                            Serial.print("PASSWORD: ");
                            Serial.println(password.c_str());
                            Serial.println(" ");

                            this->networking_ssid = new char[ssid.length() + 1];
                            this->networking_password = new char[password.length() + 1];

                            strcpy(this->networking_ssid, ssid.c_str());
                            strcpy(this->networking_password, password.c_str());

                            WiFi.begin(this->networking_ssid, this->networking_password);

                            uint8_t tryes = 0;

                            while (WiFi.status() != WL_CONNECTED && tryes < 20) {
                                delay(500);
                                Serial.print(".");
                                tryes++;
                            }
                            Serial.println("");

                            if (tryes == 10) {
                                Serial.println("[ERRO] Network connection error!");
                                std::string response = "{\"error\": \"invalid credentials\"}";
                                client.printf("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %u\r\n\r\n%s", response.length(), response.c_str());
                                break;
                            }
                            Serial.println("[INFO] Successfully connected to network!");
                            std::string response = "{\"success\": \"Valid network connected\"}";
                            client.printf("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %u\r\n\r\n%s", response.length(), response.c_str());

                            this->saveConfig();
                            delay(10000);
                            ESP.restart();
                            break;
                        }

                        String response = HTML_TOP;
                        response += this->getWifiItems();
                        response += HTML_BOTTOM;

                        client.printf("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s", response.length(), response.c_str());

                        break;
                    } else {
                        if (currentLine.length() < 4) {
                            currentLine = "";
                            continue;
                        }
                        if (currentLine.c_str()[0] == 's' && currentLine.c_str()[1] == 's' && currentLine.c_str()[2] == 'i' && currentLine.c_str()[3] == 'd') {
                            int i;
                            ssid = "";
                            for (i = 6; i < currentLine.length(); i++) {
                                ssid += currentLine.c_str()[i];
                            }

                        } else if (currentLine.c_str()[0] == 'p' && currentLine.c_str()[1] == 'a' && currentLine.c_str()[2] == 's' && currentLine.c_str()[3] == 's') {
                            int i;
                            password = "";
                            for (i = 10; i < currentLine.length(); i++) {
                                password += currentLine.c_str()[i];
                            }
                        }
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        this->wifiHeader = "";
        client.stop();
        Serial.println("[INFO] Client disconnected.");
    }
    this->dnsServer.processNextRequest();
}

void HomeAutomation::loopWifi() {
    if (WiFi.status() != WL_CONNECTED && (this->networking_offline_time + 10000L) > millis()) {
        ESP.restart();
        return;
    }
    this->networking_offline_time = millis();
    this->handleClient();
}

std::string HomeAutomation::to_string(float f) {
    std::stringstream ss;
    ss << f;
    std::string finalString = ss.str();
    return finalString;
}
