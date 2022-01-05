
// example    https://www.youtube.com/watch?v=KaN7Y4XxHhc
// datatypes https://learn.sparkfun.com/tutorials/data-types-in-arduino/defining-data-types
// simulated floats are 2 decimal precision floats x 100 to fit an integer

#define SAMPLE_SIZE                       10                                    // number of samples (past) are used to calculate the input characteristics
#define SILENCE_SIZE                    1000                                    // how many samples are used to determin "silence" (background conditions) average 
#define DETECTOR_PRECISION                 2                                    // threshold from until which, signal average variations are not considered as sound
#define BOARD_LED                         13                                    // internal board LED
#define FAST_DELAY                       200                                    // fast delay definition (e.g. for quick blinking)
#define NUM_MICS                           1//3                                    //

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
} ; // 1 + 2: 3 bytes


/* * * * * * * * * *
 * variables definition
 */

sndSensorStruct 
    soundSensors[NUM_MICS]      = { {A5, 0} }      ;                            // sound sensors' vector

soundKPIStruct silenceKPIs[NUM_MICS]      = { {0} } ;                           // background noise KPIs (silence)
soundKPIStruct inputsampleKPIs[NUM_MICS]  = { {0} } ;                           // sound processing KPIs (sliding window)

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
  Serial.println("Finished backgound noise calculation (established 'silence' baseline')...");
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
  Serial.println("Signal_Instant\tRunning_Avg");

  // capture background information
  determineSilenceLevels(silenceKPIs);

  // @@@ debug info
  Serial.print("baseline = "); Serial.println(silenceKPIs[0].fsoundAvg / 100);
  Serial.print("sumIntencities = "); Serial.println(silenceKPIs[0].soundSum);
}


void loop() {
  //if (index < SAMPLESIZE){
    index++;
    
    sensorValue = analogRead (soundSensors[0].pin) - (silenceKPIs[0].fsoundAvg / 100);
    //sensorValue = digitalRead (sensorPin);
    index = calcNewStat(index % SAMPLE_SIZE, sensorValue);
    sampleAvgPrevious = sampleAvg;
    
    sampleAvg = sampleSum / SAMPLE_SIZE;
    
    Serial.print(index); Serial.print("_");
    Serial.print(sampleAvg); Serial.print("_|_");
    Serial.print(sampleAvgPrevious); Serial.print("_||_");
    Serial.println(abs(sampleAvg - sampleAvgPrevious));
    if (abs(sampleAvg - sampleAvgPrevious) >= DETECTOR_PRECISION) {
      
      Serial.println("sound detected. diffd");
    }
    
    //Serial.print(analogRead (sensorPin), DEC);
    //Serial.print("\t");
    //Serial.print(index);Serial.print("_"); 
    
    //for (int n = 0; n < SAMPLESIZE; n++) {Serial.print(sample[n]);Serial.print("_");}
    
    Serial.println(sample[index]);//Serial.print("\t");
    
    //Serial.println(silenceKPIs[0].fsoundAvg / 100, DEC);
    
    //Serial.print("||");Serial.print(sampleSum);
    
    //Serial.println(analogRead (sensorPin1), DEC);%
    //Serial.println();
  //}
}
