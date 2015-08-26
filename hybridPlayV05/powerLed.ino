void fadeLed(){
  analogWrite(powerLed, powerLedBrightness);    

  // change the brightness for next time through the loop:
  powerLedBrightness = powerLedBrightness + powerLedFade;

  // reverse the direction of the fading at the ends of the fade: 
  if (powerLedBrightness == 0 || powerLedBrightness == 255) {
    powerLedFade = -powerLedFade; 
  }
}

