//#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <ArduinoJson.h>

// WiFi and Firebase
#define WIFI_SSID "5 Nang Cong Chua"
#define WIFI_PASSWORD "06112000"
#define FIREBASE_HOST "finalproject-1c468-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KKtPJb8Hv5Y302KfKjeLzd0KPmRumgjl2msLBxnm"

// Define ss pin, rst pin and dio0 pin of LoRa SX1278
#define ss 15
#define rst 16
#define dio0 4

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;
uint8_t flag[4];
uint8_t i = 0;

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
  while(!Serial);
  Serial.println("LoRa receiver:");

  // Set LoRa pin and check LoRa is valid or invalid
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)){
    Serial.println("Starting LoRa failed!");
    while(1);
  }
  else{
    Serial.println("LoRa Ready");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet: ");

    // read packet
    while (LoRa.available()) {
      if(i == 4)
      {
        i = 0;
      }

      // Read data form LoRa
      flag[i] = (uint8_t)LoRa.read();
      Serial.print(flag[i]);
      Serial.print(" ");
      i++;
    }
    Serial.println();

    // Take data that are read from LoRa to Firebase
    Firebase.setInt(firebaseData, path + "/thief", flag[0]);
    Firebase.setInt(firebaseData, path + "/light", flag[1]);
    Firebase.setInt(firebaseData, path + "/Humidity", flag[2]);
    Firebase.setInt(firebaseData, path + "/Temperature", flag[3]);
  }
}
