const int espinput = 9;
const int relayout = 8;


void setup() {

  pinMode (espinput,INPUT);
  pinMode (relayout,OUTPUT);
  delay(10);
  digitalWrite(relayout ,HIGH);
  Serial.begin(9600);

}

void loop() {
  
  relayfet();
  delay(50);
}

void relayfet() {

  int rin = digitalRead(espinput);
  digitalWrite(relayout , rin);
  Serial.println(rin);
}

