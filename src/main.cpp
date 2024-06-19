#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <secrets.h>

#define JSON_DOC_SIZE 2048
#define MSG_SIZE 128

#define PC_SWITCH_PIN D0 // D0
#define PC_LIGHT_PIN A0 // D1

WebSocketsClient wsClient;

void handleErrorMessage(const char *error) {
  char msg[MSG_SIZE];
  sprintf(msg, "{\"msg\":\"%s\"}", error);
  wsClient.sendTXT(msg);
}

void interpretPayload(uint8_t * payload) {
  StaticJsonDocument<JSON_DOC_SIZE> doc;

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserialize failed");
    Serial.println(error.f_str());
    handleErrorMessage(error.c_str());
    return;
  }

  if (!doc["msg"].is<const char *>()) {
    handleErrorMessage("invalid message type");
    return;
  }

  if (strcmp(doc["msg"], "CHECK STATE") == 0) {
    // CHECK PIN MODE RETURN THAT
    Serial.println("LIGHT V:");
    int lightV = analogRead(PC_LIGHT_PIN);
    Serial.println(analogRead(PC_LIGHT_PIN));
    char msg[MSG_SIZE];

    if (lightV < 10) {
      sprintf(msg, "{\"msg\":\"%s\"}", "OFF");
    } else {
      sprintf(msg, "{\"msg\":\"%s\"}", "ON");
    }
    
    wsClient.sendTXT(msg);

    // pinMode(doc["body"]["pin"])
  } else if (strcmp(doc["msg"], "ON") == 0) {
    // Check that PC is not powered on, if so, do nothing
    int lightV = analogRead(PC_LIGHT_PIN);

    if (lightV < 10) {
      // PC OFF --> PC ON
      digitalWrite(PC_SWITCH_PIN, HIGH);
      delay(150);
      digitalWrite(PC_SWITCH_PIN, LOW);
    }

    Serial.println("RECIEVED ON");
  } else if (strcmp(doc["msg"], "OFF") == 0) {
    // Check that PC is not powered off, if so, do nothing
    int lightV = analogRead(PC_LIGHT_PIN);

    if (lightV > 10) {
      // PC ON --> PC OFF
      digitalWrite(PC_SWITCH_PIN, HIGH);
      delay(150);
      digitalWrite(PC_SWITCH_PIN, LOW);
      Serial.println("RECIEVED OFF");
    }
  }
}

void onEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WS Connected");
      break;
    case WStype_DISCONNECTED:
      Serial.println("WS Disconnected");
      break;
    case WStype_TEXT:
      Serial.printf("WS Message: %s\n", payload);

      interpretPayload(payload);

      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Begin WiFi connections
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("CONNECTED TO WIFI!");

  pinMode(PC_LIGHT_PIN, INPUT);
  pinMode(PC_SWITCH_PIN, OUTPUT);

  wsClient.begin(WS_HOST, WS_PORT, WS_URL);
  wsClient.onEvent(onEvent);
}

void loop() {
  wsClient.loop();
}