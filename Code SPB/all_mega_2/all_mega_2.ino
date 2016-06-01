
#include <stdlib.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

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

const float V1R1 = 18;  // 18k OHM
const float V1R2 = 13;   // 16k OHM
const float V2R1 = 91;  // 18k OHM    change values accordingly
const float V2R2 = 24;   // 16k OHM
const float V3R1 = 62;  // 18k OHM
const float V3R2 = 10;   // 16k OHM
const float V4R1 = 13;  // 18k OHM
const float V4R2 = 1.5;   // 16k OHM

float V1 = 0;
float V2 = 0;
float V3 = 0;
float V4 = 0;
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
  
  
  Serial1.begin(9600);
  
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
         updateInfo();
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

  /*Serial.println (V1);
  Serial.println (V2);
  Serial.println (V3);
  Serial.println (V4);
  Serial.println (V);
  Serial.println (C);

  Serial.println (" ------- ");*/

  writesdcard();
  
  count++;
  
  if (count == 200) {
    
    tcpconnection();;
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

  int v1 = analogRead(volt1);
  int v2 = analogRead(volt2);
  int v3 = analogRead(volt3);
  int v = analogRead(volt4);
  int c = analogRead(curr);

  float v1_temp = ( v1*5.0 )/1024.0 ;
  float v2_temp = ( v2*5.0 )/1024.0 ;
  float v3_temp = ( v3*5.0 )/1024.0 ;
  float v_temp = ( v*5.0 )/1024.0 ;
  float c_temp = ( c*5.0 )/1024.0 ;

  
  V1 = v1_temp * ( V1R1 + V1R2 ) / V1R2 ;    // first battery volatge
  float V2_tp = v2_temp * ( V2R1 + V2R2 ) / V2R2 ;   //  2 batteries
  float V3_tp = v3_temp * ( V3R1 + V3R2 ) / V3R2 ;   //  3 batteries

  V = v_temp * ( V4R1 + V4R2 ) / V4R2 ;   // V total

    
  V2 = V2_tp - V1 ;
  V3 = V3_tp - V2_tp ;
  V4 = V - V3_tp ;
  
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

