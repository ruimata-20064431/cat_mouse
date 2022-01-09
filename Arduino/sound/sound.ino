// example    https://www.youtube.com/watch?v=KaN7Y4XxHhc
// datatypes https://learn.sparkfun.com/tutorials/data-types-in-arduino/defining-data-types
// simulated floats are 2 decimal precision floats x 100 to fit an integer

#include "fft_calc.h"

#define SAMPLE_SIZE                       10                                    // number of samples (past) are used to calculate the input characteristics
#define SILENCE_SIZE                    1000                                    // how many samples are used to determin "silence" (background conditions) average 
#define DETECTOR_PRECISION                 2                                    // threshold from until which, signal average variations are not considered as sound
#define BOARD_LED                         13                                    // internal board LED
#define FAST_DELAY                       200                                    // fast delay definition (e.g. for quick blinking)
#define NUM_MICS                           3                                    // number of active mics
#define FFT_SAMPLE_SIZE                   64                                    // size of FFT data buffer input
//#define FFT_REFERENCE_FREQUENCY_0         38                                    // reference f0 of 37.50hz (100hz base)
//#define FFT_REFERENCE_FREQUENCY_1         33                                    // reference f1 of 32.81hz (100hz base)
#define FFT_REFERENCE_FREQUENCY_SAMPLES    4                                    // number of reference f0 and f1 frequencies
#define FFT_TOLERANCE_THRESHOLD           .3                                    // allowed variation from reference (percentage)
#define FFT_NOMALIZED_FREQUENCY         7700                                    // scale factor for FFT data input (all calculated with same reference scale/freq)


/* * * * * * * * * *
 * Datatypes definition
 */


struct soundKPIStruct{
  word  fsoundAvg /* simulated float %.2f*/   ;                                 // calculated sum of sound imput (baseline normalized)  || in silence struct, IS THE baseline!
  word  soundSum                              ;                                 // sum of the calculated absolute intensities (normalized to baseline)
  word  peekCountAvg                          ;                                 // average count detected peeks (per [x] samples)
  word  peekSumAvg                            ;                                 // average sum detected depressions (per [x] samples)
  word  valleyCountAvg                        ;                                 // average count detected peeks (per [x] samples)
  word  valleySumAvg                          ;                                 // average sum detected depressions (per [x] samples)
} ; // 12 bytes (6 * 2bytes)

struct sndSensorStruct{
  byte pin                                    ;                                 // mic input pins
  word direction                              ;                                 // mic relative dir (forward: 0; rigth = 90; left = -90)
  byte value                                  ;                                 // input value (ephemeral)
} ; // 1 + 2 +1 : 4 bytes

byte FFTRefFrequencies[4]                 = { 7, 18, 45, 34 };                  // sorted by occurences in sampling data

/* * * * * * * * * *
 * variables definition
 */

sndSensorStruct 
    soundSensors[NUM_MICS]      = { {A5, 0} }      ;                            // sound sensors' vector

soundKPIStruct silenceKPIs[NUM_MICS]      = { {0} } ;                           // background noise KPIs (silence)
soundKPIStruct inputsampleKPIs[NUM_MICS]  = { {0} } ;                           // sound processing KPIs (sliding window)

byte fftData[NUM_MICS][FFT_SAMPLE_SIZE]   = {0};    /* 3 * 64 = 192bytes */     // arrays of type word (1byte), since sound input is normalized to baseline

// @@@ review declaration from this point
// @@@ convert into vector!
//int sensorPin                         =    A5 ;                                 // first mic analog pin
//int sensorPin                         =    D5 ;                                 // first mic analog pin
//int sensorPin1                        =    A4 ;                                 

int sensorValue                               ;                                 // serial value read from sensor

short sample[SAMPLE_SIZE]             =   {0} ;                                 // serial values (buffer or sliding window)
long  sampleSum                       =     0 ;                                 // 
int   sampleAvg                       =     0 ;
int   sampleAvgPrevious               =     0 ;
short index                           =    -1 ;

int   silenceAvg                      =     0 ;
int   silenceVector[SILENCE_SIZE]     =   {0} ;
bool  silenceDetermined   = false ;

