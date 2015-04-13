void initIRSensor(){
  pulse.setReg(PulsePlug::HW_KEY, 0x17);
  
  pulse.setReg(PulsePlug::INT_CFG, 0x03);       // turn on interrupts
  pulse.setReg(PulsePlug::IRQ_ENABLE, 0x10);    // turn on interrupt on PS3
  pulse.setReg(PulsePlug::IRQ_MODE2, 0x01);     // interrupt on ps3 measurement
  pulse.setReg(PulsePlug::MEAS_RATE, 0x84);     // see datasheet
  pulse.setReg(PulsePlug::ALS_RATE, 0x08);      // see datasheet
  pulse.setReg(PulsePlug::PS_RATE, 0x08);       // see datasheet
  pulse.setReg(PulsePlug::PS_LED21, 0x66 );      // LED current for LEDs 1 (red) & 2 (IR1)
  pulse.setReg(PulsePlug::PS_LED3, 0x06);       // LED current for LED 3 (IR2)
  
  pulse.writeParam(PulsePlug::PARAM_CH_LIST, 0x77);         // all measurements on

  // increasing PARAM_PS_ADC_GAIN will increase the LED on time and ADC window
  // you will see increase in brightness of visible LED's, ADC output, & noise 
  // datasheet warns not to go beyond 4 because chip or LEDs may be damaged
  pulse.writeParam(PulsePlug::PARAM_PS_ADC_GAIN, 0x00);

  pulse.writeParam(PulsePlug::PARAM_PSLED12_SELECT, 0x21);  // select LEDs on for readings see datasheet
  pulse.writeParam(PulsePlug::PARAM_PSLED3_SELECT, 0x04);   //  3 only
  pulse.writeParam(PulsePlug::PARAM_PS1_ADCMUX, 0x03);      // PS1 photodiode select
  pulse.writeParam(PulsePlug::PARAM_PS2_ADCMUX, 0x03);      // PS2 photodiode select
  pulse.writeParam(PulsePlug::PARAM_PS3_ADCMUX, 0x03);      // PS3 photodiode select  

  pulse.writeParam(PulsePlug::PARAM_PS_ADC_COUNTER, B01110000);    // B01110000 is default                                   
  pulse.setReg(PulsePlug::COMMAND, PulsePlug::PSALS_AUTO_Cmd);     // starts an autonomous read loop
  
}

void resetIRReadings(){
  totalIR = 0;
  counterIR = 0;
  red = 0;
  IR1 = 0;
  IR2 = 0;
}

void readIRSensor(){
  
  while (counterIR < samples){
    pulse.fetchLedData();

    red += pulse.ps1;
    IR1 += pulse.ps2;
    IR2 += pulse.ps3;
    
    counterIR++;
  }
  
}

void extractIRReadings(){
  red = red / counterIR;
  IR1 = IR1 / counterIR;
  IR2 = IR2 / counterIR;
  totalIR = red + IR1 + IR2;
  
  minIR = min(minIR,totalIR);
  maxIR = max(maxIR,totalIR);
  
  mappedIR = map(totalIR,minIR,maxIR,0,1000);
}

float smooth(float data, float filterVal, float smoothedVal){
    if(filterVal > 1){      // check to make sure param's are within range
        filterVal = .999;
    }else if (filterVal <= 0.0){
        filterVal = 0.001;
    }

    smoothedVal = (data * (1.0 - filterVal)) + (smoothedVal  *  filterVal);
    return smoothedVal;
}
