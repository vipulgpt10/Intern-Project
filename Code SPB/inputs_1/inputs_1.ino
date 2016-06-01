
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

float V1 = 0;
float V2 = 0;
float V3 = 0;
float V4 = 0;
float V = 0;   // Total Voltage across battery

float C = 0.0;   // total load current

float gps_lat = 12.9;
float gps_lng = 77.7;


void setup() {
  
  pinMode (relay, OUTPUT);
  pinMode (buzzer , OUTPUT);
  relayswitch (1);
  buzzerswitch (0);
  Serial.begin(9600);

}

void loop() {

  Read_getValues();

  Protection();
  
  Check_geofence();

  Serial.println (V1);
  Serial.println (V2);
  Serial.println (V3);
  Serial.println (V4);
  Serial.println (V);

  Serial.println (" ------- ");

  delay(500);
  
  
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

  
  V1 = v1_temp * ( V1R1 + V1R2 ) / V1R2 ;    // first battery volatge
  float V2_tp = v2_temp * ( V2R1 + V2R2 ) / V2R2 ;   //  2 batteries
  float V3_tp = v3_temp * ( V3R1 + V3R2 ) / V3R2 ;   //  3 batteries

  V = v_temp * ( V4R1 + V4R2 ) / V4R2 ;   // V total

  Serial.println(V2_tp);
  Serial.println(V3_tp);
  Serial.println(V);
  Serial.println("---");
  V2 = V2_tp - V1 ;
  V3 = V3_tp - V2_tp ;
  V4 = V - V3_tp ;
  
  C = c_temp * 100.0 ; // Current sensor !! need to be checked
  
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

