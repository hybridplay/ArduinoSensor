// TONES  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
// Define a special note, 'R', to represent a rest
#define  R     0

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
int melodyStart[] = {  C,  b,  g, C,  R};
int beatsStart[]  = { 16, 16, 16, 8, 32};

int melodyStop[] = {  b,   e};
int beatsStop[]  = {  8,  16};

int melodyNo[] = {  c };
int beatsNo[]  = {  32};

int MAX_COUNT1 = sizeof(melodyStart)/2; // Melody length, for looping.
int MAX_COUNT2 = sizeof(melodyStop)/2; // Melody length, for looping.
int MAX_COUNT3 = sizeof(melodyNo)/2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone1 = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone1 > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone1 / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone1 / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone1);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

void pitidoPositivo() {
  
   for (int i=0; i<MAX_COUNT1; i++) {
    tone1 = melodyStart[i];
    beat = beatsStart[i];
                
    duration = beat * tempo; // Set up timing
    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
    }
}

void pitidoParada(){
            
  for (int i=0; i<MAX_COUNT2; i++) {
    tone1 = melodyStop[i];
    beat = beatsStop[i];
        
    duration = beat * tempo; // Set up timing
    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
    }
           
}

void pitidoNegativo() {
  
  for (int i=0; i<MAX_COUNT3; i++) {
    tone1 = melodyNo[i];
    beat = beatsNo[i];
        
    duration = beat * tempo; // Set up timing
    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
    }
}
