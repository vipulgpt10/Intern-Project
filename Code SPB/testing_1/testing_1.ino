
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <TinyGPS++.h>


//on/off sensor 1
int sensor1=4;
int sensor2=7;

float sensor1State=12.456555;
float sensor2State=45.534555;


String state1, state2,state3,state4,state5 ,state6 ,gps_date , gps_time ;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB



/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx of arduino) and 3(tx of arduino).
*/
static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);



// this runs once
void setup() {                
  // initialize the sensor at 7th pin as an input.
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
 


  
  // enable debug serial
  Serial.begin(115200); 

  // enable GPS Software serial
  ss.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
 
  // reset ESP8266
  Serial.println("AT+RST");
}


// the loop 
void loop() {
  
  //reading the state of sensor1
  //sensor1State = digitalRead(sensor1);
  //sensor2State = digitalRead(sensor2);
  //sensor3State = digitalRead(sensor3);
 // sensor4State = digitalRead(sensor4);
  
  while(ss.available() > 0) {
      if (gps.encode(ss.read())) {
         updateInfo();
      }  
    }
    
   
  
  //Serial.println("  ------------  ");
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  
  
  String state1=String(sensor1State);
  
  
  String state2=String(sensor2State);

  state3 = 10;
  state4 = 20;
  state5 = 50;
  state6 = 60;
  
  
  
  tcpconnection();

  delay(16000);
    
}


void tcpconnection() {
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80"; //port no
  
  Serial.println(cmd);
   
  if(Serial.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET https://api.thingspeak.com/update?api_key=7FQST9F6CXX3TLXE&field1=0";
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(state1);
  getStr +="&field2=";
  getStr += String(state2);
  getStr +="&field3=";
  getStr += String(state3);
  getStr +="&field4=";
  getStr += String(state4);
  getStr +="&field5=";
  getStr += String(state5);
  getStr +="&field6=";
  getStr += String(state6);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  
  Serial.println(cmd);

  if(Serial.find(">")){
    
    Serial.print(getStr);
  }
  else{
    
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  //delay(16000);
}


void updateInfo() {

 if (gps.location.isValid())
  {
    state3 = String(gps.location.lat(), 6);
    state4 = String(gps.location.lng(), 6);
  }
  else
  {
    state3 = "00";
    state4 = "00";
  } 


  if (gps.date.isValid())
  {
    gps_date = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
  }
  else
  {
    gps_date = "00/00/0000";
  }

  if (gps.time.isValid())
  {
    gps_time = String (gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  }
  else
  {
    gps_time = "00:00:00";
  }
    
  
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}


