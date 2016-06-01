#include <SoftwareSerial.h>
#include <stdlib.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

int count = 0;

/*
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

*/

const int chipSelect = 10;

String var1, var2,var3,var4,var5 ,var6 ,gps_date , gps_time ;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void setup() {                

  
  if (!SD.begin(chipSelect)) {
    
    return;
  }
  
  ss.begin(GPSBaud);
  Serial.begin(115200); 
  // reset ESP8266
  Serial.println("AT+RST");
}


// the loop 
void loop() {

  int volt = analogRead(0);
  int curr = analogRead(1);

  int sensor1var = map (volt ,0,1023,0,5);
  int sensor2var = map (curr ,0,1023,0,5);
  
  
  
    while(ss.available() > 0) {
      if (gps.encode(ss.read())) {
         updateInfo();
      }  
    }    
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  
  var1=String(sensor1var);
  
  
  var2=String(sensor2var);

  
  var5 = gps_date;
  var6 = gps_time;
  
  writesdcard();
  
  count++;
  
  if (count == 155) {
    
    tcpconnection();
    count = 0;
  }
 

  delay(100);
    
}


void tcpconnection() {
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80"; //port no
  
  Serial.println(cmd);
   
  if(Serial.find("Error")){
    
    return;
  }
  
  // prepare GET string
  String getStr = "GET https://api.thingspeak.com/update?api_key=7FQST9F6CXX3TLXE&field1=0";
  getStr += apiKey;
  
  getStr +="&field1=";
  getStr += String(var1);
  getStr +="&field2=";
  getStr += String(var2);
  getStr +="&field3=";
  getStr += String(var3);
  getStr +="&field4=";
  getStr += String(var4);
  getStr +="&field5=";
  getStr += String(var5);
  getStr +="&field6=";
  getStr += String(var6);
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

}


void updateInfo() {

 if (gps.location.isValid())
  {
    var3 = String(gps.location.lat(), 6);
    var4 = String(gps.location.lng(), 6);
  }
  else
  {
    var3 = "00";
    var4 = "00";
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
  dataString += var1;
  dataString += "  ,  ";
  dataString += "Current = ";
  dataString += var2;
  dataString += "  ,  ";
  dataString += "Lattitude = ";
  dataString += var3;
  dataString += "  ,  ";
  dataString += "Longitude = ";
  dataString += var4;
  dataString += "  ,  ";
  dataString += "Date = ";
  dataString += var5;
  dataString += "  ,  ";
  dataString += "Time = ";
  dataString += var6;
  
 

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog1.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    
  }

  
}

