#pragma once

//constants
//#define HOLD_TIME 500
#define MIN_TEMP 5.0
#define MAX_TEMP 35.0

#define SERIAL_BAUD 115200
#define HOSTNAME    "esp32-thermostat"
#define SERVER_PORT 80

#define SETUP_WIFI_SSID "esp32thermostat"
#define SETUP_WIFI_PASS NULL

#define LCD_I2C_ADDR 0x27
#define EEPROM_I2C_ADDR 0x50
#define EEPROM_I2C_SIZE I2C_DEVICESIZE_24LC04

//pin definitions
#define BTN_UP_PIN    36
#define BTN_DOWN_PIN  32
#define BTN_LEFT_PIN  39
#define BTN_RIGHT_PIN 35
#define BTN_MODE_PIN  34

#define LED_BUILTIN_PIN 2
#define ANALOG_CTRL_PIN 25

#define TEMP_SENSOR_PIN 26
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22
#define VSPI_MISO_PIN   19
#define VSPI_MOSI_PIN   23
#define VSPI_CLK_PIN    18
#define VSPI_CS_PIN     5
