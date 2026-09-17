
#include "main.h"
#include <atomic>

using namespace express;

auto app = express::express();

BaseType_t startHTTPTask() {
	return xTaskCreate(
		taskHTTP,
		"HTTP Server task",
		20000,
		nullptr,
		5,
		nullptr
	);
}

void taskHTTP(void* pvParameters) {

	//wait until WiFi is configured
	// xSemaphoreTake(wifiSemaphore, 5000 / portTICK_PERIOD_MS);

	log_i("Setting up HTTP server...");

	app.use(express::cors());
	// app.use(express::json());

	app.use([](Request &req, Response &res, const NextFunction &next) {
		log_i("HTTP Request: %s %s", methodToString(req.method), req.url.c_str());
		log_i("Free internal heap size: %d", esp_get_free_internal_heap_size());
		next();
	});

	app.use("/assets", express::serveStatic(LittleFS, "/client"));

	// server.onNotFound([](AsyncWebServerRequest* request) {
	// 	request->send(404, "text/plain", "404 Not found");
	// });

	// //client app
	// server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	if(!LittleFS.exists("/client/index.html.gz")) {
	// 		request->send(500, "text/plain", "Error - client code has not been uploaded. Please flash the filesystem image.");
	// 		return;
	// 	}
	// 	AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/client/index.html.gz", "text/html");
	// 	response->addHeader("Content-Encoding", "gzip");
	// 	request->send(response);
	// });

	setupApiEndpoints();

	log_i("api endpoints set up");

	// Catch-all route to index.html (client-side routing)
	app.use([](Request &req, Response &res, const NextFunction &next) {
		if(req.path() == "/iqrf-logo.png") {
			res.type("image/png");
			res.sendFile(LittleFS, "/client/iqrf-logo.png");
			return;
		}
		if((req.method == HTTP_GET || req.method == HTTP_HEAD)) {
			if(!LittleFS.exists("/client/index.html")) {
				res.status(500).send("Error - client code has not been uploaded. Please flash the filesystem image.");
				return;
			}
			res.type("text/html");
			res.sendFile(LittleFS, "/client/index.html");
			return;
		}
		next();
	});

	log_i("Free internal heap size: %d", esp_get_free_internal_heap_size());

	// server.begin();
	// app.listen(SERVER_PORT);
	if(HTTP::createServer(app).listen(SERVER_PORT)) {
		log_i("HTTP server started on port %d", SERVER_PORT);
	}

	// if(HTTPS::createServer(app).listen(443)) {
	// 	log_i("HTTPS server started on port %d", 443);
	// }

	while(true) {
		//do nothing
		delay(10);
	}
}

