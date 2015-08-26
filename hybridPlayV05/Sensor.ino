void initSensor(){
  I2C_Init();
  
  digitalWrite(powerLed,LOW);
  delay(1500);
 
  Accel_Init();
  Compass_Init();
  Gyro_Init();
  
  delay(20);
  
  for(int i=0;i<32;i++)    // We take some readings...
    {
    Read_Gyro();
    Read_Accel();
    for(int y=0; y<6; y++)   // Cumulate values
      AN_OFFSET[y] += AN[y];
    delay(20);
    }
    
  for(int y=0; y<6; y++)
    AN_OFFSET[y] = AN_OFFSET[y]/32;
    
  AN_OFFSET[5]-=GRAVITY*SENSOR_SIGN[5];
  
  delay(2000);
  
  // IR init
  // Read the shift bit register from the module, used in calculating range
  Wire.beginTransmission(IR_ADDRESS);    
  Wire.write(IR_SHIFT);
  Wire.endTransmission();
  
  Wire.requestFrom(IR_ADDRESS, 1);
  while(Wire.available() == 0);
  ir_shift = Wire.read();
  
}

void updateIR(){
  // Request and read the 2 address bytes from the GP2Y0E02B
  Wire.beginTransmission(IR_ADDRESS);
  Wire.write(IR_DISTANCE_REG);
  Wire.endTransmission();
  
  Wire.requestFrom(IR_ADDRESS, 2);
  
  while(Wire.available() < 2);
  
  ir_high = Wire.read();
  ir_low = Wire.read();
  
  ir_distance = (ir_high * 16 + ir_low)/16/(int)pow(2,ir_shift); // Calculate the range in CM
  
}
