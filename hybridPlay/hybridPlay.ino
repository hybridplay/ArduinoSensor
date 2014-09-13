
/*
 * HybridPlay v0.2 | 07/2014
 * ------------------------- 
 *  
 * Read HybridPlay sensors (Accelerometer & infrared), and
 * send the data via serial/bluetooth
 *
 * rev 2:
 * - sends analog pins data as binary data
 * - filtering sensor data with simple 1D kalman filter
 *
 * (cc) 2014 Lalalab n3m3da
 * http://www.lalalab.org
 * http://www.d3cod3.org
 */
 
#include "Kalman.h"

const char HEADER = 'H';
const char FOOTER = 'F';

int valX = 0;
int valY = 0;
int valZ = 0;
int valBat = 0;
int valIR = 0;

int xbee = 4;
int speakerOut = 9;

Kalman kalmanX;
Kalman kalmanY;
Kalman kalmanZ;
Kalman kalmanIR;

void setup(){ 
  Serial.begin(115200);
  
  pinMode(speakerOut, OUTPUT); 
  pinMode(xbee, OUTPUT);
  
  // XBEE pull up
  digitalWrite(xbee, HIGH); 
  
  // Wait for sensor to stabilize
  delay(300);
  
  // capture initial sansors values
  kalmanX.init(analogRead(0));
  kalmanY.init(analogRead(1));
  kalmanZ.init(analogRead(2));
  kalmanIR.init(analogRead(4));

  // device initialized
  blinkLed(13,3);
  pitidoPositivo();
  
}
 
void loop() {
 
  Serial.write(HEADER); // 1 BYTE
  valX    = (int)kalmanX.update(analogRead(0));
  sendBinary(valX);   // 2 BYTES
  
  valY    = (int)kalmanY.update(analogRead(1));
  sendBinary(valY);   // 2 BYTES
  
  valZ    = (int)kalmanZ.update(analogRead(2));
  sendBinary(valZ);   // 2 BYTES
  
  valIR   = (int)kalmanIR.update(analogRead(4)); // infrarrojo
  sendBinary(valIR); // 2 BYTES
  
  valBat  = (int)analogRead(3); // Bateria
  sendBinary(valBat);  // 2 BYTES
  
  Serial.write(FOOTER); // 1 BYTE
  
  delay(20);
  
  // total message 12 BYTES long ==> 48 bits
  
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
    * We have to wait for a message of 11 BYTES to arrive,
    * and start the reading from the HEADER
    *
    * then we read the 5 analog values (2 BYTES each), re-converting binary to integer,
    * where:
    *
    * finalValue = least significant byte + most significant byte*256
   */
  
}



