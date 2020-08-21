#include <PubSubClient.h> //libary wemos

#include <ESP8266WiFi.h>  //library mqtt
#include <Wire.h> //library bawaan
#include <Servo.h> //library motor servo
#include <DHT_U.h> //sensor suhu
#include <DHT.h>
#include "DHT.h"



const char* ssid = "lab iot on the road";
const char* password = "123abc?000";

const char* mqtt_server = "192.168.43.251"; //server
const char* mqtt_user = "adit";
const char* mqtt_password = "skripsi";




WiFiClient espClient; //bawaan mqtt
PubSubClient client(espClient);

#define DHTPIN 2 //D4
#define DHTTYPE DHT11
DHT dht (DHTPIN, DHTTYPE);
const char ledhijau = 5; //pin relay
const char ledmerah = 5; //pin relay
const char servoPin = 4;
const char ldrPin = 14;


#define LED_TOPIC     "adit/hijau" //topic kontrol alat
#define LED_TOPIC2    "adit/merah"
#define LED_TOPIC3    "adit/off"
#define SERVO_TOPIC1   "adit/servo1"
#define SERVO_TOPIC2   "adit/servo2"

Servo Servo1;

long lastMsg = 0;
char msg[20];

void receivedCallback(char* topic, byte* payload, unsigned int length) //perintah control
{
  Serial.print("Message received: ");
  Serial.println(topic);
 // Serial.print("]");
  String payloadtemp;

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if((char)payload[0] =='h'){
    digitalWrite(ledhijau, HIGH);
    digitalWrite(ledmerah, LOW);
  }if((char)payload[0] == 'm'){
    digitalWrite(ledhijau, LOW);
    digitalWrite(ledmerah, HIGH);
  }if((char)payload[0] == 'a'){
    Servo1.write(-180);
    delay(3600);
    Servo1.write(90);
    
  }if((char)payload[0] == 'b'){
    Servo1.write(180);
    delay(3600);
    Servo1.write(90);
  }


}


void mqttconnect() {

  while (!client.connected()) {
    Serial.print("MQTT connecting ...");

    String clientId = "ESP32Client";

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(LED_TOPIC);
      client.subscribe(LED_TOPIC2);
      client.subscribe(LED_TOPIC3);
      client.subscribe(SERVO_TOPIC1);
      client.subscribe(SERVO_TOPIC2);
    } else{
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

 dht.begin();
  pinMode (ldrPin,INPUT);
  pinMode(5, OUTPUT);
  pinMode(5, OUTPUT);
  Servo1.attach(servoPin);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  client.setServer(mqtt_server, 1883);

  client.setCallback(receivedCallback);
}

void loop() {

  if (!client.connected()) {
    mqttconnect();
  }

  client.loop();

   long t = dht.readTemperature(); //variabel baca temperature
  long h = dht.readHumidity(); //variabel baca kelembapan

  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Kelembapan: ");
  Serial.println(h);
   client.publish("adit/temperature", String(t).c_str(), true); //Mengirim data temperature
   client.publish("adit/humidity", String(h).c_str(), true); //Mengirim data kelembapan
   
  int ldrStatus = digitalRead(ldrPin);
  Serial.print("No: ");
  Serial.println(ldrStatus);
  client.publish("adit/cuaca", String(ldrStatus).c_str(), true);
  delay(500);

 /* if (ldrStatus == 1){
  Servo1.write(-180); //servo nyala
  delay(1800);    //kalibrasi
  Servo1.write(90); //servo mati
  }else{
    Servo1.write(180);
    delay(1800);
    Servo1.write(90); 
  } */

  

  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;

    }
  }
