
#include <stdlib.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "28ece6d09f1c439ab5fe00842d3ceb51";


// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial3


// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);


#define VMIN  42.0     // min cutoff voltage
#define VMAX  52.0     // max cutoff voltage
#define CMAX  60.0     // max cutoff current

#define LAT_lw  12.5 
#define LAT_up  13.5
#define LNG_lw  77.0
#define LNG_up  78.0  

const int volt1 = 0;    // analog pins
const int volt2 = 1;
const int volt3 = 2;
const int volt4 = 3;
const int curr = 4;
const int relay = 8;  // digital pin to control MoSFET connected to Main Relay 
const int buzzer = 9;

const float VV1R1 = 18;  // 18k OHM
const float VV1R2 = 13;   // 16k OHM
const float VV2R1 = 91;  // 18k OHM    change values accordingly
const float VV2R2 = 24;   // 16k OHM
const float VV3R1 = 62;  // 18k OHM
const float VV3R2 = 10;   // 16k OHM
const float VV4R1 = 13;  // 18k OHM
const float VV4R2 = 1.5;   // 16k OHM

float VV1 = 0;
float VV2 = 0;
float VV3 = 0;
float VV4 = 0;
float V = 0;   // Total Voltage across battery

float C = 0.0;   // total load current

float gps_lat = 0;
float gps_lng = 0;


const int chipSelect = 53;
/* analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 50
 ** MISO - pin 51
 ** CLK - pin 52
 ** CS - pin 53

*/


int count = 0;

String state1, state2,state3,state4,state5 ,state6 ,gps_date , gps_time ;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";



/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   
*/

static const uint32_t GPSBaud = 9600;
const float offset = 5.5;     // indian time scale
time_t prevDisplay = 0;


// The TinyGPS++ object
TinyGPSPlus gps;




// this runs once
void setup() {                

  pinMode (relay, OUTPUT);
  pinMode (buzzer , OUTPUT);
  relayswitch (1);
  buzzerswitch (0);
  
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

  delay(10);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, "Harison", "8867396716");
  
  Serial1.begin(115200);
  
  // reset ESP8266
  //Serial1.println("AT+RST");
  delay(1000);
  
  Serial1.println("AT+CWJAP=\"Harison\",\"8867396716\"");
  
  Serial.println("AT+CWJAP=\"Harison\",\"8867396716\"");
  
}


// the loop 
void loop() {
  
  while(Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {

         setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
         adjustTime(offset * SECS_PER_HOUR);

         if (timeStatus()!= timeNotSet) {
            if (now() != prevDisplay) { 
              //update the display only if the time has changed
              prevDisplay = now();
              updateInfo();
              Serial.println(gps_time);
            }
         }   
      }  
    }
    
   
  
  //Serial.println("  ------------  ");
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  Read_getValues();
  
  state1=String(V);
  state2=String(C);
  state3 = String(gps_lat,6);
  state4 = String(gps_lng,6);
  state5 = gps_date;
  state6 = gps_time;

  

  Protection();
  
  Check_geofence();

  /*Serial.println (VV1);
  Serial.println (VV2);
  Serial.println (VV3);
  Serial.println (VV4);
  Serial.println (V);
  Serial.println (C);

  Serial.println (" ------- ");*/

  writesdcard();
  Blynk.run();
  
  count++;
  
  if (count == 240) {
    
    tcpconnection();
    count = 0;
    Serial.println(gps_time);
  }

  delay(25);
    
    
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
    gps_lat = gps.location.lat();
    gps_lng = gps.location.lng();
  }
  else
  {
    gps_lat = 0;
    gps_lng = 0;
  } 


  if (gps.date.isValid())
  {
    gps_date = String(day()) + "/" + String(month()) + "/" + String(year());
  }
  else
  {
    gps_date = "01/1/2000";
  }

  if (gps.time.isValid())
  {
    String time_hour = "";
    String time_min = "";
    String time_sec = "";
    
    if (hour() < 10) time_hour += "0";
    time_hour += hour();
    if (minute() < 10) time_min += "0";
    time_min += minute();
    if (second() < 10) time_sec += "0";
    time_sec += second();
    
    
    gps_time = time_hour + ":" + time_min + ":" + time_sec;
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
  File dataFile = SD.open("data2.txt", FILE_WRITE);

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

void Read_getValues() {

  int VV1 = analogRead(volt1);
  int VV2 = analogRead(volt2);
  int VV3 = analogRead(volt3);
  int v = analogRead(volt4);
  int c = analogRead(curr);

  float VV1_temp = ( VV1*5.0 )/1024.0 ;
  float VV2_temp = ( VV2*5.0 )/1024.0 ;
  float VV3_temp = ( VV3*5.0 )/1024.0 ;
  float v_temp = ( v*5.0 )/1024.0 ;
  float c_temp = ( c*5.0 )/1024.0 ;

  
  VV1 = VV1_temp * ( VV1R1 + VV1R2 ) / VV1R2 ;    // first battery volatge
  float VV2_tp = VV2_temp * ( VV2R1 + VV2R2 ) / VV2R2 ;   //  2 batteries
  float VV3_tp = VV3_temp * ( VV3R1 + VV3R2 ) / VV3R2 ;   //  3 batteries

  V = v_temp * ( VV4R1 + VV4R2 ) / VV4R2 ;   // V total

    
  VV2 = VV2_tp - VV1 ;
  VV3 = VV3_tp - VV2_tp ;
  VV4 = V - VV3_tp ;
  
  C = c_temp * 10 ; // Current sensor !! need to be checked
  
}

void Protection() {
    
  if ( V <= VMIN || V >= VMAX ) {

    relayswitch(0);
    buzzerswitch (1); 
  }

  else if ( V > VMIN || V < VMAX ) {

    relayswitch(1);
    buzzerswitch (0); 
  }

 /* if ( C >= CMAX || C == 0) {
    
    relayswitch (0);
    buzzerswitch (1);
  }

  else if ( C < CMAX ) {
    
    relayswitch (1);
    buzzerswitch (0);
  }*/

}

void Check_geofence() {
  
  if ( (gps_lat <= LAT_lw || gps_lat >= LAT_up) && (gps_lng <= LNG_lw || gps_lng >= LNG_up) ) {

    relayswitch (0);
    buzzerswitch (1);
  }
}

void relayswitch ( int t) {

  if ( t==0){
    digitalWrite(relay ,LOW);
  }
  else if ( t==1) {
    digitalWrite (relay ,HIGH);
  }  
}

void buzzerswitch ( int k) {

  if ( k==0){
    digitalWrite(buzzer ,LOW);
  }
  else if ( k== 1) {
    digitalWrite (buzzer ,HIGH);
  } 
}

