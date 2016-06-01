#include <SoftwareSerial.h>
#include <stdlib.h>

#include <TinyGPS++.h>



//on/off sensor 1
int sensor1=7;
int sensor2=8;

int sensor1State=0;
int sensor2State=0;



// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB
SoftwareSerial ser(10, 11); // RX, TX


static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);





String state1 , state2, state3, state4, state5 ;
int count = 0;

// this runs once
void setup() {                
  // initialize the sensor at 7th pin as an input.
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);

  
  // enable debug serial
  Serial.begin(115200); 

  ss.begin(GPSBaud);
  // enable software serial
  ser.begin(115200);
  
  // reset ESP8266
  ser.println("AT+RST");

  

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  
  
}


// the loop 
void loop() {

    while(ss.available() > 0) {
      if (gps.encode(ss.read())) {
         displayInfo();
      }  
    }
    
   
   
 
       
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  //Serial.println(" ----  ");
  
 //reading the state of sensor1
  sensor1State = digitalRead(sensor1);
  sensor2State = digitalRead(sensor2);

  String gps_date = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
  
  
   state1=String(sensor1State);
  
  
   state2=String(sensor2State);
  
 state3= "45.23";

  state4 = "71.666";

   state5 = gps_date;

  
  /*Serial.println(" -----");

  Serial.println(gps.location.lat(),6);
  Serial.println(gps.location.lng(),6);
  Serial.println(gps_date);
  Serial.println(gps.time.second());

  

  Serial.println(" -----");

  
  count++;
  if (count == 100) {
     tcpconnection();
     count = 0;
  }
  */
  //tcpconnection();
  delay(50);

}

void tcpconnection() {
 
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
  Serial.println(cmd);
   
  if(ser.find("Error")){
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
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);
  Serial.println(cmd);

  if(ser.find(">")){
    ser.print(getStr);
    Serial.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
    
  }
 
  // thingspeak needs 15 sec delay between updates
 delay(16000);  

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