int max = 0;
int min = 0;

// determine "silence" conditions
// no need to store the samples. Running calculation of KPIs is enough
// soundKPIs is assumed to be initialized when invoked
bool determineSilenceLevels(soundKPIStruct soundKPIs[NUM_MICS]){
  int   n;
  int   sample_num = 0;
  int   sampleSize = 1000;
  long  tempSum[NUM_MICS] = {0};
  float ftempAvg[NUM_MICS] = {0.0};

  Serial.println("Starting backgound noise calculation (determine 'silence' baseline')...");
    blink(BOARD_LED , 3, 200);                                                   // blink internal LED - start
    // capture samples for each mic
    while (sample_num < sampleSize){
      for (n = 0; n < NUM_MICS; n++){
        tempSum[n] += analogRead(soundSensors[n].pin);
      }
      sample_num++;
    }

    // establish baseline and calculate other KPIs for each mic
    for (n = 0; n < NUM_MICS; n++){
      // average (baseline)
      ftempAvg[n] = (tempSum[n] * 1.0) / sample_num;                            // avg calculation
      soundKPIs[n].fsoundAvg = ftempAvg[n] * 100.0;                             // should truncate, keeping two decimal places in integer part

      // calculate sum of intensities (normalized to baseline)
      soundKPIs[n].soundSum = tempSum[n] - ( (soundKPIs[n].fsoundAvg / 100.0) * sample_num );      // normalizes intensities sum to baseline

      // @@@ tbd to remining KPIs
    }
    blink(BOARD_LED , 5, 100);                                                  // blink internal LED - end
  Serial.println("Finished backgound noise calculation (established 'silence' baseline)...");
  return true;
}

