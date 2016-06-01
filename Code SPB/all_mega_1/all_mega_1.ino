
#include <stdlib.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;
/* analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 50
 ** MISO - pin 51
 ** CLK - pin 52
 ** CS - pin 53

*/


int count = 0;


float sensor1State= 12.45;
float sensor2State= 45.53;
  

String state1, state2,state3,state4,state5 ,state6 ,gps_date , gps_time ;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";



/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   
*/

static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;


// this runs once
void setup() {                
 
  // enable debug serial
  Serial.begin(115200); 

  // enable GPS Software serial
  Serial2.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  // enable software serial
  Serial1.begin(115200);
  
  // reset ESP8266
  Serial1.println("AT+RST");
}


// the loop 
void loop() {
  
  //reading the state of sensor1
  //sensor1State = digitalRead(sensor1);
  //sensor2State = digitalRead(sensor2);
  //sensor3State = digitalRead(sensor3);
 // sensor4State = digitalRead(sensor4);
  
  while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
         updateInfo();
      }  
    }
    
   
  
  //Serial.println("  ------------  ");
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  
  state1=String(sensor1State);
  
  
  state2=String(sensor2State);

  
  state5 = gps_date;
  state6 = gps_time;

  writesdcard();
  
  count++;
  
  if (count == 200) {
    
    tcpconnection();;
    count = 0;
    Serial.println(gps_time);
  }
 

  delay(50);
    
    
}


void tcpconnection() {
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80"; //port no
  Serial1.println(cmd);
  Serial.println(cmd);
   
  if(Serial1.find("Error")){
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
  Serial1.println(cmd);
  Serial.println(cmd);

  if(Serial1.find(">")){
    Serial1.print(getStr);
    Serial.print(getStr);
  }
  else{
    Serial1.println("AT+CIPCLOSE");
    // alert user
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


void writesdcard() {

  String dataString = "";
  dataString += "Voltage = ";
  dataString += state1;
  dataString += "  ,  ";
  dataString += "Current = ";
  dataString += state2;
  dataString += "  ,  ";
  dataString += "Latitude = ";
  dataString += state3;
  dataString += "  ,  ";
  dataString += "Longitude = ";
  dataString += state4;
  dataString += "  ,  ";
  dataString += "Date = ";
  dataString += state5;
  dataString += "  ,  ";
  dataString += "Time = ";
  dataString += state6;
  
 
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("data1.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    
  }
  else {
    Serial.println("error opening datalog.txt");
  }
 
}

