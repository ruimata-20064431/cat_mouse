#ifndef soundCapture_h
#define soundCapture_h_h

#include "fft_calc.h"
#include "divs.h"

#define NUM_MICS                              3                                 //
#define SAMPLE_SIZE                          10                                 // number of samples (past) are used to calculate the input characteristics
#define PREV_VALUES                           5                                 //
#define FFT_SAMPLE_SIZE                      64                                 // size of FFT data buffer input
#define FFT_NOMALIZED_FREQUENCY             100                                 // scale factor for FFT data input (all calculated with same reference scale/freq)
#define FFT_REFERENCE_FREQUENCY_SAMPLES       4                                 // number of reference f0 and f1 frequencies
#define SOUND_DISRUPTION_PRECISION            7                                 // threshold from until which, signal average variations are not considered as sound



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Datatypes definition
 */

struct soundKPIStruct{
  float fsoundAvg                             ;                                 // calculated sum of sound imput (baseline normalized)  || in silence struct, IS THE baseline!
  word  soundSum                              ;                                 // sum of the calculated absolute intensities (normalized to baseline)
  word  peakCountAvg                          ;                                 // average count detected peaks (per [x] samples)
  word  peakSumAvg                            ;                                 // average sum detected depressions (per [x] samples)
  word  valleyCountAvg                        ;                                 // average count detected peaks (per [x] samples)
  word  valleySumAvg                          ; 
  // average sum detected depressions (per [x] samples)
} ; // 12 bytes (6 * 2bytes)


struct sndSensorStruct{
  byte pin                                    ;                                 // mic input pins
  word direction                              ;                                 // mic relative dir (forward: 0; rigth = 90; left = -90)
  int  value                                  ;                                 // input value (ephemeral)
} ; // 1 + 2 + 2 : 5 bytes


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Declarations
 */

sndSensorStruct soundSensors[NUM_MICS]  = { {A0, 0,  0},
                                            {A1, 0,  0},
                                            {A2, 0,  0}    };                   // sound sensors' vector
                                            
const byte FFTRefFrequencies[4]         = {  7, 18, 45, 34 };                   // sorted by occurences in sampling data
long  sampleSum                         =                0  ;                   // 
word  sample[SAMPLE_SIZE]               = {              0 };                   // serial values (buffer or sliding window)
word  prevValues[NUM_MICS][PREV_VALUES] = {              0 };                   // immediate LAST_VALUE_SIZE's values
byte  prevValuesIndex                   =                0  ;                   // index to keep track os last values buffer
word  prevValuesSum[NUM_MICS]           = {              0 };                   // calculated sum of prevValues
char  fftData[NUM_MICS][FFT_SAMPLE_SIZE]= {              0 }; //3*64=192bytes   // arrays of type word (1byte), since sound input is normalized to baseline
soundKPIStruct silenceKPIs[NUM_MICS]    = {            {0} };                   // background noise KPIs (silence)
soundKPIStruct sampleKPIs[NUM_MICS]     = {            {0} };                   // background noise KPIs (silence)


/* -Silence detection & KPIs------------------------------------------------- */

// determine "silence" conditions
// no need to store the samples. Running calculation of KPIs is enough
// soundKPIs is assumed to be initialized when invoked
bool determineSilenceLevels(soundKPIStruct soundKPIs[NUM_MICS]){
  int   n;
  int   sample_num = 0;
  int   sampleSize = 1000;
  long  tempSum[NUM_MICS] = {0};
  //float ftempAvg[NUM_MICS] = {0.0};

  Serial.println("Starting_backgound_noise_calculation_(determine_'silence'_baseline')...");
    blink(BOARD_LED , 3, 200);                                                  // blink internal LED - start
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
      soundKPIs[n].fsoundAvg = (tempSum[n] * 1.0) / sample_num;                 // avg calculation

      // calculate sum of intensities (normalized to baseline)
      soundKPIs[n].soundSum  = tempSum[n] - 
              (soundKPIs[n].fsoundAvg * sample_num );                           // normalizes intensities sum to baseline

      // @@@ tbd to remining KPIs
    }
    blink(BOARD_LED , 5, 100);                                                  // blink internal LED - end
  Serial.println("Finished_backgound_noise_calculation_(established_'silence'_baseline)...");
  return true;
}

