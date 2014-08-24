/*
  blink method blinks an LED a given number of times
 */
void blinkLed(int thisPin, int howManyTimes) {
  // set the LED's pin as an output:
  pinMode(thisPin, OUTPUT);
  // blink the LED:
  for (int t = 0; t < howManyTimes; t++) {
    digitalWrite(thisPin, LOW);
    delay(200);
    digitalWrite(thisPin, HIGH);
    delay(200);
  } 
}
