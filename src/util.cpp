
#include "main.h"
#include "ThermostatController.h"


bool loadThermostatConfig() {
	if(!LittleFS.exists(THERMOSTAT_CONFIG_FILE)) {
		return false;
	}
	ThermostatController::PersistentConfig config;
	File cfg = LittleFS.open(THERMOSTAT_CONFIG_FILE, FILE_READ);
	if(!cfg) {
		log_e("Failed to open thermostat config file.");
		return false;
	}
	
	cfg.read((byte*)&config, sizeof(config));
	cfg.close();
	if(!thermostat.loadConfig(config)) {
		return false;
	}
	return true;
}

bool saveThermostatConfig() {
	ThermostatController::PersistentConfig config;
	File cfg = LittleFS.open(THERMOSTAT_CONFIG_FILE, FILE_WRITE, true);
	if(!cfg) {
		log_e("Failed to open thermostat config file.");
		return false;
	}
	thermostat.storeConfig(config);
	cfg.write((byte*)&config, sizeof(config));
	cfg.close();
	return true;
}

bool loadWifiConfig(WiFiConfig &config) {
	if(!LittleFS.exists(WIFI_CONFIG_FILE)) {
		return false;
	}
	File cfg = LittleFS.open(WIFI_CONFIG_FILE, FILE_READ);
	if(!cfg) {
		log_e("Failed to open wifi config file.");
		return false;
	}
	cfg.read((byte*)&config, sizeof(config));
	cfg.close();
	if(config.version != 1) {
		log_e("Invalid saved WiFi config version.");
		return false;
	}
	return true;
}

// bool saveWifiConfig(const WiFiConfig &config) {
// 	eeprom.updateBlock(300, (byte*)&config, sizeof(config));
// 	return true;
// }

bool connectWiFi(WiFiConfig &wifi, bool reportToLcd) {

	WiFi.mode(WIFI_STA);

	if(wifi.ip) {
		IPAddress localIP = IPAddress(wifi.ip);
		IPAddress gateway = IPAddress(wifi.gateway);
		IPAddress subnet = IPAddress(wifi.subnet);
		if(!WiFi.config(localIP, gateway, subnet)) {
			log_e("Failed to set static IP.");
		}
	}

	if(WiFi.begin(wifi.ssid, wifi.password) == WL_CONNECT_FAILED) {
		return false;
	}

	WiFi.setAutoReconnect(true);

	int retries = 16;
	while(WiFi.status() != WL_CONNECTED) {
		if(reportToLcd) lcd.print(".");
		vTaskDelay(500 / portTICK_PERIOD_MS);
		if(!--retries) {
			return false;
		}
	}

	return true;
}


void setupCaptivePortal() {



}

