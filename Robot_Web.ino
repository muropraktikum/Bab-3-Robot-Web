#include<ESP8266WiFi.h> 
#include<ESPAsyncTCP.h> 
#include<ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h> 
#include <Servo.h>

#define base_servo_pin D3
#define wifi_no_led D8

Servo base_servo;

// Deklarasikan internet dengan variabel ssid dan password
..........

// Membuat objek AsyncWebServer di port 80
AsyncWebServer server(80);

// Membuat objek AsyncWebSocket 
AsyncWebSocket ws("/ws");

// Membuat variabel base_servo_angle dengan tipe data integer
...........

String message = ""; 
String sliderValue1 = "0"; 

//Membuat objek JSONVar untuk menyimpan nilai slider 
JSONVar sliderValues;

//Membuat Untuk menerima nilai slider 
String getSliderValues(){
.........
String jsonString = JSON.stringify(sliderValues);
.........
.........


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
.........
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

// Membuat kondisi pertukaran data dari klien ke server jika base servo angle >=0
    if (message.indexOf("1s") >= 0){
      sliderValue1 = message.substring(2); 
      .........
      .........
      notifyClients(getSliderValues());
      .........
      if (strcmp((char*)data, "getValues") == 0){
        notifyClients(getSliderValues());
        }
    }
}

//Notifikasi ketika ada client yang terhubung
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
 
//membuat kondisi switch case untuk terhubung dan terputus dengan klien
 switch (type){ 
  ...........
  Serial.printf("Client WebSocket ke-#%u terhubung dari %s\n", client->id(), client->remoteIP().toString().c_str());
  break;
  
  ...........
  Serial.printf("Client WebSocket ke-#%u memutus hubungan\n", client->id()); 
  break;
  
  case WS_EVT_DATA: handleWebSocketMessage(arg, data, len);
  ............
  
  ............
  ............
  break;
  }
}

void initWebSocket(){
  ws.onEvent(onEvent); 
  server.addHandler(&ws);
}


void setup(){
//Membuat baud rate 115200
  ........
  pinMode(wifi_no_led, OUTPUT);
  base_servo.attach(base_servo_pin);

// Memanggil fungsi WiFi
  .........
  .........
  .........

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
//  println WiFi Connected
  ...........
  Serial.print("Use This URL To Connect: ");
  Serial.print("http://");
//  print Wifi local IP
  ...........
  Serial.println("/");

// Start server 
  ..........
  }
  ...........

void loop(){
  base_servo.write(base_servo_angle); 
//  membuat delay setengah detik
  ........
  ws.cleanupClients();
}
