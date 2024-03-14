#include <Arduino.h>
#include <WiFi.h>
#include "TelegramBot.h"

const char* ssid = "xxx";
const char* password = "xxx";
const String bot_key = "xxx";
const String chat_id = "xxx";
const int eeprom_address = 0; // Адрес EEPROM для хранения message_id

TelegramBot bot(bot_key, chat_id, eeprom_address);

unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 60000; // Проверяем новые сообщения каждую минуту

void setup() {
    Serial.begin(115200);
    EEPROM.begin(512); // Инициализируем EEPROM

    WiFi.onEvent(reconnect_WiFi, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    reconnectWiFi();
}

void loop() {
    if (millis() - lastCheckTime >= checkInterval) {
        String message;
        if (bot.get_new_message(message)) {
            Serial.println("New message: " + message);
        }
        lastCheckTime = millis();
    }
}

void reconnectWiFi() {
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
    } else {
        Serial.println("Connection failed. Restarting...");
        ESP.restart();
    }
}

void reconnect_WiFi(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from WiFi. Reconnecting...");
    reconnectWiFi();
}
