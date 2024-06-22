#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <secrets.h>

#define JSON_DOC_SIZE 2048
#define MSG_SIZE 128

#define PC_SWITCH_PIN D0
#define PC_LIGHT_PIN A0

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
    handleErrorMessage(error.c_str());
    return;
  }

  if (!doc["msg"].is<const char *>()) {
    handleErrorMessage("invalid message type");
    return;
  }

  if (strcmp(doc["msg"], "CHECK STATE") == 0) {
    // CHECK PIN MODE AND RETURN --> "OFF" or "ON" 
    int lightV = analogRead(PC_LIGHT_PIN);
    char msg[MSG_SIZE];

    if (lightV < 300) {
      sprintf(msg, "{\"msg\":\"%s\"}", "OFF");
    } else {
      sprintf(msg, "{\"msg\":\"%s\"}", "ON");
    }
    
    wsClient.sendTXT(msg);
  } else if (strcmp(doc["msg"], "ON") == 0) {
    // Check that PC is not powered on, if so, do nothing
    int lightV = analogRead(PC_LIGHT_PIN);

    if (lightV < 300) {
      // PC OFF --> PC ON
      digitalWrite(PC_SWITCH_PIN, HIGH);
      delay(400);
      digitalWrite(PC_SWITCH_PIN, LOW);
    }
  } else if (strcmp(doc["msg"], "OFF") == 0) {
    // Check that PC is not powered off, if so, do nothing
    int lightV = analogRead(PC_LIGHT_PIN);

    if (lightV >= 300) {
      // PC ON --> PC OFF
      digitalWrite(PC_SWITCH_PIN, HIGH);
      delay(400);
      digitalWrite(PC_SWITCH_PIN, LOW);
    }
  }
}

void onEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      interpretPayload(payload);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Begin WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  pinMode(PC_LIGHT_PIN, INPUT);
  pinMode(PC_SWITCH_PIN, OUTPUT);
  digitalWrite(PC_SWITCH_PIN, LOW);

  wsClient.beginSSL(WS_HOST, WS_PORT, WS_URL);
  wsClient.onEvent(onEvent);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    wsClient.loop();
  } else {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
  }
}