// new sample's impact on sample input stats
int calcNewStat(int n, int newValue){
  
  sampleSum += (- sample[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sample[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

/*
int signalDetect(int avg, int newAvg){
  if (avg != newAvg){
    Serial.println();
  }
}
*/

void blink(int led, int times, int duration){
    for (int n = 0; n < times; n++){
      digitalWrite(led, HIGH);
      delay(duration);
      digitalWrite(led, LOW);
      delay(duration);
    }
}

void setup() {
  //Serial.begin(9600);
  Serial.begin(19200);
  //Serial.println("max\tmin\tSignal_Instant\tRunning_Avg");
  Serial.println("Starting somthing nice");
  // capture background information
  determineSilenceLevels(silenceKPIs);

  // @@@ debug info
  Serial.print("baseline = "); Serial.println(silenceKPIs[0].fsoundAvg / 100);
  Serial.print("sumIntencities = "); Serial.println(silenceKPIs[0].soundSum);
}


void captureFFTBuffer(){
  uint8_t inputSample = 0;
  int n = 0;
  while ( inputSample < FFT_SAMPLE_SIZE ){
    for (n = 0; n < NUM_MICS; n++){   // cycle through mics
      fftData[n][inputSample] = analogRead(soundSensors[n].pin) - (silenceKPIs[n].fsoundAvg / 100.0); // sound is acquired already normalized regarding baseline
      /// @@@ include a sum of intensities for mic input differentiation?
    }
    inputSample++;
  }
}

// calculate FFT values for each mic
void calcFFT(){ 
  int n, i;
  int mic = 0;
  byte fftMatchResults[NUM_MICS][2] = {0};
  char output[60] = "";
  bool f0Found = false;
  bool f1Found = false;

  for (mic = 0; mic < NUM_MICS; mic++){
    FFT((int*)fftData[mic], FFT_SAMPLE_SIZE, FFT_NOMALIZED_FREQUENCY);                // calculate FFT values for each mic
    fftMatchResults[mic][0] = 1.0 * f_peaks[0];//  / FFT_REFERENCE_FREQUENCY_0 * 100; // store most relevant calculated frequencies 
    fftMatchResults[mic][1] = 1.0 * f_peaks[1];// / FFT_REFERENCE_FREQUENCY_1 * 100;  // store second most relevant calculated frequencies 

    // detect if acquired f0 is on of the reference frequencies
    for (i = 0; i < FFT_REFERENCE_FREQUENCY_SAMPLES; i++) {if (fftMatchResults[mic][0] == FFTRefFrequencies[i]){ f0Found = true; break;} }
    for (i = 0; i < FFT_REFERENCE_FREQUENCY_SAMPLES; i++) {if (fftMatchResults[mic][1] == FFTRefFrequencies[i]){ f1Found = true; break;} }

    Serial.print(f0Found); Serial.print(" ");Serial.print(f1Found); Serial.print(" ");
    sprintf(output, "mic %d, freq[%d;%d]", mic, fftMatchResults[mic][0], fftMatchResults[mic][1]);Serial.print(output); Serial.print("   ||   ");
    for (n = 0; n < FFT_SAMPLE_SIZE; n++) { Serial.print(fftMatchResults[mic][n]);Serial.print("; ");} Serial.print(f0Found);Serial.print("%%");Serial.print(f1Found);Serial.println();
    
    if (f0Found && f1Found) Serial.println("VALID Sample!!!!!!!!!!!!!!!!!!!!!!!!");
    
    /*if (
        (abs(fftMatchResults[mic][0] - FFT_REFERENCE_FREQUENCY_0) <= 
          FFT_REFERENCE_FREQUENCY_0 * FFT_TOLERANCE_THRESHOLD) &&
        (abs(fftMatchResults[mic][1] - FFT_REFERENCE_FREQUENCY_1) <= 
        FFT_REFERENCE_FREQUENCY_0 * FFT_TOLERANCE_THRESHOLD) )
    {
          Serial.println("Valid Sample!!!");
    }*/
  }
}



struct micReadStruct {
  byte sensorValue[NUM_MICS];
};

void captureMicsArraySample(sndSensorStruct sensors[NUM_MICS]){
  int n;
  for (n = 0; n < NUM_MICS; n++){
    sensors[n].value = analogRead (sensors[0].pin) - (silenceKPIs[0].fsoundAvg / 100);
  }
}

void loop() {

  //if (index < SAMPLESIZE){
    index++;
    captureSample(soundSensors);

    for (int n = 0; n < NUM_MICS; n++){
      Serial.print(soundSensors[n].value); Serial.print(" ");
    }
    Serial.println();
    
    /*
    sensorValue = analogRead (soundSensors[0].pin) - (silenceKPIs[0].fsoundAvg / 100);
    //sensorValue = digitalRead (sensorPin);
    index = calcNewStat(index % SAMPLE_SIZE, sensorValue);
    sampleAvgPrevious = sampleAvg;

    if (sensorValue > max) max = sensorValue;
    if (sensorValue < min) min = sensorValue;
  
    sampleAvg = sampleSum / SAMPLE_SIZE;
    /*
    // determine if there is a disruption in sound detected
    if (sensorValue > DETECTOR_PRECISION) {
      // output result detected
      // capture a sizable buffer for FFT calculation
      captureFFTBuffer();
      calcFFT();
    }
    */
    //Serial.print(index); Serial.print("_");
    //Serial.print(sampleAvg); Serial.print("_|_");
    //Serial.print(sampleAvgPrevious); Serial.print("_||_");
    //Serial.println(abs(sampleAvg - sampleAvgPrevious));
    /*
      
     if (abs(sampleAvg - sampleAvgPrevious) >= DETECTOR_PRECISION) {
      
      Serial.println("sound detected. diffd");
    }
    */
    
    //Serial.print(analogRead (sensorPin), DEC);
    //Serial.print("\t");
    //Serial.print(index);Serial.print("_"); 
    
    //for (int n = 0; n < SAMPLESIZE; n++) {Serial.print(sample[n]);Serial.print("_");}
    
//    Serial.print("Max:");Serial.print(max);Serial.print(",");
//    Serial.print("Min:");Serial.print(min);Serial.print(",");
//    
//    Serial.print("Input:");Serial.print(sensorValue);Serial.print(",");
//    Serial.print("InputAvg[50]:");Serial.println(sampleAvg);
    //Serial.println(silenceKPIs[0].fsoundAvg / 100, DEC);
    
    //Serial.print("||");Serial.print(sampleSum);
    
    //Serial.println(analogRead (sensorPin1), DEC);%
    //Serial.println();
  //}
}