void setupApiEndpoints() {
	// server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {

	// });

	// server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	File file = LittleFS.open(THERMOSTAT_CONFIG_FILE, FILE_READ);
	// 	if(!file) {
	// 		request->send(500, "text/plain", "Failed to open config file.");
	// 		return;
	// 	}

	// 	ThermostatController::PersistentConfig cfg;
	// 	file.read((byte*)&cfg, sizeof(cfg));
	// 	file.close();

	// 	request->send(200, "application/octet-stream", (byte*)&cfg, sizeof(cfg));
	// });

	// server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	request->send(200, "text/plain", String(Time::now().toEpoch()));
	// });

	// server.on("/api/time", HTTP_POST, [](AsyncWebServerRequest* request) {
	// 	long time = request->getParam("time")->value().toInt();
	// 	rtc.setTime(time, 0);
	// 	request->send(200, "text/plain", "ok");
	// });

	// server.on("/api/temperature/history", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	AsyncJsonResponse* response = new AsyncJsonResponse();
	// 	JsonArray root = response->getRoot().to<JsonArray>();

	// 	File meta = LittleFS.open(HISTORY_METADATA_FILE, FILE_READ);
	// 	File history = LittleFS.open(TEMPERATURE_HISTORY_FILE, FILE_READ);
	// 	if(!meta || !history) {
	// 		JsonObject root = response->getRoot().to<JsonObject>();
	// 		root["success"] = false;
	// 		root["error"] = "Failed to open history files.";
	// 		response->setLength();
	// 		response->setCode(500);
	// 		request->send(response);
	// 		return;
	// 	}

	// 	struct TempMeta {
	// 		int numPoints;
	// 		int pointer;
	// 	};

	// 	struct TempRecord {
	// 		int64_t timestamp;
	// 		float temperature;
	// 	};

	// 	TempMeta metaInfo;
	// 	meta.read((byte*)&metaInfo, sizeof(metaInfo));
	// 	meta.close();

	// 	for(int i = 0; i < metaInfo.numPoints; i++) {
	// 		TempRecord record;
	// 		history.read((byte*)&record, sizeof(record));
	// 		JsonArray obj = root[i].to<JsonArray>();
	// 		obj[0] = record.timestamp;
	// 		if(record.temperature == Temperature::NOT_SET) obj[1] = nullptr;
	// 		else obj[1] = record.temperature;
	// 	}
	// 	history.close();

	// 	response->setLength();
	// 	request->send(response);
	// });

	// server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	Temperature temp = thermostat.getCurrentTemperature();
	// 	request->send(
	// 		200,
	// 		"text/plain",
	// 		temp == Temperature::NOT_SET ? "null" : String(temp.temperature, 1)
	// 	);
	// });

	// server.on("/api/temperature", HTTP_POST, [](AsyncWebServerRequest* request) {
	// 	Temperature temp = request->getParam("temp")->value().toFloat();
	// 	thermostat.sendTemperatureUpdate(temp.temperature);
	// 	request->send(200, "text/plain", "ok");
	// });

	// server.on("/api/slots/time", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	AsyncJsonResponse* response = new AsyncJsonResponse();
	// 	JsonArray root = response->getRoot().to<JsonArray>();
	// 	for(int i = 0; i < ThermostatController::TIME_SLOT_COUNT; i++) {
	// 		TimeSlot* slot = thermostat.getTimeSlot(i);
	// 		JsonObject obj = root[i].to<JsonObject>();
	// 		obj["startTime"] = slot->startTime.toEpoch();
	// 		obj["endTime"] = slot->endTime.toEpoch();
	// 		if(slot->temperatureSlot) {
	// 			obj["temperatureSlot"] = slot->temperatureSlot - thermostat.getTemperatureSlot(0);
	// 		}
	// 		else {
	// 			obj["temperatureSlot"] = nullptr;
	// 		}
	// 		obj["days"] = slot->daysEnabled.toByte();
	// 		obj["active"] = slot->active;
	// 	}
	// 	response->setLength();
	// 	request->send(response);
	// });

	// AsyncCallbackJsonWebHandler* jsonHandler = new AsyncCallbackJsonWebHandler("/api/slots/time");
	// jsonHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant &json) {
	// 	AsyncJsonResponse* response = new AsyncJsonResponse();
	// 	JsonObject root = response->getRoot().to<JsonObject>();
	// 	try {
	// 		if(!json.is<JsonArray>()) {
	// 			throw "Invalid Request";
	// 		}
	// 		JsonArray arr = json.as<JsonArray>();
	// 		for(int i = 0; i < arr.size(); i++) {
	// 			JsonObject obj = arr[i].as<JsonObject>();
	// 			int index = obj["slot"].as<int>();
	// 			//TODO: check size
	// 			TimeSlot* slot = thermostat.getTimeSlot(index);
	// 			slot->startTime = obj["startTime"].as<Time::time_t>();
	// 			slot->endTime = obj["endTime"].as<Time::time_t>();
	// 			slot->daysEnabled = (Day::Days)obj["days"].as<uint8_t>();
	// 			slot->active = obj["active"].as<bool>();
	// 			if(obj["temperatureSlot"].isNull()) {
	// 				slot->temperatureSlot = nullptr;
	// 			}
	// 			else {
	// 				int tempIndex = obj["temperatureSlot"].as<int>();
	// 				slot->temperatureSlot = thermostat.getTemperatureSlot(tempIndex);
	// 			}
	// 		}
	// 	}
	// 	catch(const char* e) {
	// 		root["success"] = false;
	// 		root["error"] = e;
	// 		response->setLength();
	// 		response->setCode(400);
	// 		request->send(response);
	// 	}

	// 	root["success"] = true;
	// 	response->setLength();
	// 	request->send(response);
	// });
	// jsonHandler->setMethod(HTTP_POST | HTTP_PUT);
	// server.addHandler(jsonHandler);

	// server.on("/api/slots/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
	// 	AsyncJsonResponse* response = new AsyncJsonResponse();
	// 	JsonArray root = response->getRoot().to<JsonArray>();
	// 	for(int i = 0; i < ThermostatController::TEMP_SLOT_COUNT; i++) {
	// 		Temperature* slot = thermostat.getTemperatureSlot(i);
	// 		if(slot->temperature == Temperature::NOT_SET) {
	// 			root.add(nullptr);
	// 		}
	// 		else {
	// 			root.add(slot->temperature);
	// 		}
	// 	}
	// 	response->setLength();
	// 	request->send(response);
	// });

	// jsonHandler = new AsyncCallbackJsonWebHandler("/api/slots/temperature");
	// jsonHandler->onRequest([](AsyncWebServerRequest* request, JsonVariant &json) {
	// 	AsyncJsonResponse* response = new AsyncJsonResponse();
	// 	JsonObject root = response->getRoot().to<JsonObject>();
	// 	try {
	// 		if(!json.is<JsonArray>()) {
	// 			throw "Invalid Request";
	// 		}
	// 		JsonArray arr = json.as<JsonArray>();
	// 		for(int i = 0; i < arr.size(); i++) {
	// 			JsonObject obj = arr[i].as<JsonObject>();
	// 			int index = obj["slot"].as<int>();
	// 			//TODO: check size
	// 			if(obj["temperature"].isNull()) {
	// 				thermostat.getTemperatureSlot(index)->temperature = Temperature::NOT_SET;
	// 			}
	// 			else {
	// 				thermostat.getTemperatureSlot(index)->temperature = obj["temperature"].as<float>();
	// 			}
	// 		}
	// 	}
	// 	catch(const char* e) {
	// 		root["success"] = false;
	// 		root["error"] = e;
	// 		response->setLength();
	// 		response->setCode(400);
	// 		request->send(response);
	// 	}

	// 	root["success"] = true;
	// 	response->setLength();
	// 	request->send(response);
	// });
	// jsonHandler->setMethod(HTTP_POST | HTTP_PUT);
	// server.addHandler(jsonHandler);
}
