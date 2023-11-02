#include <Servo.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "HX711.h"

// WiFi and Firebase values
#define WIFI_SSID "5 Nang Cong Chua"
#define WIFI_PASSWORD "06112000"
#define FIREBASE_HOST "finalproject-1c468-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KKtPJb8Hv5Y302KfKjeLzd0KPmRumgjl2msLBxnm"


FirebaseData firebaseData;
String path = "/";
FirebaseJson json;

// servo variable
Servo s1;

// Clock pin and data pin of HX711
const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 12;

// HX711 variable
HX711 scale;
long scaleData = 0;
float x;
int i = 0;
int j = 0;
  //mySerial.begin(9600);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //Config WiFi and Firebase
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

  // Start loadcell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // Calibrate loadcell
  scale.set_scale(479);
  scale.tare();
  Serial.print("read: \t\t");
  Serial.println(scale.read());               
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5)); 
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);
  // Start servo
  s1.attach(D7);
  // Set value feed weight to 0 in firebase
  Firebase.setInt(firebaseData, path + "/feed_web", 0);
}

void readScale(){
  // Calculate loadcell value
  Serial.print("one reading:\t");
  Serial.print(scale.get_units());
  Serial.print("\t| average:\t");
  scaleData = scale.get_units(5);
  Serial.println(scaleData);

  // Get set value from firbase
  if(Firebase.getInt(firebaseData, path + "/feed_web")){
      x = (float)firebaseData.intData();
  }
  Serial.println(x);

  // Control servo
  // open if set value isn't the same as 0 and i is equal to 0
  if(x != 0 && i == 0){
    s1.write(0);
    delay(70);
    s1.write(30);
    delay(70);
    s1.write(60);
    delay(70);
    s1.write(90);
    delay(70);
    s1.write(120);
    delay(70);
    s1.write(150);
    delay(70);
    s1.write(180);
    i = 1;
  }

  // close if real value is greater than or equal to set value and i is equal to 1
  if(scaleData >= x && i == 1){
    s1.write(180);
    delay(70);
    s1.write(150);
    delay(70);
    s1.write(120);
    delay(70);
    s1.write(90);
    delay(70);
    s1.write(60);
    delay(70);
    s1.write(30);
    delay(70);
    s1.write(0);
    Firebase.setInt(firebaseData, path + "/feed_web", 0);
    i = 0;
  } 
  scale.power_down();
  delay(300);
  scale.power_up();
}


void loop() {
  // put your main code here, to run repeatedly:
  readScale();
}
