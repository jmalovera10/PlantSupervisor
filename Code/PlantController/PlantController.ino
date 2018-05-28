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
#define DEBUG true

/*---------------------
   LIBRARY CONSTRUCTORS
  ----------------------*/
#define DHTTYPE DHT11
DHT dht(DHT11, DHTTYPE);
Adafruit_BMP085 bmp;

/*--------------------------
   VALUE STORAGE FOR SENSORS
  ----------------------------*/
float LM35_temperature;
float PT100_temperature;
int LDR_value;
float BMP_pressure;
float BMP_altitude;
int YL_soilhumidity;
float DHT_humidity;
float DHT_temperature;

/*
   Initial setup
*/
void setup() {
  Serial.begin(9600);    ///////For Serial monitor
  Serial2.begin(115200); ///////ESP Baud rate

  //Default value setup
  LM35_temperature = 0.0;
  PT100_temperature = 0.0;
  LDR_value = 0;
  BMP_pressure = 0.0;
  BMP_altitude = 0.0;
  YL_soilhumidity = 0;
  DHT_humidity = 0.0;
  DHT_temperature = 0.0;

  //Pin mode definition for sensors and actuator
  pinMode(LM35_temperature, INPUT);
  pinMode(PT100, INPUT);
  pinMode(LDR, INPUT);
  pinMode(BMP180_1, INPUT);
  pinMode(BMP180_2, INPUT);
  pinMode(YL69, INPUT);
  pinMode(PUMP, OUTPUT);

  //Library startup
  dht.begin();
  bmp.begin();

  //ESP Startup
  sendData("AT+CWSAP=\"PLANT-PLUS\",\"osmasegura\",5,3\r\n", 1000, DEBUG);
  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80
}

int connectionId;
void loop() {
  //LM35 temperature read
  LM35_temperature = (5.0 * analogRead(LM35) * 100.0) / 1024;
  //PT100 temperature value
  PT100_temperature = analogRead(PT100);
  //LDR light value
  LDR_value = analogRead(LDR);
  //BMP pressure value
  BMP_pressure = bmp.readPressure();
  //BMP altitude value
  BMP_altitude = bmp.readAltitude();
  //YL soil humidity value
  YL_soilhumidity = analogRead(YL69);
  //DHT humidity value
  DHT_humidity = dht.readHumidity();
  //DHT temperature value
  DHT_temperature = dht.readTemperature();
  
  if (Serial2.available())
  {
    /////////////////////Recieving from web browser to toggle led
    if (Serial2.find("+IPD,"))
    {
      delay(300);
      connectionId = Serial2.read() - 48;
      if (Serial2.find("pin="))
      {
        Serial.println("recieving data from web browser");
        int pinNumber = (Serial2.read() - 48) * 10;
        pinNumber += (Serial2.read() - 48);
        digitalWrite(pinNumber, !digitalRead(pinNumber));
      }

      /////////////////////Sending data to browser
      else
      {
        String webpage = "<h1>WAFFI ME LA CHUPA</h1>";
        espsend(webpage);
      }

      if (LM35_temperature != 0 && LM35_temperature < 100)
      {
        String add1 = "<h4>Temperature=</h4>";
        String two =  String(LM35_temperature, 3);
        add1 += two;
        add1 += "&#x2103"; //////////Hex code for degree celcius
        espsend(add1);
      }

      else
      {
        String c = "LM35 is not connected";
        espsend(c);
      }

      String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}

void espsend(String d)
{
  String cipSend = " AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 1000, DEBUG);
  sendData(d, 1000, DEBUG);
}
//////////////gets the data from esp and displays in serial monitor///////////////////////
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial2.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial2.available())
    {
      char c = Serial2.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response); //displays the esp response messages in arduino Serial monitor
  }
  return response;
}
