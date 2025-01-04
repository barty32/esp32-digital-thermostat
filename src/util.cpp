
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
