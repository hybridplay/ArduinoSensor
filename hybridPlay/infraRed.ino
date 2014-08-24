const int TABLE_ENTRIES = 12;
const int firstElement = 250; // first entry is 250 mV
const int INTERVAL  = 250; // millivolts between each element
static int distance[TABLE_ENTRIES] = {150,140,130,100,60,50,40,35,30,25,20,15};
const long referenceMv = 5000;

int getDistance(int mV){
   if( mV >  INTERVAL * TABLE_ENTRIES-1 ){
      return distance[TABLE_ENTRIES-1];
   }else{
      int index = mV / INTERVAL;
      float frac = (mV % 250) / (float)INTERVAL;
      return distance[index] - ((distance[index] - distance[index+1]) * frac);
   }
}

int getCmFromValue(int value){
  int mV = (value * referenceMv) / 1023;
  int cmIR = getDistance(mV);
  
  return cmIR;
}
