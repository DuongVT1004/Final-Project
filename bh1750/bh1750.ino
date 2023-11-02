#include <Wire.h>
#include <BH1750.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

// Config WiFi and Firebase
#define WIFI_SSID "5 Nang Cong Chua"
#define WIFI_PASSWORD "06112000"
#define FIREBASE_HOST "finalproject-1c468-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KKtPJb8Hv5Y302KfKjeLzd0KPmRumgjl2msLBxnm"

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;
int x = 0;

// Led
const int led = D4;

// Pin to control direction
int in1 = D5;
int in2 = D6;

// variables to calculate PID
double dt, last_time;
double integral, previous, output = 0;
double kp, ki, kd;

//const int button = 0;
//const int pump = 15;

//int flag_pump = 0;
int flag_web;

// Bh1750 variable
BH1750 lightMeter;

void setup(){
  Serial.begin(115200);
  
  //Config 2 pin that control direction in L298
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  // kp, ki, kd coefficient in PID
  kp = 7;
  ki = 0.4;
  kd = 0.008;
  last_time = 0;

  // Connect WiFi
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
  // Initialize I2C
  Wire.begin();
  
  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}

// Function that controls PID
// Paremeter : difference between set value and real value
double pid(double error)
{
  double proportional = error; // error: giá trị chênh lệch
  integral += error * dt; // Tính toán tích phân, dt là thời gian lấy mẫu
  double derivative = (error - previous) / dt; // Tính toán vi phân, previous là giá trị error trước đó
  previous = error;
  double output = (kp * proportional) + (ki * integral) + (kd * derivative); // Tính toán pid
  return output;
}

void loop() {
  // get value that is set from Firebase
  if(Firebase.getInt(firebaseData, path + "/Lum")){
      x = firebaseData.intData();
  }

  // get real value
  float lux = lightMeter.readLightLevel();
  double now = millis();
  dt = (now - last_time)/100.00;
  last_time = now;
  double error = x - lux;

  // calcualate pid value
  output = pid(error);
  output = map(output, 0 , 1500, 0, 255);

  // control light indensity
  analogWrite(led, output); 
  Serial.println(lux);
  Firebase.setInt(firebaseData, path + "/Lum_control", (int)lux);
  delay(100);
  
}
