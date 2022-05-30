#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <ListLib.h>
#include "ThingSpeak.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
//#include <DHT.h>
#include "MeanFilterLib.h"
#include "DHT.h"
const char* ssid = "PUBLICA";   // your network SSID (name) 
const char* password = "";   // your network password
//#define DHTPIN   21
//#define DHTTYPE DHT21
// MESH Details
#define BLUE 26
#define GREEN 13
#define RED 27
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
WiFiClient  client;
#define RXD2 34
#define TXD2 35

unsigned long myChannelNumber = 1664681;
const char * myWriteAPIKey = "HVFQ8D4V9SE5P931";

unsigned long myChannelNumberD = 1684475;
const char * myWriteAPIKeyD = "BUVMMGKHIF65NMGF";

unsigned long myChannelNumberL = 1714887;
const char * myWriteAPIKeyL = "6IBO7CRME9U6JVMQ";

unsigned long myChannelNumberTemp = 1714889;
const char * myWriteAPIKeyTemp = "6HUKX0L12677FXQF";

unsigned long myChannelNumberH = 1714892;
const char * myWriteAPIKeyH = "HIAGL68M48YT52VB";

unsigned long myChannelNumberV = 1716007;
const char * myWriteAPIKeyV = "J272OA7P9VXRS5DH";
HardwareSerial Receiver(1); 
void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  Serial.begin(115200);
  ThingSpeak.begin(client);
  Receiver.begin(115200, SERIAL_8N1, RXD2, TXD2);
}
void connectWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
    Serial.println("Conectando...");
    
  }
  Serial.println("Connected to the WiFi network");
  digitalWrite(BLUE, HIGH);
  delay(1000);
  digitalWrite(BLUE, LOW);
}
String data = "";
JSONVar object = new JSONVar;
void enviar(String data){
      connectWifi();
      Serial.println(data);
      object =JSON.parse(data.c_str());
      int nodeNumber = object["node"];
      int turbidez = object["turbidez"];
      int distancia = object["distancia"];
      int temperatura = object["temperatura"];
      int humedad = object["humedad"];
      int lluvia = object["lluvia"];
      Serial.println(nodeNumber);
      Serial.println(turbidez);
      Serial.println(distancia);
      Serial.println(temperatura);
      Serial.println(humedad);
      Serial.println(lluvia);
      int x = ThingSpeak.writeField(myChannelNumber, nodeNumber, turbidez, myWriteAPIKey);
  if(x == 200){
      Serial.println("Channel update successful.");
      digitalWrite(GREEN, HIGH);
      delay(1000);
      digitalWrite(GREEN, LOW);
      Serial.println("Sent to TS");    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
    }
    delay(1000);
    int y = ThingSpeak.writeField(myChannelNumberD, nodeNumber, distancia, myWriteAPIKeyD);
    if(y == 200){
      Serial.println("Channel update successful.");
      digitalWrite(GREEN, HIGH);
      delay(1000);
      digitalWrite(GREEN, LOW);
      Serial.println("Sent to TS");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
    }
    delay(1000);
    int z = ThingSpeak.writeField(myChannelNumberL, nodeNumber, lluvia, myWriteAPIKeyL);
    if(z == 200){
      Serial.println("Channel update successful.");
      digitalWrite(GREEN, HIGH);
      delay(1000);
      digitalWrite(GREEN, LOW);
      Serial.println("Sent to TS");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
    }
    delay(1000);
    int w = ThingSpeak.writeField(myChannelNumberTemp, nodeNumber, temperatura, myWriteAPIKeyTemp);
    if(w == 200){
      Serial.println("Channel update successful.");
      digitalWrite(GREEN, HIGH);
      delay(1000);
      digitalWrite(GREEN, LOW);
      Serial.println("Sent to TS");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
    }
    delay(1000);
    int u = ThingSpeak.writeField(myChannelNumberH, nodeNumber, humedad, myWriteAPIKeyH);
    if(u == 200){
      Serial.println("Channel update successful.");
      digitalWrite(GREEN, HIGH);
      delay(1000);
      digitalWrite(GREEN, LOW);
      Serial.println("Sent to TS");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
    }
    delay(1000);
}
void loop() {
  //char bytes = Serial2.read();
   if(Receiver.available()>0){
    while (Receiver.available()>0)
   {
      data += Receiver.readString();
   }
   Receiver.read();
      enviar(data);
      data = "";
   } 
   delay(5000);
}
