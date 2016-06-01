
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
const int nval = 100;


const float V1R1 = 12;  // 18k OHM
const float V1R2 = 8.2;   // 16k OHM
const float V2R1 = 18;  // 18k OHM    change values accordingly
const float V2R2 = 4.7;   // 16k OHM
const float V3R1 = 56;  // 18k OHM
const float V3R2 = 8.2;   // 16k OHM
const float V4R1 = 18;  // 18k OHM
const float V4R2 = 1.8;   // 16k OHM

float V_1 = 0;
float V_2 = 0;
float V_3 = 0;
float V_4 = 0;
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

String state1, state2,state3,state4,state5 ,state6, state7, state8 ,gps_date , gps_time ;


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

  Blynk.begin(auth, wifi, "Vipul", "vipul123");
  
  Serial1.begin(115200);
  
  // reset ESP8266
  //Serial1.println("AT+RST");
  delay(1000);
  
  Serial1.println("AT+CWJAP=\"Vipul\",\"vipul123\"");
  
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
  state2=String(V_1);
  state3=String(V_2);
  state4=String(V_3);
  state5=String(V_4);
  
  state6=String(C);
  state7 = String(gps_lat,4)+ "," +String(gps_lng,4);
  state8 = gps_date + "," + gps_time ;

  

  Protection();
  
  Check_geofence();

  writesdcard();
  Blynk.run();
  
  count++;
  
  if (count == 600) {
    
    tcpconnection();
    count = 0;
  }

  delay(10);
    
    
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
  getStr +="&field7=";
  getStr += String(state7);
  getStr +="&field8=";
  getStr += String(state8);
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
  dataString += "Total Voltage = ";
  dataString += state1;
  dataString += "  ,  ";
  dataString += "Bat1 = ";
  dataString += state2;
  dataString += "  ,  ";
  dataString += "Bat2 = ";
  dataString += state3;
  dataString += "  ,  ";
  dataString += "Bat3 = ";
  dataString += state4;
  dataString += "  ,  ";
  dataString += "Bat4 = ";
  dataString += state5;
  dataString += "  ,  ";
  dataString += "Current = ";
  dataString += state6;
  dataString += "  ,  ";
  dataString += "Lat/Lng = ";
  dataString += state7;
  dataString += "  ,  ";
  dataString += "Date = ";
  dataString += gps_date;
  dataString += "  ,  ";
  dataString += "Time = ";
  dataString += gps_time;
  
 
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("data4.txt", FILE_WRITE);

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

    long v1=0 , v2=0 , v3=0, v=0, c=0;
  
  for ( int k =0; k< nval ; k++) {
   v1 += analogRead(volt1);
   v2 += analogRead(volt2);
   v3 += analogRead(volt3);
   v += analogRead(volt4);
   c += analogRead(curr);
  }
  
  v1 = v1/nval ;
  v2 = v2/nval ;
  v3 = v3/nval ;
  v = v/nval ;
  c = c/nval;

  float v1_temp = ( v1*5.32 )/1024.0 ;
  float v2_temp = ( v2*5.21 )/1024.0 ;
  float v3_temp = ( v3*4.81 )/1024.0 ;
  float v_temp = ( v*4.82 )/1024.0 ;
  float c_temp = ( c*5.0 )/1024.0 ;
  
  V_1 = v1_temp * ( V1R1 + V1R2 ) / V1R2 ;    // first battery volatge
  float V_2_tp = v2_temp * ( V2R1 + V2R2 ) / V2R2 ;   //  2 batteries
  float V_3_tp = v3_temp * ( V3R1 + V3R2 ) / V3R2 ;   //  3 batteries

  V = v_temp * ( V4R1 + V4R2 ) / V4R2 ;   // V total

    
  V_2 = V_2_tp - V_1 ;
  V_3 = V_3_tp - V_2_tp ;
  V_4 = V - V_3_tp ;
  
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


