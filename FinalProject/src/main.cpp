#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"


// Replace with your network credentials
const char* ssid = "ECET_IoT_2G_334_434";
const char* password = "Spring2026";

int motor1P1 = 2;
int motor1P2 = 4;
int motor2P1 = 18;
int motor2P2 = 19;
int motor3P1 = 22;
int motor3P2 = 23;
int motor4P1 = 27;
int motor4P2 = 26;
int motor5P1 = 33;
int motor5P2 = 32;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


// init spiffs
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize Wi-Fi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void down(int p1, int p2){
  digitalWrite(p1, LOW);
  digitalWrite(p2, HIGH);
}

void up(int p1, int p2){
  digitalWrite(p1, HIGH);
  digitalWrite(p2, LOW);
}

void stop(int p1, int p2){
  digitalWrite(p1, LOW);
  digitalWrite(p2, LOW);
}

void finalFunction(int motorNumber, String command){
  int p1;
  int p2;

  switch (motorNumber)
  {
  case 1:
    p1 = motor1P1;
    p2 = motor1P2;
    break;
  case 2:
    p1 = motor2P1;
    p2 = motor2P2;
    break;
  case 3:
    p1 = motor3P1;
    p2 = motor3P2;
    break;
  case 4:
    p1 = motor4P1;
    p2 = motor4P2;
    break;
  case 5:
    p1 = motor5P1;
    p2 = motor5P2;
    break;
  default:
    break;
  }

  if(command == "CW"){
    up(p1, p2);
  } else if(command == "CCW"){
    down(p1, p2);
  } else if(command == "STOP"){
    stop(p1, p2);
  }
  
}

// Handle incoming WebSocket messages
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String msg = (char*)data;

    Serial.print("WebSocket message: ");
    Serial.println(msg);


    // Expected message format: "1s40", "2s75", "3s20"
    int separator = msg.indexOf(':');
    if (separator > 0) {
      int motor = msg.substring(0, separator).toInt();
      String value = msg.substring(separator + 1);
      //function to move motor
      finalFunction(motor, value);
    }
  }
}

// WebSocket event handler
void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void                 *arg,
             uint8_t              *data,
             size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n",
                    client->id(),
                    client->remoteIP().toString().c_str());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;

    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;

    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Initialize WebSocket
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initSPIFFS();
  initWiFi();
  initWebSocket();

  pinMode(motor1P1,OUTPUT);
  pinMode(motor1P2,OUTPUT);
  pinMode(motor2P1,OUTPUT);
  pinMode(motor2P2,OUTPUT);
  pinMode(motor3P1,OUTPUT);
  pinMode(motor3P2,OUTPUT);
  pinMode(motor4P1,OUTPUT);
  pinMode(motor4P2,OUTPUT);
  pinMode(motor5P1,OUTPUT);
  pinMode(motor5P2,OUTPUT);

  // Serve webpage files from SPIFFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  ws.cleanupClients();
}
