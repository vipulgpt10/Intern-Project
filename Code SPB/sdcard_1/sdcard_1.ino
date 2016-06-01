/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

*/

#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

void setup() {
  
  


  

  if (!SD.begin(chipSelect)) {
    
    return;
  }
  
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";

  
  int volt = analogRead(0);
  int curr = analogRead(1);
  
  String state1 = String(volt);
  String state2 = String(curr);
  String state3,state4,state5,state6 ;
  state3 = "00";
  state4 = "00";
  state5 = "00";
  state6 = "00";
  

  dataString += "Voltage = ";
  dataString += state1;
  dataString += "  ,  ";
  dataString += "Current = ";
  dataString += state2;
  dataString += "  ,  ";
  dataString += "Lattitude = ";
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
    dataFile.close();
    
  }
  
}