void initSoundKPIs( soundKPIStruct pKPIs[NUM_MICS] ){
  for (int n = 0; n < NUM_MICS; n++){
    pKPIs[n].fsoundAvg      = 0 ;
    pKPIs[n].soundSum       = 0 ;
    pKPIs[n].peakCountAvg   = 0 ;
    pKPIs[n].peakSumAvg     = 0 ;
    pKPIs[n].valleyCountAvg = 0 ;
    pKPIs[n].valleySumAvg   = 0 ;
  }
}

// process inputted sound buffer and calculate KPIs for each mic
// KPIs are stored in a global sampleKPI
void calculateBufferKPIs(char  pSoundData[NUM_MICS][FFT_SAMPLE_SIZE]){
  int n = 0;
  initSoundKPIs (sampleKPIs); 
  /*
  Serial.print("=> ");
  Serial.print(pSoundData[0][0]);
  Serial.println();

  int m, x;
  for (m = 0; m < NUM_MICS; m++){
    for (x = 0; x < FFT_SAMPLE_SIZE; x++)
      Serial.print(m); Serial.print(", ");
      Serial.print(x); Serial.print(", ");
      Serial.print(pSoundData[m][x]); Serial.print(", ");
    Serial.println(); 
  }
  */
  for (int m = 0; m < NUM_MICS; m++){                                           // cycle for each mic
    // Initialize KPIs
    for (n = 0; n < FFT_SAMPLE_SIZE; n++){                                      // cycle through sample data
        // Sums (total, positive and negative intensities)
        sampleKPIs[m].soundSum += pSoundData[m][n];
        // positive intensities
        if (pSoundData[m][n] > 0) {                                             // sum/count peak
          sampleKPIs[m].peakSumAvg    += pSoundData[m][n]       ;    
          sampleKPIs[m].peakCountAvg++                          ; 
        }  
        if (pSoundData[m][n] < 0) {                                             // sum/count "valleys"
          sampleKPIs[m].valleySumAvg  += abs(pSoundData[m][n])  ;  
          sampleKPIs[m].valleyCountAvg++                        ; 
        }  
    }
    sampleKPIs[m].fsoundAvg = 1.0 * sampleKPIs[m].soundSum / n;
  }
}

// check KPIs in sampleKPIs to identify sound origin...
// (why am I not a believer...??)
void compareKPIs(){
  int m = 0;

  Serial.println("=mic comparision of values===================>>>>");
  
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].soundSum       );  Serial.print("  "); } Serial.println(); // check intentities
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].fsoundAvg      );  Serial.print("  "); } Serial.println(); // check total average
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].peakSumAvg     );  Serial.print("  "); } Serial.println(); // check peak intentities
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].peakCountAvg   );  Serial.print("  "); } Serial.println(); // check peak count
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].valleySumAvg   );  Serial.print("  "); } Serial.println(); // check valley intensities
  for (m = 0; m < NUM_MICS; m++){ Serial.print(sampleKPIs[m].valleyCountAvg );  Serial.print("  "); } Serial.println(); // check valley count
  
  Serial.println("=end mics comparision========================<<<<<<<");
}

/* -sound processing -------------------------------------------------------- */

void outputSample(sndSensorStruct pSensors[NUM_MICS]){
    for (int n = 0; n < NUM_MICS; n++){ Serial.print(pSensors[n].value); Serial.print(" "); }
    Serial.println();
}