void setupApiEndpoints() {

	server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {

	});

	server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest* request) {
		File file = LittleFS.open(THERMOSTAT_CONFIG_FILE, FILE_READ);
		if(!file) {
			request->send(500, "text/plain", "Failed to open config file.");
			return;
		}

		ThermostatController::PersistentConfig cfg;
		file.read((byte*)&cfg, sizeof(cfg));
		file.close();

		request->send(200, "application/octet-stream", (byte*)&cfg, sizeof(cfg));
	});

	server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(200, "text/plain", String(Time::now().toEpoch()));
	});

	server.on("/api/time", HTTP_POST, [](AsyncWebServerRequest* request) {
		long time = request->getParam("time")->value().toInt();
		rtc.setTime(time, 0);
		request->send(200, "text/plain", "ok");
	});

	server.on("/api/temperature/history", HTTP_GET, [](AsyncWebServerRequest* request) {
		AsyncJsonResponse* response = new AsyncJsonResponse();
		JsonArray root = response->getRoot().to<JsonArray>();

		File meta = LittleFS.open(HISTORY_METADATA_FILE, FILE_READ);
		File history = LittleFS.open(TEMPERATURE_HISTORY_FILE, FILE_READ);
		if(!meta || !history) {
			JsonObject root = response->getRoot().to<JsonObject>();
			root["success"] = false;
			root["error"] = "Failed to open history files.";
			response->setLength();
			response->setCode(500);
			request->send(response);
			return;
		}

		struct TempMeta {
			int numPoints;
			int pointer;
		};

		struct TempRecord {
			int64_t timestamp;
			float temperature;
		};

		TempMeta metaInfo;
		meta.read((byte*)&metaInfo, sizeof(metaInfo));
		meta.close();

		for(int i = 0; i < metaInfo.numPoints; i++) {
			TempRecord record;
			history.read((byte*)&record, sizeof(record));
			JsonArray obj = root[i].to<JsonArray>();
			obj[0] = record.timestamp;
			if(record.temperature == Temperature::NOT_SET) obj[1] = nullptr;
			else obj[1] = record.temperature;
		}
		history.close();

		response->setLength();
		request->send(response);
	});

	server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
		Temperature temp = thermostat.getCurrentTemperature();
		request->send(
			200,
			"text/plain",
			temp == Temperature::NOT_SET ? "null" : String(temp.temperature, 1)
		);
	});

	server.on("/api/temperature", HTTP_POST, [](AsyncWebServerRequest* request) {
		Temperature temp = request->getParam("temp")->value().toFloat();
		thermostat.sendTemperatureUpdate(temp.temperature);
		request->send(200, "text/plain", "ok");
	});

	server.on("/api/slots/time", HTTP_GET, [](AsyncWebServerRequest* request) {
		AsyncJsonResponse* response = new AsyncJsonResponse();
		JsonArray root = response->getRoot().to<JsonArray>();
		for(int i = 0; i < ThermostatController::TIME_SLOT_COUNT; i++) {
			TimeSlot* slot = thermostat.getTimeSlot(i);
			JsonObject obj = root[i].to<JsonObject>();
			obj["startTime"] = slot->startTime.toEpoch();
			obj["endTime"] = slot->endTime.toEpoch();
			if(slot->temperatureSlot) {
				obj["temperatureSlot"] = slot->temperatureSlot - thermostat.getTemperatureSlot(0);
			}
			else {
				obj["temperatureSlot"] = nullptr;
			}
			obj["days"] = slot->daysEnabled.toByte();
			obj["active"] = slot->active;
		}
		response->setLength();
		request->send(response);
	});

	AsyncCallbackJsonWebHandler* jsonHandler = new AsyncCallbackJsonWebHandler("/api/slots/time");
	jsonHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant &json) {
		AsyncJsonResponse* response = new AsyncJsonResponse();
		JsonObject root = response->getRoot().to<JsonObject>();
		try {
			if(!json.is<JsonArray>()) {
				throw "Invalid Request";
			}
			JsonArray arr = json.as<JsonArray>();
			for(int i = 0; i < arr.size(); i++) {
				JsonObject obj = arr[i].as<JsonObject>();
				int index = obj["slot"].as<int>();
				//TODO: check size
				TimeSlot* slot = thermostat.getTimeSlot(index);
				slot->startTime = obj["startTime"].as<Time::time_t>();
				slot->endTime = obj["endTime"].as<Time::time_t>();
				slot->daysEnabled = (Day::Days)obj["days"].as<uint8_t>();
				slot->active = obj["active"].as<bool>();
				if(obj["temperatureSlot"].isNull()) {
					slot->temperatureSlot = nullptr;
				}
				else {
					int tempIndex = obj["temperatureSlot"].as<int>();
					slot->temperatureSlot = thermostat.getTemperatureSlot(tempIndex);
				}
			}
		}
		catch(const char* e) {
			root["success"] = false;
			root["error"] = e;
			response->setLength();
			response->setCode(400);
			request->send(response);
		}

		root["success"] = true;
		response->setLength();
		request->send(response);
	});
	jsonHandler->setMethod(HTTP_POST | HTTP_PUT);
	server.addHandler(jsonHandler);

	server.on("/api/slots/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
		AsyncJsonResponse* response = new AsyncJsonResponse();
		JsonArray root = response->getRoot().to<JsonArray>();
		for(int i = 0; i < ThermostatController::TEMP_SLOT_COUNT; i++) {
			Temperature* slot = thermostat.getTemperatureSlot(i);
			if(slot->temperature == Temperature::NOT_SET) {
				root.add(nullptr);
			}
			else {
				root.add(slot->temperature);
			}
		}
		response->setLength();
		request->send(response);
	});

	jsonHandler = new AsyncCallbackJsonWebHandler("/api/slots/temperature");
	jsonHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant &json) {
		AsyncJsonResponse* response = new AsyncJsonResponse();
		JsonObject root = response->getRoot().to<JsonObject>();
		try {
			if(!json.is<JsonArray>()) {
				throw "Invalid Request";
			}
			JsonArray arr = json.as<JsonArray>();
			for(int i = 0; i < arr.size(); i++) {
				JsonObject obj = arr[i].as<JsonObject>();
				int index = obj["slot"].as<int>();
				//TODO: check size
				if(obj["temperature"].isNull()) {
					thermostat.getTemperatureSlot(index)->temperature = Temperature::NOT_SET;
				}
				else {
					thermostat.getTemperatureSlot(index)->temperature = obj["temperature"].as<float>();
				}
			}
		}
		catch(const char* e) {
			root["success"] = false;
			root["error"] = e;
			response->setLength();
			response->setCode(400);
			request->send(response);
		}

		root["success"] = true;
		response->setLength();
		request->send(response);
	});
	jsonHandler->setMethod(HTTP_POST | HTTP_PUT);
	server.addHandler(jsonHandler);
}

