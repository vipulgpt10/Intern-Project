#include <TimerOne.h>

const int led = 13;
int countsec = 0;  

void setup(void)
{
  pinMode(led, OUTPUT);
  Timer1.initialize(4000000);    // 4 seconds
  Timer1.attachInterrupt(newfunct); // blinkLED to run every 0.15 seconds
  Serial.begin(9600);
}

int ledState = LOW;
volatile unsigned long blinkCount = 0; // use volatile for shared variables


void newfunct() {
  countsec ++ ;
  if ( countsec == 4 ) {
    if (ledState == LOW) {
    ledState = HIGH;
    blinkCount++;
  } 
  else {
    ledState = LOW;
  }
  digitalWrite(led, ledState);
  countsec = 0;
  }
  Serial.println("functioncall");
}

void loop(void)
{
  unsigned long blinkCopy;  // holds a copy of the blinkCount

  noInterrupts();
  blinkCopy = blinkCount;
  interrupts();

  Serial.print("blinkCount = ");
  Serial.println(blinkCopy);
  delay(100);
}
