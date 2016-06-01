void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  

}

void loop() {
  // put your main code here, to run repeatedly:
int curr = analogRead(4);
//Serial.println (curr);
float curr_v = curr / 1023.0 * 5.0 ;
float curr_f = (curr_v - 2.48778)/0.02 ;
Serial.print ("Current = ");
Serial.println (curr_f,2);
delay (500);
}
