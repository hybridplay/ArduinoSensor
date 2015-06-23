
/*
 * HybridPlay v0.5 | 06/2015
 * ------------------------- 
 *  
 * Read HybridPlay sensors (Pololu AltIMU-10 board & infrared GP2Y0E02B), and
 * send the data via serial/bluetooth
 *
 * rev 05:
 * - final board, based on LilyPad Arduino USB, added led power button and speaker
 *
 * PINs Mapping:
 * 
 *  pin 11 digital --> speaker
 *  pin 10 digital --> power LED
 *  pin 0 analog --> battery
 *
 * (cc) 2015 Lalalab n3m3da
 * http://www.lalalab.org
 * http://www.d3cod3.org
 */

 
 
// External Libraries
#include "pitches.h"
#include <SI114.h>
#include <Wire.h>


// DEBUG vars
boolean debugMode = false;

////////////////////////////////////////////////
// Hardware interface
#define speaker 11
#define powerLed 10
#define batteryPin 0

long timerBattery = 0;
int batteryCapacity = 150; // mAh
int powerLedBrightness = 0;
int powerLedFade = 5;

int sensorON_melody[] = {
  NOTE_C4, NOTE_C4, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int sensorON_durations[] = {
  16, 16, 16
};

////////////////////////////////////////////////
// GP2Y0E02B IR proximity sensor Variables
#define IR_ADDRESS       0x80 >> 1  // Arduino uses 7 bit addressing so we shift address right one bit
#define IR_DISTANCE_REG  0x5E
#define IR_SHIFT         0x35

int ir_distance = 0;                // Stores the calculated distance 
byte ir_high,ir_low = 0;            // High and low byte of distance
int ir_shift = 0;                   // Value in shift bit register

////////////////////////////////////////////////
// Pololu AltIMU-10 sensor init & calibration
int SENSOR_SIGN[9] = {1,1,1,-1,-1,-1,1,1,1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer

// LSM303 accelerometer: 8 g sensitivity
// 3.9 mg/digit; 1 g = 256
#define GRAVITY 256  //this equivalent to 1G in the raw data coming from the accelerometer 

#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi

// L3G4200D gyro: 2000 dps full scale
// 70 mdps/digit; 1 dps = 0.07
#define Gyro_Gain_X 0.07 //X axis Gyro gain
#define Gyro_Gain_Y 0.07 //Y axis Gyro gain
#define Gyro_Gain_Z 0.07 //Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x)*ToRad(Gyro_Gain_X)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x)*ToRad(Gyro_Gain_Y)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x)*ToRad(Gyro_Gain_Z)) //Return the scaled ADC raw data of the gyro in radians for second

// LSM303 magnetometer calibration constants; use the Calibrate example from
// the Pololu LSM303 library to find the right values for your board

// min: -286,-223,-673 / max: 778, 864, 499
#define M_X_MIN -286
#define M_Y_MIN -223
#define M_Z_MIN -673
#define M_X_MAX 778
#define M_Y_MAX 864
#define M_Z_MAX 499

#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

#define OUTPUTMODE 1

float G_Dt=0.02; // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer=0;   //general purpuse timer
long timer_old;
long timer24=0; //Second timer used to print values 
int AN[6]; //array that stores the gyro and accelerometer data
int AN_OFFSET[6]={0,0,0,0,0,0}; //Array that stores the Offset of the sensors

int gyro_x;
int gyro_y;
int gyro_z;
int accel_x;
int accel_y;
int accel_z;
int magnetom_x;
int magnetom_y;
int magnetom_z;
float c_magnetom_x;
float c_magnetom_y;
float c_magnetom_z;
float MAG_Heading;

float Accel_Vector[3]= {0,0,0}; //Store the acceleration in a vector
float Gyro_Vector[3]= {0,0,0};//Store the gyros turn rate in a vector
float Omega_Vector[3]= {0,0,0}; //Corrected Gyro_Vector data
float Omega_P[3]= {0,0,0};//Omega Proportional correction
float Omega_I[3]= {0,0,0};//Omega Integrator
float Omega[3]= {0,0,0};

// Euler angles
float roll;
float pitch;
float yaw;

float errorRollPitch[3]= {0,0,0}; 
float errorYaw[3]= {0,0,0};

unsigned int counter=0;
byte gyro_sat=0;

float DCM_Matrix[3][3]= {
  {
    1,0,0  }
  ,{
    0,1,0  }
  ,{
    0,0,1  }
}; 
float Update_Matrix[3][3]={{0,1,2},{3,4,5},{6,7,8}}; //Gyros here


float Temporary_Matrix[3][3]={
  {
    0,0,0  }
  ,{
    0,0,0  }
  ,{
    0,0,0  }
};

// Serial packet
#define SERIAL_BAUD_VEL 115200
const char HEADER = 'H';
const char FOOTER = 'F';

int angleX = 0;
int angleY = 0;
int angleZ = 0;
int valIR = 0;
int valBattery = 0;

void setup(){ 

  // Serial
  initSerial();
  
  // Power Led Mode
  pinMode(powerLed,OUTPUT);

  // sound on start
  sensorON_play();
  
  // INIT sensor
  initSensor();

  // Battery
  readBattery();
  
  // Time
  timer = millis();
  timerBattery = millis();
  delay(20);
  counter = 0;
  
}

void loop() {
  
  if((millis()-timer) >= 20){  // Main loop runs at 50Hz (same as 50 FPS)
    counter++;
    timer_old = timer;
    timer=millis();
    
    if (timer>timer_old){
      G_Dt = (timer-timer_old)/1000.0; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
    }else{
      G_Dt = 0;
    }
    
    // *** DCM algorithm
    // Data adquisition
    Read_Gyro();   // This read gyro data
    Read_Accel();     // Read I2C accelerometer
    
    if(counter > 5){  // Read compass data at 10Hz... (5 loop runs)
      counter=0;
      Read_Compass();    // Read I2C magnetometer
      Compass_Heading(); // Calculate magnetic heading  
    }
    
    // Calculations...
    Matrix_update(); 
    Normalize();
    Drift_correction();
    Euler_angles();
    // ***

    // POWER LED
    fadeLed();

    // Battery
    if((millis()-timerBattery) >= 60000){ // read battery level every minute
      timerBattery = millis();
      readBattery();
    }
    
    // IR
    updateIR();
    
    // Prepare serial data
    prepareData();
  
    // print on usb (Serial)
    if(debugMode){
      printData();
    }

    // data send via BLUETOOTH (Serial1)
    sendSerialPacket(); // <-- binary data send
  
  }
  
}