void logCurrentTemperature() {
	
	struct TempMeta {
		int numPoints;
		int pointer;
	};

	struct TempRecord {
		int64_t timestamp;
		float temperature;
	};

	const int maxPoints = 1440;
	Temperature temp = thermostat.getCurrentTemperature();
	TempRecord current;
	current.timestamp = Time::now().toEpoch();
	current.temperature = temp.temperature;

	File file;
	File meta;
	if(!LittleFS.exists(HISTORY_METADATA_FILE) || !LittleFS.exists(TEMPERATURE_HISTORY_FILE)) {
		meta = LittleFS.open(HISTORY_METADATA_FILE, "w", true);
		file = LittleFS.open(TEMPERATURE_HISTORY_FILE, "w", true);
		if(!file || !meta) {
			log_e("Failed to open temperature log file.");
			return;
		}

		TempMeta data;
		data.numPoints = 0;
		data.pointer = 0;
		meta.write((byte*)&data, sizeof(TempMeta));
		meta.close();
		file.write((byte*)&current, sizeof(TempRecord));
		file.close();
	}
	else {
		meta = LittleFS.open(HISTORY_METADATA_FILE, "r+", true);
		file = LittleFS.open(TEMPERATURE_HISTORY_FILE, "r+", true);
		if(!file || !meta) {
			log_e("Failed to open temperature log file.");
			return;
		}

		TempMeta data;
		meta.read((byte*)&data, sizeof(TempMeta));

		file.seek(data.pointer * sizeof(TempRecord));
		file.write((byte*)&current, sizeof(TempRecord));
		data.pointer++;
		if(data.pointer >= maxPoints) data.pointer = 0;
		if(data.numPoints < maxPoints) data.numPoints++;
		meta.seek(0);
		meta.write((byte*)&data, sizeof(TempMeta));
		meta.close();
		file.close();
	}
}

// int timedRead(Stream &stream, int timeout = 1000) {
// 	int64_t start = esp_timer_get_time();
// 	do {
// 		int c = stream.read();
// 		if(c >= 0) {
// 			return c;
// 		}
// 		vTaskDelay(10);
// 	} while(esp_timer_get_time() - start < timeout * 1000);
// 	return -1; // -1 indicates timeout
// }

// String readStreamUntil(Stream &stream, char terminator, int timeout = 1000) {
// 	String ret;
// 	int c = timedRead(stream, timeout);
// 	while(c >= 0 && c != terminator) {
// 		ret += (char)c;
// 		c = timedRead(stream, timeout);
// 	}
// 	return ret;
// }

