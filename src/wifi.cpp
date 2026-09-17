
#include "main.h"
#include <atomic>
#include <esp_wifi.h>


BaseType_t startWifiTask() {
	return xTaskCreate(
		taskWifi,
		"WiFi task",
		30000,
		nullptr,
		10,
		nullptr
	);
}

void taskWifi(void* pvParameters) {
	log_i("Setting up WiFi...");
	WiFi.setHostname(HOSTNAME);
	WiFi.softAPsetHostname(HOSTNAME);
	WiFi.STA.setMinSecurity(WIFI_AUTH_OPEN);
	WiFi.STA.setAutoReconnect(false);

	log_i("Trying to connect to last known WiFi network...");
	WiFi.STA.begin(true);

	/*WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
		log_i("WiFi event: %d, status: %d", event, WiFi.status());
	});*/

	// xSemaphoreGive(wifiSemaphore);

	// if(!config.isSetup()) {
	// 	log_i("Bridge is not setup yet. Creating default AP...");
	// 	createSetupAP();

	// 	//wait a bit until HTTP server is configured
	// 	delay(6000);

	// 	//start a scan here so that /api/wifi/status can return rssi and encryption
	// 	WiFi.scanNetworks(true, true, true);
	// 	vTaskDelete(nullptr);
	// 	return;
	// }

	int64_t lastCheck = 0;
	while(true) {
		if(!WiFi.STA.connected()) {
			int64_t now = esp_timer_get_time() / 1000;
			if(now - lastCheck > 5000) {
				log_w("WiFi not connected. Attempting reconnection...");
				WiFi.STA.reconnect();
				lastCheck = now;
			}
		}
		delay(10);
	}

	// WiFiConfig wifi;
	// if(loadWifiConfig(wifi)) {

	// 	lcd.clear();
	// 	lcd.home();
	// 	lcd.print("WiFi connecting ");
	// 	lcd.setCursor(0, 1);

	// 	if(!connectWiFi(wifi, true)) {
	// 		log_e("Failed to connect to saved WiFi network.");
	// 		WiFi.disconnect(true);
	// 		lcd.clear();
	// 		lcd.home();
	// 		lcd.print("   Connection   ");
	// 		lcd.setCursor(0, 1);
	// 		lcd.print("   failed.      ");

	// 		vTaskDelay(1000 / portTICK_PERIOD_MS);
	// 		WiFiConfig* params = new WiFiConfig();
	// 		memcpy(params, &wifi, sizeof(wifi));
	// 		xTaskCreate(
	// 			taskReconnectWifi,
	// 			"Reconnect WiFi",
	// 			3000,
	// 			params,
	// 			5,
	// 			nullptr
	// 		);
	// 	}
	// 	else {
	// 		log_i("WiFi connected");
	// 		log_i("IP address: %s", WiFi.localIP().toString().c_str());
	// 		lcd.home();
	// 		lcd.print("   Connection   ");
	// 		lcd.setCursor(0, 1);
	// 		lcd.print("   successful.  ");
	// 	}
	// }
	// else {

	// 	// Serial.println("WiFi is not setup yet. Creating default AP...");
	// 	// if(!WiFi.softAP(SETUP_WIFI_SSID, SETUP_WIFI_PASS)) {
	// 	// 	log_e("Soft AP creation failed.");
	// 	// 	//while(1);
	// 	// }
	// 	// WiFi.softAPConfig(
	// 	// 	IPAddress(192, 168, 1, 1),
	// 	// 	IPAddress(192, 168, 1, 1),
	// 	// 	IPAddress(255, 255, 255, 0)
	// 	// );
	// 	// IPAddress myIP = WiFi.softAPIP();
	// 	// Serial.print("AP IP address: ");
	// 	// Serial.println(myIP);
	// 	wifi.version = 1;
	// 	wifi.ip = 0;
	// 	strcpy(wifi.ssid, "<ssid>");
	// 	strcpy(wifi.password, "<password>");

	// 	File cfg = LittleFS.open(WIFI_CONFIG_FILE, FILE_WRITE, true);
	// 	if(cfg) {
	// 		cfg.write((byte*)&wifi, sizeof(wifi));
	// 		cfg.close();
	// 	}
	// }
}


// void taskReconnectWifi(void* pvParameters) {
// 	WiFiConfig* wifi = (WiFiConfig*)pvParameters;
// 	while(true) {
// 		log_i("Reconnecting to WiFi...");
// 		WiFi.disconnect(true);

