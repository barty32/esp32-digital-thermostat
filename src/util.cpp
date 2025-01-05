
#include "main.h"
#include "ThermostatController.h"


struct WiFiConfig {
	char ssid[32];
	char password[32];
};


void setupCaptivePortal() {



}

void setupWebServer() {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		
		request->send(200, "text/plain", "Hello, world");
	});

	server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(200, "text/plain", String(Time::now().toEpoch()));
	});

	server.on("/api/time", HTTP_POST, [](AsyncWebServerRequest* request) {
		long time = request->getParam("time")->value().toInt();
		rtc.setTime(time, 0);
		request->send(200, "text/plain", "ok");
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
		stream.println("  sweep - Sweep the temperature range");
		stream.println("  dump - Dump the current schedule");
		stream.println("  setTemp - Set the current temperature");
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
