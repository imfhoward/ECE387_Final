#include <LiquidCrystal.h>
#include <Air_Quality_Sensor.h>
#include "Seeed_BME280.h"
#include <Wire.h>
#include <SoftwareSerial.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
double tem=0,hum=0;
String aq,str,data;
char chrstr[30];
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial ArduinoMega(26,27);

BME280 bme280;
AirQualitySensor sensor(A0);

void setup() {
  pinMode(53,OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(115200);
  ArduinoMega.begin(9600);
  while (!Serial){
  lcd.println("Waiting sensor to init...");
  delay(10000);
  lcd.clear();
  }
  while(!bme280.init()){
    lcd.println("Barometer error!");
    delay(10000);
    lcd.clear();
  }
  
  lcd.print("System activated!\nWaiting for signal");
  delay(3000);
  lcd.clear();
}

void loop() {
  if(analogRead(A15)>0){
    digitalWrite(53,LOW);
  }
  else{
    digitalWrite(53,HIGH);
  }
  lcd.display();
  int quality = sensor.slope();
  if (quality == AirQualitySensor::HIGH_POLLUTION) {
    aq = "High pollution!";
  }
  else if (quality == AirQualitySensor::LOW_POLLUTION) {
    aq = "Low pollution!";
  }
  else if (quality == AirQualitySensor::FRESH_AIR) {
    aq = "Fresh air";
  }
  lcd.print(aq);
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  tem = bme280.getTemperature();
  lcd.print(tem,1);
  hum = bme280.getHumidity();
  lcd.print(" Hum:");
  lcd.print(hum,1);
  str = " "+aq+" Tempp"+tem+" Humm"+hum+" ";
  //str.toCharArray(chrstr, 30);
  //ArduinoMega.write(chrstr);
  //Serial.write(chrstr);
  ArduinoMega.println(str);
  delay(1000);
  lcd.clear();
}
