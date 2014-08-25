
// function to send the given integer value to the serial port
void sendBinary(int value){
  // send the two byte that comprise an integer
  Serial.write(lowByte(value));
  Serial.write(highByte(value));
}

void sendBinaryIR(int value){
  int cmIR = getCmFromValue(value);
  // send the two byte that comprise an integer
  Serial.write(lowByte(cmIR));
  Serial.write(highByte(cmIR));
}

// Standard Arduino Serial.print (for TESTING)
void sendDistanceIR(int value){
  int cmIR = getCmFromValue(value);
  
  Serial.print(",");
  if(cmIR < 100){
    Serial.print(0);
  }else{
    Serial.print(cmIR);
  }
  Serial.print(",");
}

void sendValueIR(int value){
  Serial.print(",");
  Serial.print(value);
  Serial.print(",");
}




