// Made on hardware serial1 of Arduino Mega 2560
#include <stdlib.h>

//on/off sensor 1
int sensor1=2;
int sensor2=3;
int sensor3=7;
int sensor4=8;
int sensor1State=0;
int sensor2State=0;
int sensor3State=0;
int sensor4State=0;


// replace with your channel's thingspeak API key
String apiKey = "7FQST9F6CXX3TLXE";

// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB


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
  Serial1.begin(115200);
  
  // reset ESP8266
  Serial1.println("AT+RST");
}


// the loop 
void loop() {
  
  //reading the state of sensor1
  sensor1State = digitalRead(sensor1);
  sensor2State = digitalRead(sensor2);
  sensor3State = digitalRead(sensor3);
  sensor4State = digitalRead(sensor4);
  
  String state1=String(sensor1State);
  
  
  String state2=String(sensor2State);
  
  
  String state3=String(sensor3State);
  
  
  String state4=String(sensor4State);
  
  
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
  /*if(i==0)
  {
    Serial.println("1");
  getStr +="&field1=";
  getStr += String(state1);
  i++;
  }
  else if(i==1)
  {Serial.println("2");
  getStr +="&field2=";
  getStr += String(state2);
  i++;
  }
  else if(i==2)
  {
    Serial.println("3");
  getStr +="&field3=";
  getStr += String(state3);
  i++;
  }
  else if(i==3)
  {
    Serial.println("4");
  getStr +="&field4=";
  getStr += String(state4);
  i=0;
  }
  
  */
  getStr +="&field1=";
  getStr += String(state1);
  getStr +="&field2=";
  getStr += String(state2);
  getStr +="&field3=";
  getStr += String(state3);
   getStr +="&field4=";
  getStr += String(state4);
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
  delay(16000);  
}
