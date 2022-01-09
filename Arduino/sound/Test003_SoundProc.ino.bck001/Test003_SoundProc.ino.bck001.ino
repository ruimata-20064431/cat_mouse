struct soundKPIStruct{
  float fsoundAvg                             ;                                 // calculated sum of sound imput (baseline normalized)  || in silence struct, IS THE baseline!
  word  soundSum                              ;                                 // sum of the calculated absolute intensities (normalized to baseline)
  word  peakCountAvg                          ;                                 // average count detected peaks (per [x] samples)
  word  peakSumAvg                            ;                                 // average sum detected depressions (per [x] samples)
  word  valleyCountAvg                        ;                                 // average count detected peaks (per [x] samples)
  word  valleySumAvg                          ; 
  // average sum detected depressions (per [x] samples)
} ; // 12 bytes (6 * 2bytes)




int sensorPinR                         =    A2 ;                                 // Left - first mic analog pin
int sensorValueL                               ;                                 // Left - serial value read from sensor

int sensorPinC                         =    A1 ;                                 // Center - first mic analog pin
int sensorValueC                               ;                                 // Center - serial value read fr

int sensorPinL                         =    A0 ;                                 // Center - first mic analog pin
int sensorValueR;                                                                // Center - serial value read fr


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


void setup() {
  //Serial.begin(9600);
  Serial.begin(19200);

  Serial.print("Left"); Serial.print(" ");
  Serial.print("Central"); Serial.print(" ");
  Serial.println("Right"); Serial.print(" ");
  
  Serial.println("RightAVG"); Serial.print(" ");
  
}

#define SAMPLESIZE 50
short sample[SAMPLESIZE];
int sampleSum = 0;
int sampleAvg = 0;
short index = 0;

// new sample's impact on sample input stats
int calcNewStat(int n, int newValue){

  
  sampleSum += (- sample[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sample[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

void loop() {
    index++;
    Serial.println();

    sensorValueL = analogRead (sensorPinL) + 20;
    Serial.print(sensorValueL);Serial.print(" ");
    
    sensorValueC = analogRead (sensorPinC);
    Serial.print(sensorValueC);Serial.print(" ");
    
    sensorValueR = analogRead (sensorPinR) - 20;
    Serial.print(sensorValueR);Serial.print(" ");
    
    index = calcNewStat(index % SAMPLESIZE, sensorValueR);
    sampleAvg = sampleSum / SAMPLESIZE;
    
    Serial.println(sampleAvg);Serial.print(" ");
    
    
    
}
