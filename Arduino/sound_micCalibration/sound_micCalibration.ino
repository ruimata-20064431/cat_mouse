#include "soundCapture.h"

int sensorPinR                         =    A2 ;                                 // Left - first mic analog pin
int sensorValueL                               ;                                 // Left - serial value read from sensor

int sensorPinC                         =    A1 ;                                 // Center - first mic analog pin
int sensorValueC                               ;                                 // Center - serial value read fr

int sensorPinL                         =    A0 ;                                 // Center - first mic analog pin
int sensorValueR;                                                                // Center - serial value read fr


#define SAMPLESIZE 50
short sampleL[SAMPLESIZE];
long sampleSumL = 0;
float sampleAvgL = 0;

short sampleC[SAMPLESIZE];
long sampleSumC = 0;
float sampleAvgC = 0;

short sampleR[SAMPLESIZE];
long sampleSumR = 0;
float sampleAvgR = 0;


short sampleLAvg[SAMPLESIZE * 2];
long sampleSumLAvg = 0;
float sampleAvgLAvg = 0;

short sampleCAvg[SAMPLESIZE * 2];
long sampleSumCAvg = 0;
float sampleAvgCAvg = 0;

short sampleRAvg[SAMPLESIZE * 2];
long sampleSumRAvg = 0;
float sampleAvgRAvg = 0;

int index = 0;
int index2 = 0;

void setup() {
  //Serial.begin(9600);
  Serial.begin(19200);

  //Serial.print("Left"); Serial.print(" ");
  Serial.print("LeftAvg"); Serial.print(" ");
  //Serial.print("Central"); Serial.print(" ");
  //Serial.print("CentralAvg"); Serial.print(" ");
  //Serial.print("Right"); Serial.print(" ");
  Serial.println("RightAvg"); 
  
  determineSilenceLevels(silenceKPIs);              // capture background information
  Serial.println(silenceKPIs[2].fsoundAvg); 

}


// new sample's impact on sample input vector stats 
int calcNewStatL(int n, int newValue){
  
  sampleSumL += (- sampleL[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleL[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

// new sample's impact on sample input vector stats 
int calcNewStatC(int n, int newValue){
  
  sampleSumC += (- sampleC[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleC [n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

// new sample's impact on sample input vector stats 
int calcNewStatR(int n, int newValue){
  
  sampleSumR += (- sampleR[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleR[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}






// new sample's impact on sample input vector stats 
int calcNewStatLAvg(int n, int newValue){
  
  sampleSumLAvg += (- sampleLAvg[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleLAvg[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

// new sample's impact on sample input vector stats 
int calcNewStatCAvg(int n, int newValue){
  
  sampleSumCAvg += (- sampleCAvg[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleCAvg [n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}

// new sample's impact on sample input vector stats 
int calcNewStatRAvg(int n, int newValue){
  
  sampleSumRAvg += (- sampleRAvg[n] + newValue);  // remove previous last SAMPLESIZE'th sample and add new one
  sampleRAvg[n] = newValue;                   // store new value on n'th position of the buffer
  return n;                               // returning n, comletes the circular buffer handling
}





void loop() {
    index++;
    index2++;
    Serial.println();
    index = index % SAMPLESIZE;
    index2 = index2 % (SAMPLESIZE * 2);

    sensorValueL = analogRead (sensorPinL) - (silenceKPIs[0].fsoundAvg);;
    //Serial.print(sensorValueL);Serial.print(" ");

    index = calcNewStatL(index , abs(sensorValueL));
    sampleAvgL = 1.0 * sampleSumL / SAMPLESIZE;
    //Serial.print(sampleAvgL);Serial.print(" ");


    index2 = calcNewStatLAvg(index2 , sampleAvgL);
    sampleAvgLAvg = 1.0 * sampleSumLAvg / (SAMPLESIZE * 2);
    Serial.print(sampleSumLAvg);Serial.print(" ");

    
    
    sensorValueC = analogRead (sensorPinC) - (silenceKPIs[1].fsoundAvg);
    //Serial.print(sensorValueC);Serial.print(" ");

    index = calcNewStatC(index, abs(sensorValueC));
    sampleAvgC = 1.0 * sampleSumC / SAMPLESIZE;
    //Serial.print(sampleAvgC);Serial.print(" ");

    index2 = calcNewStatCAvg(index2, sampleAvgC);
    sampleAvgCAvg = 1.0 * sampleSumCAvg / SAMPLESIZE * 2;
    //Serial.print(sampleSumCAvg);Serial.print(" ");




   
    sensorValueR = analogRead (sensorPinR) - (silenceKPIs[2].fsoundAvg);
    //Serial.print(sensorValueR);Serial.print(" ");
    
    index = calcNewStatR(index, abs(sensorValueR));
    sampleAvgR = 1.0 * sampleSumR / SAMPLESIZE;
    //Serial.print(sampleAvgR);Serial.println(" ");

    index2 = calcNewStatRAvg(index2, sampleAvgR);
    sampleAvgRAvg = 1.0 * sampleSumRAvg / SAMPLESIZE * 2;
    Serial.print(sampleSumRAvg);Serial.println(" ");






    
    
    
    
}
