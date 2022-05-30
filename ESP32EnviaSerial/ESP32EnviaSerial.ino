#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <ListLib.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "MeanFilterLib.h"
#include "DHT.h"
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
// MESH Details
#define   MESH_PREFIX     "WSN" //name for your MESH
#define   MESH_PASSWORD   "password" //password for your MESH
#define   MESH_PORT       5555 //default port
#define RXD2 25
#define TXD2 26

int nodeNumber = 1;
int sensorPin = 35;
const int Trigger = 5;   //Pin digital 2 para el Trigger del sensor
const int Echo = 18;   //Pin digital 3 para el Echo del sensor
const int lluviaPin = 33;
int voltajePin = 34;
int turbidezMean;
int distanciaMean;
int lluviaMean;
int humedadMean;
int temperaturaMean;
//String to send to other nodes with sensor readings
String readings;
JSONVar myObject = new JSONVar;
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
HardwareSerial Sender(1); 
// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

//Create tasks: to send messages and get readings;
Task taskSendMessage(TASK_SECOND * 60 , TASK_FOREVER, &sendMessage);

String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["turbidez"] = analogRead(sensorPin);
  jsonReadings["distancia"] = distanciaMean;
  jsonReadings["temperatura"] = temperaturaMean;
  jsonReadings["humedad"] = humedadMean;
  jsonReadings["lluvia"] = lluviaMean;
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}
// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  myObject =JSON.parse(msg.c_str());
  int node = myObject["node"];
  int turbidez = myObject["turbidez"];
  int distancia = myObject["distancia"];
  int temperatura = myObject["temperatura"];
  int humedad = myObject["humedad"];
  int lluvia = myObject["lluvia"];
  Serial.println(node);
  Serial.println(turbidez);
  Serial.println(distancia);
  Serial.println(temperatura);
  Serial.println(humedad);
  Serial.println(lluvia);
  for (int i = 0; i < msg.length(); i++)
  {
    Sender.write(msg[i]); // Empuje cada carácter 1 por 1 en cada pase de bucle
  }
  Serial.print("Enviado");
}


void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  Sender.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd está en pin: "+String(TX));
  Serial.println("Serial Rxd está en pin: "+String(RX));
  

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}
void loop() {
  mesh.update();
}
