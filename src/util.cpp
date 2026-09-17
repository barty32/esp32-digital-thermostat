
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
		// if(loadWifiConfig(wifi)) {
		// 	stream.println("SSID: " + String(wifi.ssid));
		// 	stream.println("Password: " + String(wifi.password));
		// }
		// else {
			stream.println("No WiFi config found.");
		// }
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
	else if(command == "time") {
		if(params.startsWith("set")) {
			String timeStr = params.substring(3);
			timeStr.trim();
			int64_t epoch = timeStr.toDouble();
			rtc.setTime(epoch);

			// tm* timeinfo = localtime((time_t*)&epoch);
			ds1307.adjust(DateTime(epoch));
			// ds1307.adjust(DateTime(Time::now().toEpoch()));
			stream.println("RTC time set to: " + String(ctime(&epoch)));
		}
		else {
			stream.println("Usage: time set <epoch>");
		}
	}
	else if(command == "help") {
		stream.println("Available commands:");
		stream.println("  clear - Clear the terminal screen");
		stream.println("  ping - Pong!");
		stream.println("  reboot - Reboot the device");
		stream.println("  uptime - Show the device uptime");
		stream.println("  time set - Set RTC time");
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
