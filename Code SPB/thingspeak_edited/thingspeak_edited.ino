
#include <SoftwareSerial.h>
#include <stdlib.h>

//on/off sensor 1
int sensor1=4;
int sensor2=7;
int sensor3=8;
int sensor4=9;
float sensor1State=12.456555;
float sensor2State=45.534555;
int sensor3State=0;
int sensor4State=0;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB
SoftwareSerial ser(10, 11); // RX, TX

int i=1;


// this runs once
void setup() {                
  // initialize the sensor at 7th pin as an input.
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  pinMode(sensor3,INPUT);
  pinMode(sensor4,INPUT);
  
  // enable debug serial
  Serial.begin(115200); 
  // enable software serial
  ser.begin(115200);
  
  // reset ESP8266
  ser.println("AT+RST");
}


// the loop 
void loop() {
  
  //reading the state of sensor1
  //sensor1State = digitalRead(sensor1);
  //sensor2State = digitalRead(sensor2);
  //sensor3State = digitalRead(sensor3);
 // sensor4State = digitalRead(sensor4);
  
  String state1=String(sensor1State);
  
  
  String state2=String(sensor2State);
  
  
  String state3= "15/2/2016";
  
  String state4= "45";
  
  String state5= "15:15:06:00" ;

  
  
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80"; //port no
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
