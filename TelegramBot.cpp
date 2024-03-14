#include "TelegramBot.h"
#include <HTTPClient.h>

TelegramBot::TelegramBot(const String& bot_key, const String& chat_id, int eeprom_address) {
    this->bot_key = bot_key;
    this->chat_id = chat_id;
    this->eeprom_address = eeprom_address;
}

bool TelegramBot::get_new_message(String& message) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + bot_key + "/getUpdates?offset=-1");
    int httpResponseCode = http.GET();

    if (httpResponseCode != 200) return false;

    String source = http.getString();
    DeserializationError error = deserializeJson(doc, source);

    http.end();
    if (error) return false;

    if (!doc["result"][0]["message"]["message_id"].isNull() && !doc["result"][0]["message"]["text"].isNull()) {
        unsigned int message_id = doc["result"][0]["message"]["message_id"].as<unsigned int>();

        // Читаем last_message_id из EEPROM
        unsigned int last_message_id = 0;
        EEPROM.get(eeprom_address, last_message_id);

        // Если last_message_id не равен 0 и равен message_id из документа, возвращаем false
        Serial.print("last_message_id: ");
        Serial.println(last_message_id);
        Serial.print("message_id: ");
        Serial.println(message_id);
        Serial.print("eeprom_address: ");
        Serial.println(eeprom_address);


        if (last_message_id != 0 && last_message_id == message_id) {
            return false;
        }

        // Извлекаем текст сообщения
        message = doc["result"][0]["message"]["text"].as<String>();

        // Сохраняем message_id в EEPROM
        EEPROM.put(eeprom_address, message_id);
        EEPROM.commit();

        return true;
    }

    return false;
}

bool TelegramBot::send_message(const String& text) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + bot_key + "/sendMessage");
    http.addHeader("content-type", "application/json");
    String payload = "{\"chat_id\":\"" + chat_id + "\",\"text\":" + "\"" + text + "\"}";
    int httpResponseCode = http.POST(payload);
    http.end();

    return httpResponseCode == 200;
}
