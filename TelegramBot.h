#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

class TelegramBot {
private:
    String bot_key;
    String chat_id;
    int eeprom_address;
    JsonDocument doc; // Объявляем JsonDocument как член класса

public:
    TelegramBot(const String& bot_key, const String& chat_id, int eeprom_address);
    bool get_new_message(String& message);
    bool send_message(const String& text);
};
