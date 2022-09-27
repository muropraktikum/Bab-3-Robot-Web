#include<ESP8266WiFi.h> 
#include<ESPAsyncTCP.h> 
#include<ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h> 
#include <Servo.h>

#define base_servo_pin D5
#define wifi_no_led D8

Servo base_servo;

// Ganti dengan nama serta password WiFi yang akan digunakan 
const char* ssid = "YOUR_SSID_WIFI";
const char* password = "YOUR_PASSWORD_WIFI";

// Membuat objek AsyncWebServer di port 80
AsyncWebServer server(80);

// Membuat objek AsyncWebSocket 
AsyncWebSocket ws("/ws");

// Menyiapkan variabel-variabel sudut servo
int base_servo_angle = 0;

String message = ""; 
String sliderValue1 = "0"; 

//Membuat objek JSONVar untuk menyimpan nilai slider 
JSONVar sliderValues;

//Untuk menerima nilai slider 
String getSliderValues(){
sliderValues["sliderValue1"] = String(sliderValue1);
String jsonString = JSON.stringify(sliderValues);
return jsonString;
}


// Inisialisasi LittleFS 
void initFS(){
  if (!LittleFS.begin()){
    Serial.println("An error has occurred while mounting LittleFS");
    }
    else{
      Serial.println("LittleFS mounted successfully");
      }
  }
  
// Inisialisasi WiFi 
void initWiFi(){
WiFi.mode(WIFI_STA); 
WiFi.begin(ssid, password);
Serial.print("Connecting to WiFi ..");
}

void notifyClients(String sliderValues){
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len){
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){
    data[len] = 0;
    message = (char*)data;
    
    if (message.indexOf("1s") >= 0){
      sliderValue1 = message.substring(2); 
      base_servo_angle = sliderValue1.toInt();
      //Serial.print("\nBase Servo Angle  : ");
      //Serial.print(base_servo_angle); 
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
      }
      if (strcmp((char*)data, "getValues") == 0){
        notifyClients(getSliderValues());
        }
    }
}

//Notifikasi ketika ada client yang terhubung
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
 switch (type){
  case WS_EVT_CONNECT:
  Serial.printf("Client WebSocket ke-#%u terhubung dari %s\n", client->id(), client->remoteIP().toString().c_str());
  break;
  
  case WS_EVT_DISCONNECT:
  Serial.printf("Client WebSocket ke-#%u memutus hubungan\n", client->id()); 
  break;
  
  case WS_EVT_DATA: handleWebSocketMessage(arg, data, len);
  break;
  
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
  break;
  }
}

void initWebSocket(){
ws.onEvent(onEvent); 
server.addHandler(&ws);
}


void setup(){
Serial.begin(115200);
pinMode(wifi_no_led, OUTPUT);
base_servo.attach(base_servo_pin);

initFS(); 
initWiFi(); 
initWebSocket();

// URL Root WebServer
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
request->send(LittleFS, "/index.html", "text/html");
});

server.serveStatic("/", LittleFS, "/");

while (WiFi.status() != WL_CONNECTED){
  digitalWrite(wifi_no_led, HIGH); 
  Serial.print('.');
  delay(500);
  Serial.println(""); 
  Serial.println("WiFi Connected"); 
  Serial.print("Use This URL To Connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
//  Serial.print(WiFi.status());
  Serial.println("/");

// Start server 
server.begin();
}
}

void loop(){
base_servo.write(base_servo_angle); 
delay(15);
ws.cleanupClients();
}
