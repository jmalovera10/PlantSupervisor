/*----------------
   LIBRARY IMPORTS
  -----------------*/
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

/*---------------------------------
   SENSORS/ACTUATORS PIN DEFINITION
  ----------------------------------*/
#define LM35 A0
#define PT100 A1
#define LDR A2
#define BMP180_1 A3
#define BMP180_2 A4
#define YL69 A5
#define PUMP A6
#define DHT11 7

/*---------------------
   LIBRARY CONSTRUCTORS
  ----------------------*/
#define DHTTYPE DHT11
DHT dht(DHT11, DHTTYPE);
Adafruit_BMP085 bmp;

/*--------------------------
   VALUE STORAGE FOR SENSORS
  ----------------------------*/
float LM35_value;
float PT100_value;
int LDR_value;
float BMP_pressure;
float BMP_altitude;
float DHT_humidity;
float DHT_temperature;

/*
   Initial setup
*/
void setup() {
  Serial.begin(9600);

  //Default value setup
  LM35_value = 0.0;
  PT100_value = 0.0;
  LDR_value = 0;
  BMP_pressure = 0.0;
  BMP_altitude = 0.0;
  DHT_humidity = 0.0;
  DHT_temperature = 0.0;

  //Pin mode definition for sensors and actuator
  pinMode(LM35_value, INPUT);
  pinMode(PT100, INPUT);
  pinMode(LDR, INPUT);
  pinMode(BMP180_1, INPUT);
  pinMode(BMP180_2, INPUT);
  pinMode(YL69, INPUT);
  pinMode(PUMP, OUTPUT);

  //Library startup
  dht.begin();
  bmp.begin();
}

void loop() {
  //LM35 temperature read
  LM35_value = (5.0 * analogRead(LM35) * 100.0) / 1024;
  //PT100 temperature value
  PT100_value = analogRead(PT100);
  //LDR light value
  LDR_value = analogRead(LDR);
  //BMP pressure value
  BMP_pressure = bmp.readPressure();
  //BMP altitude value
  BMP_altitude = bmp.readAltitude();
  //DHT humidity value
  DHT_humidity = dht.readHumidity();
  //DHT temperature value
  DHT_temperature = dht.readTemperature();
}
