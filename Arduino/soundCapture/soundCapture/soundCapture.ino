#include "soundCapture.h"
#include "divs.h"




void setup() {
  
  Serial.begin(19200);

  /*
  int sensorPinR                         =    A2 ;                                 // Left - first mic analog pin
  int sensorValueL                               ;                                 // Left - serial value read from sensor
  
  int sensorPinC                         =    A1 ;                                 // Center - first mic analog pin
  int sensorValueC                               ;                                 // Center - serial value read fr
  
  int sensorPinL                         =    A0 ;                                 // Center - first mic analog pin
  int sensorValueR;  
  Serial.print("Left"); Serial.print(" ");
  Serial.print("Central"); Serial.print(" ");
  Serial.println("Right"); Serial.print(" ");

  Serial.println();
  for (int n = 0; n < 10000; n++){
    sensorValueL = analogRead (sensorPinL);
    Serial.print(sensorValueL);Serial.print(" ");
    
    sensorValueC = analogRead (sensorPinC);
    Serial.print(sensorValueC);Serial.print(" ");
    
    sensorValueR = analogRead (sensorPinR);
    Serial.println(sensorValueR);Serial.print(" ");
  }*/
  
  determineSilenceLevels(silenceKPIs);              // capture background information


  Serial.print("Number_of_mics____________"); Serial.println(NUM_MICS)                   ;
  Serial.print("SoundDisruptionBufferSize_"); Serial.println(PREV_VALUES)                ;
  Serial.print("SoundDisruptionTolerance__"); Serial.println(SOUND_DISRUPTION_PRECISION) ;
  Serial.print("FFT_SAMPLE_SIZE___________"); Serial.println(FFT_SAMPLE_SIZE)            ;
  Serial.print("FFTRefFrequencies_________"); 
  int n;   for(n = 0; n<FFT_SAMPLE_SIZE; n++);{Serial.print(FFTRefFrequencies[n])         ;}Serial.println();
  
  // @@@ debug info
  for (int n = 0; n < NUM_MICS; n++){
    Serial.print(n); Serial.print("_Silence_baseline________"); Serial.println(silenceKPIs[n].fsoundAvg );
    Serial.print(n); Serial.print("_Silence_sumIntencities__"); Serial.println(silenceKPIs[n].soundSum  );
  }

}

void loop() {

  /*
    int sensorPinR                         =    A2 ;                                 // Left - first mic analog pin
    int sensorValueL                               ;                                 // Left - serial value read from sensor
    
    int sensorPinC                         =    A1 ;                                 // Center - first mic analog pin
    int sensorValueC                               ;                                 // Center - serial value read fr
    
    int sensorPinL                         =    A0 ;                                 // Center - first mic analog pin
    int sensorValueR;  
    sensorValueL = analogRead (sensorPinL- (silenceKPIs[0].fsoundAvg));
    Serial.print(sensorValueL);Serial.print(" ");
    
    sensorValueC = analogRead (sensorPinC- (silenceKPIs[1].fsoundAvg));
    Serial.print(sensorValueC);Serial.print(" ");
    
    sensorValueR = analogRead (sensorPinR- (silenceKPIs[2].fsoundAvg));
    Serial.println(sensorValueR);Serial.print(" ");

    */
    
    captureMicsArraySample(soundSensors);
    outputSample(soundSensors);

    if (detectedNoiseDisruption()){
      captureFFTBuffer();                                                   // acquire FFT buffer
      calcFFT();                                                            // calculate FFT (Fast Fourier Transform}
      // calculate KPIs
      calculateBufferKPIs(fftData);

      compareKPIs();
      //calculateSoundOrigin();
      
    }
    
}