// new sample's impact on sample input vector stats 
int calcNewStat(int n, int newValue){
  
  sampleSum += (- sample[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sample[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

void captureMicsArraySample(sndSensorStruct sensors[NUM_MICS]){
  int n;
  for (n = 0; n < NUM_MICS; n++){
    sensors[n].value = analogRead (sensors[n].pin) - (silenceKPIs[n].fsoundAvg);
  }
}


// tries to detect a disruption on the sound detected by sampling 
// the x (LAST_VALUES_SIZE) previous samples and checking if the 
// absolute sum is greater that the SOUND_DISRUPTION_PRECISION x
// the number of previous samples (LAST_VALUES_SIZE)
// a value on a single mic is enough to return true
bool detectedNoiseDisruption(){             // directly accesses soundSensors for last sampling
  int   n   = 0;
  int   sum = 0;
  word  soundTolerance = SOUND_DISRUPTION_PRECISION * PREV_VALUES;      // calculate tolerance limit
  bool detected = false;

  // fill buffer last position
  prevValuesIndex = ++prevValuesIndex % PREV_VALUES;                    // normalizes incremented index (PREV_VALUES limit)
  for (n = 0; n < NUM_MICS; n++) {
    prevValuesSum[n]  = prevValuesSum[n] 
                      - prevValues[n][prevValuesIndex]
                      + soundSensors[n].value;                          // replace previous value by new one in sum
    prevValues[n][prevValuesIndex] = soundSensors[n].value ;            // store current mic's sample
    if (prevValuesSum[n] > soundTolerance){
      Serial.print("Disruption detected: "); Serial.println(n);
      detected = true; 
      }    // check if there was a disruption detected in that mic
  }
  //if (detected) for (n = 0; n < NUM_MICS; n++) prevValuesSum[n] = 0;       // clear sum to minimize impat in following samples
  return detected;
}






/* -FFT auxiliary functions-------------------------------------------------- */

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

    // Process result and compare result to reference frequencies.
    fftMatchResults[mic][0] = 1.0 * f_peaks[0];//  / FFT_REFERENCE_FREQUENCY_0 * 100; // store most relevant calculated frequencies 
    fftMatchResults[mic][1] = 1.0 * f_peaks[1];// / FFT_REFERENCE_FREQUENCY_1 * 100;  // store second most relevant calculated frequencies 

    // detect if acquired f0 is on of the reference frequencies
    for (i = 0; i < FFT_REFERENCE_FREQUENCY_SAMPLES; i++) {if (fftMatchResults[mic][0] == FFTRefFrequencies[i]){ f0Found = true; break;} }
    for (i = 0; i < FFT_REFERENCE_FREQUENCY_SAMPLES; i++) {if (fftMatchResults[mic][1] == FFTRefFrequencies[i]){ f1Found = true; break;} }

    Serial.print(f0Found); Serial.print(" ");Serial.print(f1Found); Serial.print(" ");
    sprintf(output, "mic %d, freq[%d;%d]", mic, fftMatchResults[mic][0], fftMatchResults[mic][1]);Serial.print(output); Serial.print("   ||   ");
    //for (n = 0; n < FFT_SAMPLE_SIZE; n++) { Serial.print(fftMatchResults[mic][n]);Serial.print("; ");} Serial.print(f0Found);Serial.print("%%");Serial.print(f1Found);Serial.println();
    
    if (f0Found && f1Found) Serial.println("VALID Sample!!!!!!!!!!!!!!!!!!!!!!!!");
    /*
    if (
        (abs(fftMatchResults[mic][0] - FFT_REFERENCE_FREQUENCY_0) <= 
          FFT_REFERENCE_FREQUENCY_0 * FFT_TOLERANCE_THRESHOLD) &&
        (abs(fftMatchResults[mic][1] - FFT_REFERENCE_FREQUENCY_1) <= 
        FFT_REFERENCE_FREQUENCY_0 * FFT_TOLERANCE_THRESHOLD) )
    {
          Serial.println("Valid Sample!!!");
    }
    */
  }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif
