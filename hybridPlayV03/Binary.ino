// function to send the given integer value to the serial port
void sendBinaryInt(int value){
  // send the two byte that comprise an integer
  Serial.write(lowByte(value));
  Serial.write(highByte(value));
}

// FOR TESTING
void printData(){
  Serial.print("ANG:");
  Serial.print(angleX);
  Serial.print(",");
  Serial.print(angleY);
  Serial.print(",");
  Serial.print(angleZ);
  Serial.print(", IR ");
  Serial.print(valIR);
  Serial.println(); 
}

void prepareData(){
  angleX = int(ToDeg(roll));
  angleY = int(ToDeg(pitch));
  angleZ = int(ToDeg(yaw));
  valIR  = int(mappedIR);
}

void sendSerialPacket(){
  Serial.write(HEADER);    // 1 BYTE
  
  sendBinaryInt(angleX);   // 2 BYTES
  
  sendBinaryInt(angleY);   // 2 BYTES
  
  sendBinaryInt(angleZ);   // 2 BYTES
  
  sendBinaryInt(valIR);  // 2 BYTES
  
  Serial.write(FOOTER);    // 1 BYTE
  
  Serial.flush();
}


 // total message 10 BYTES long ==> 40 bits
  
  /* Example message:
   *
   * 72 -- the character 'H', the HEADER
   * 
   * Two BYTES for each analog pin:
   * 0     -- pin 0 has an integer value of 0 so this is sent as two bytes
   * 100   -- pin 1 has a value of 100, sent as a byte of 100 and a byte of 0
   * 500   -- pin 2 has a value of 500, the reminder of 500 divided by 256 is 244,
   *          the number of times 500 can be divided by 256 is 1, so here
   *          we have the first byte of 244 and the second byte of 1 
   *
   * 70 -- the character 'F', the FOOTER
   */
   
   /* RECEIVING (on the other side)
    * 
    * We have to wait for a message of 10 BYTES to arrive,
    * and start the reading from the HEADER
    *
    * then we read the 4 analog values (2 BYTES each), re-converting binary to integer,
    * where:
    *
    * finalValue = least significant byte + most significant byte*256
   */