// 		if(connectWiFi(*wifi)) {
// 			log_i("WiFi connected");
// 			log_i("IP address: %s", WiFi.localIP().toString().c_str());

// 			break;
// 		}

// 		log_e("Connection failed. Status: %d", WiFi.status());

// 		vTaskDelay(2000 / portTICK_PERIOD_MS);
// 	}
// 	delete wifi;

// 	vTaskDelete(NULL);
// }

bool connectWiFi(const WiFiConfig &wifi) {

	if(!WiFi.STA.config(
		   wifi.getIP(),
		   wifi.getGateway(),
		   wifi.getSubnet(),
		   wifi.getDNS1(),
		   wifi.getDNS2()
	   )) {
		log_e("Failed to set static IP with DNS.");
	}

	log_i("Connecting to network: %s", wifi.getSSID().c_str());

	if(wifi.isEAP()) {
		return WiFi.STA.connect(
			wifi.getSSID().c_str(),
			static_cast<wpa2_auth_method_t>(wifi.getEapMethod()),
			wifi.getEapIdentity().c_str(),
			wifi.getEapUsername().c_str(),
			wifi.getPassword().c_str(),
			wifi.getEapCaCert().c_str(),
			wifi.getEapClientCert().c_str(),
			wifi.getEapClientKey().c_str(),
			static_cast<int>(wifi.getEapPhase2Method())
		);
	}
	return WiFi.STA.connect(wifi.getSSID().c_str(), wifi.getPassword().c_str());
}

bool createSetupAP() {

	// log_i("Creating setup AP...");
	// WiFi.enableAP(true);

	// if(!WiFi.AP.create(
	// 	   SETUP_WIFI_SSID,
	// 	   SETUP_WIFI_PASS,
	// 	   WiFi.channel(), //it seems that ESP32 does only support AP and STA mode on the same channel
	// 	   false,
	// 	   2
	//    )) {
	// 	log_e("AP creation failed.");
	// 	return false;
	// }

	// WiFi.AP.config(
	// 	IPAddress(SETUP_WIFI_IP),
	// 	IPAddress(SETUP_WIFI_IP),
	// 	IPAddress(255, 255, 255, 0)
	// );

	// WiFi.AP.enableDhcpCaptivePortal();

	//TODO: turn off after a timeout if bridge is already setup

	return true;
}

bool stopAP() {
	log_i("Stopping setup AP...");
	return WiFi.enableAP(false);
}

// bool loadWifiConfig(WiFiConfig &config) {
// 	if(!LittleFS.exists(WIFI_CONFIG_FILE)) {
// 		return false;
// 	}
// 	File cfg = LittleFS.open(WIFI_CONFIG_FILE, FILE_READ);
// 	if(!cfg) {
// 		log_e("Failed to open wifi config file.");
// 		return false;
// 	}
// 	cfg.read((byte*)&config, sizeof(config));
// 	cfg.close();
// 	if(config.version != 1) {
// 		log_e("Invalid saved WiFi config version.");
// 		return false;
// 	}
// 	return true;
// }

// bool saveWifiConfig(const WiFiConfig &config) {
// 	eeprom.updateBlock(300, (byte*)&config, sizeof(config));
// 	return true;
// }

// bool connectWiFi(WiFiConfig &wifi, bool reportToLcd) {

// 	WiFi.mode(WIFI_STA);

// 	if(wifi.ip) {
// 		IPAddress localIP = IPAddress(wifi.ip);
// 		IPAddress gateway = IPAddress(wifi.gateway);
// 		IPAddress subnet = IPAddress(wifi.subnet);
// 		if(!WiFi.config(localIP, gateway, subnet)) {
// 			log_e("Failed to set static IP.");
// 		}
// 	}

// 	if(WiFi.begin(wifi.ssid, wifi.password) == WL_CONNECT_FAILED) {
// 		return false;
// 	}

// 	WiFi.setAutoReconnect(true);

// 	int retries = 16;
// 	while(WiFi.status() != WL_CONNECTED) {
// 		if(reportToLcd) lcd.print(".");
// 		vTaskDelay(500 / portTICK_PERIOD_MS);
// 		if(!--retries) {
// 			return false;
// 		}
// 	}

// 	return true;
// }

