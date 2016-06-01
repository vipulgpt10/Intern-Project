/* The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

*/

#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <stdlib.h>


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

int count =0;
const int chipSelect = 10;

static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;



String state1,state2,state3,state4,state5 ,state6 ;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void setup() {
 
  Serial.begin(115200); 

  // reset ESP8266
  Serial.println("AT+RST");

  pinMode(2,INPUT);


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    
    // don't do anything more:
    return;
  }
  

 
  ss.begin(GPSBaud);

}

void loop() {
  
   while (ss.available() > 0)
    if (gps.encode(ss.read()))
      updateInfo();

  //Serial.println("  ------------  ");
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }


  
  float sensor1State= 12.45;
  float sensor2State= 45.53;
  
  state1=String(sensor1State);
  
  state2=String(sensor2State);

  
  state5 = state5;
  state6 = state6;
  Serial.println(state6);

 writesdcard();

 count++;
  
  if (count == 110) {
    
    //tcpconnection();
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

  delay(50);
  int i;
  for( i=0 ; i<4 ; i++) {

   pinMode(2,OUTPUT);
   digitalWrite(2,LOW);
   delay(30);
   pinMode(2,INPUT);
   delay(30);
   
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
  File dataFile = SD.open("datalog1.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.println(" ");
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    Serial.println(" ");
    
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
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
    state5 = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
  }
  else
  {
    state5 = "00/00/0000";
  }

  if (gps.time.isValid())
  {
    state6 = String (gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
  }
  else
  {
    state6 = "00:00:00";
  }
    
  
}





