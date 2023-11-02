#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

// Define WiFi and Firebase
#define WIFI_SSID "5 Nang Cong Chua"
#define WIFI_PASSWORD "06112000"
#define FIREBASE_HOST "finalproject-1c468-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KKtPJb8Hv5Y302KfKjeLzd0KPmRumgjl2msLBxnm"

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;

// button pin and pump pin
const int button = 14;
const int pump = 12;

// flag_pump : flag ensure that button state changes only once
int flag_pump = 0;
int i = 0, j = 0, y = 0;

// data1: use for real button, data2: use for button from web
int data1 = 0, data2 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Config WiFi and Firebase
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path)){
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
  else
  {
    Serial.println("Successfully");
  }

  // Set direction of pump pin as output and pull up resistor for button pin
  pinMode(pump, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  digitalWrite(pump, LOW);
}

//Delay 1000ms and check button
int Delay1000ms_button()
{
  for(i = 0; i < 50; i++)
  {
    delay(20);
    if(digitalRead(button) == LOW)
    {
      delay(20);
      if(flag_pump == 0 && digitalRead(button) == LOW)
      {
        flag_pump = 1;
        return 1;
      }
    }
  }
  flag_pump = 0;
  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Firebase.getInt(firebaseData, path + "/pump_web")){
      y = firebaseData.intData();
  }
  if(Delay1000ms_button() == 1)
  {
    data1 = 1 - data1;
    digitalWrite(pump, data1);
    Firebase.setInt(firebaseData, path + "/Pump", data1);
  }
  if(y == 1 && data2 == 0)
  {
    data2 = 1;
    data1 = 1;
    digitalWrite(pump, HIGH);
    Firebase.setInt(firebaseData, path + "/Pump", 1);
  }
  else if(y == 0 && data2 == 1)
  {
    data2 = 0;
    data1 = 0;
    digitalWrite(pump, LOW);
    Firebase.setInt(firebaseData, path + "/Pump", 0);
  }
}
