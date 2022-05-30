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
//Number for this node
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
MeanFilter<int> meanFilterTurbidez(15);
MeanFilter<int> meanFilterDistancia(15);
MeanFilter<int> meanFilterLluvia(15);
MeanFilter<int> meanFilterTemperatura(15);
MeanFilter<int> meanFilterHumedad(15);
//String to send to other nodes with sensor readings
String readings;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

//Create tasks: to send messages and get readings;
Task taskSendMessage(TASK_SECOND * 60 , TASK_FOREVER, &sendMessage);
String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["turbidez"] = turbidezMean;
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
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double turb = myObject["turbidez"];
  int distancia = myObject["distancia"];
  int temperatura = myObject["temperatura"];
  int humedad = myObject["humedad"];
  int lluvia = myObject["lluvia"];
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Turbidez: ");
  Serial.print(turb);
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.print("Lluvia: ");
  Serial.print(lluvia);
  Serial.println(" %");
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
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
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
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  float voltajeValue = (analogRead(voltajePin)*3.3)/4095.0;
  int turbidez = analogRead(sensorPin);
  long t; //timepo que demora en llegar el eco
  long d;
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm 
  int lluviaValue = analogRead(lluviaPin);
  int temperaturaValue = temp.temperature;
  int humedadValue = humidity.relative_humidity;
  turbidezMean = meanFilterTurbidez.AddValue(turbidez);
  distanciaMean = meanFilterDistancia.AddValue(d);  
  lluviaMean = meanFilterLluvia.AddValue(lluviaValue);
  temperaturaMean = meanFilterTemperatura.AddValue(temperaturaValue);
  humedadMean = meanFilterHumedad.AddValue(humedadValue);
  mesh.update();
}
