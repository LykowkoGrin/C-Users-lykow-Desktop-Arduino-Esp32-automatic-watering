#include <Arduino.h>
#include <WiFi.h>
#include "TelegramBot.h"
#include <EEPROM.h>

const char* ssid = "xxx";
const char* password = "xxx";
const String bot_key = "xxx";
const String chat_id = "xxx";

const int telegram_eeprom_address = 0; // Адрес EEPROM для хранения message_id для TelegramBot
const int relay_eeprom_address = telegram_eeprom_address + sizeof(unsigned long); // Адрес EEPROM для хранения состояния реле

const int relayPin = 4; // Пин для управления реле

TelegramBot bot(bot_key, chat_id, telegram_eeprom_address);

unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 60000; // Проверяем новые сообщения каждую минуту

void setup() {
    Serial.begin(115200);
    EEPROM.begin(100);
    pinMode(relayPin, OUTPUT); // Установка пина реле как выхода

    // Читаем состояние реле из EEPROM и устанавливаем его
    int relayState = EEPROM.read(relay_eeprom_address);
    digitalWrite(relayPin, relayState);

    Serial.println(relayState);

    WiFi.onEvent(reconnect_WiFi, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    reconnectWiFi();
}

void loop() {
    if (millis() - lastCheckTime >= checkInterval) {
        String message;
        if (bot.get_new_message(message)) {
            Serial.println("New message: " + message);
            if (message.equals("/включить")) {
                digitalWrite(relayPin, HIGH); // Включить реле
                EEPROM.write(relay_eeprom_address, HIGH); // Сохранить состояние в EEPROM
                EEPROM.commit();
                Serial.println("Relay turned ON");
            } else if (message.equals("/выключить")) {
                digitalWrite(relayPin, LOW); // Выключить реле
                EEPROM.write(relay_eeprom_address, LOW); // Сохранить состояние в EEPROM
                EEPROM.commit();
                Serial.println("Relay turned OFF");
            }
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