void handleTerminalCommand(Terminal& terminal, const String &command, const String &params) {

	Stream &stream = terminal.getStream();

	if(command.isEmpty()) {
		//do nothing
	}
	else if(command == "clear") {
		stream.print("\e[2J\e[H");
		//stream.print((char)0x0C);
	}
	else if(command == "ping") {
		stream.println("pong");
	}
	else if(command == "reboot") {
		stream.println("Rebooting...");
		ESP.restart();
		return;
	}
	else if(command == "uptime") {
		Time time = Time::millis();
		stream.printf("Uptime: %02d:%02d:%02d\r\n", time.getHours(), time.getMinutes(), time.getSeconds());
	}
	else if(command == "wifi") {
		WiFiConfig wifi;
		if(loadWifiConfig(wifi)) {
			stream.println("SSID: " + String(wifi.ssid));
			stream.println("Password: " + String(wifi.password));
		}
		else {
			stream.println("No WiFi config found.");
		}
	}
	else if(command == "temperature") {
		if(params.startsWith("list")) {
			for(int i = 0; i < ThermostatController::TEMP_SLOT_COUNT; i++) {
				stream.printf("%d: %s\r\n", i, thermostat.getTemperatureSlot(i)->toString(1).c_str());
			}
		}
		else if(params.startsWith("set")) {
			// int slot = params.substring(3).toInt();
			// float temp = params.substring(5).toFloat();
			// thermostat.getTemperatureSlot(slot)->temperature = temp;
			// stream.printf("Set temperature slot %d to %s\n", slot, thermostat.getTemperatureSlot(slot)->toString().c_str());
		}
		else {
			stream.println("Usage: temperature [list|set <slot> <temperature>]");
		}
	}
	else if(command == "sweep") {
		/*for(int i = 0; i < 1440; ++i) {
			stream.println(sweep(i * 60) ? "1," : "0,");
		}*/
	}
	else if(command == "dump") {
		/*String line1 = "+------+";
		String line2 = "| Slot |";
		String line3 = "+------+";
		for(int i = 0; i < 8; i++) {
			line1 += "-------+";
			line2 += "   " + String(i) + "   |";
			line3 += "-------+";
		}
		stream.println(line1);
		stream.println(line2);
		stream.println(line3);

		int i = 0;
		for(auto &day : slots) {
			String line1 = "| " + String(dayNames[i]) + "  |";
			String line2 = "|      |";
			String line3 = "|------+";
			for(auto &slot : day) {
				line1 += " " + printTime(slot.startTime) + " |";
				line2 += " " + printTime(slot.endTime) + " |";
				line3 += "-------+";
			}
			stream.println(line1);
			stream.println(line2);
			stream.println(line3);
			i++;
		}*/
	}
	else if(command == "setTemp") {
		// const int temp = int(payload.toFloat() * 10.0);
		// currentTemperature = temp;
		// stream.println("Set current temperature to: " + String(temp / 10.0));
	}
	else if(command == "help") {
		stream.println("Available commands:");
		stream.println("  clear - Clear the terminal screen");
		stream.println("  ping - Pong!");
		stream.println("  reboot - Reboot the device");
		stream.println("  uptime - Show the device uptime");
		stream.println("  wifi - Show WiFi configuration");
		stream.println("  temperature - Manage temperature slots");
		stream.println("  slot - Manage time slots");
		stream.println("  dump - Dump the current schedule");
	}
	else {
		stream.println("Unknown command: '" + command + "'. Use 'help' for a list of commands.");
	}
	//+------+-------+-------+---
	//| Slot |   1   |   2   |
	//|------+-------+-------+---
	//| Mon  | --:-- |  5:30 |
	//|      | --:-- |  6:30 |
	//|------+-------+-------+---
	//
	//
	//
	// long temp = received.toInt();
	// if (temp == 0) {
	//     //vypise vsechny zmeny
	//     for (auto& change : changes) {
	// 		printstream(change, false);
	// 	}
	// }
	// else if (temp > 250 || temp < 150) {
	// 	//stream3.println("mimo rozsah!");
	//     printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
	// } else {
	//     temp -= temp % 5;//zaokrouhleni
	// 	setTemperature = temp;
	//     printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
	// }

	terminal.prompt();
}
