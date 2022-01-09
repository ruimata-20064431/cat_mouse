#ifndef car_info_h
#define car_info_h

#define MTR_NUM           2   // number of motors available

// ###########################
// CAR CONFIGURATION STRUCTURE   (defines values that may vary between each car, eg.: motor power on each wheel)

typedef struct MotorS {             // DC motor management
  uint8_t pinSpeed;                 // must be PWM pins for speed control
  uint8_t pinFwd;
  uint8_t pinBkw;
} MotorType;

typedef struct MotorsS{
  MotorType motor1;
  MotorType motor2;
} MotorsType;

int cars[2][2][2][2] = {
  // CAR 0 (TOM)
  {
      // MOTOR 0 (Right)
      {
          // BACK  movement config
          {230, 100},
          // FRONT movement config
          {250, 100}
      },
      // MOTOR 1 (Left)
      {
          // BACK  movement config
          {255, 100},
          // FRONT movement config
          {255, 100}
      }  
  },
  // CAR 1 (JERRY)
  {
      // MOTOR 0 (Right)
      {
          // BACK  movement config
          {255, 100},
          // FRONT movement config
          {255, 100}
      },
      // MOTOR 1 (Left)
      {
          // BACK  movement config
          {250, 100},
          // FRONT movement config
          {230, 100}
      }
  }
};

MotorType motors[MTR_NUM] = {
  {
    .pinSpeed = 3, // motor[0] control pins
    .pinFwd = 4,
    .pinBkw = 5
  },
  {
    .pinSpeed = 9, // motor[1] control pins
    .pinFwd = 7,
    .pinBkw = 6
  }
};


// ****************** DEPRECATED CODE ******************
// Deprecated code below. Previously used to allow unique car IDs and general information.
// However it was best to scrape off the idea because of unnecessary resource usage.
// It was then decided that the cars will only be distinguished by a single character saved on EEPROM, either 0 or 1.

// ########## INVENTORYSTRUCT: for cars identification ##########
/* 
  #define INV_TAG_SIZE     10   // inventory tag size
  struct InventoryStruct{       // structure of Inventory tags
    char owner[6];
    char yearMonth[5];
    char modelVersion[4];
    char inventoryNum[4];
};*/

/* ***** ... and this was the corresponding function to manipulate InventoryStruct (naturally deprecated, too) *****
// retrieve information on inventory tag (last 15 bytes of EEPROM.
// Input out[STR_STD_SIZE] only present for memory "allocation"
// This topics could/should go to a lib
char* getInventoryTag(char out[STR_STD_SIZE], boolean displayTag){
  char* inventoryTag = out;                               // return String
  int eepromSize = EEPROM.length();                       // gather EEPROM size
  int i;
  
  EEPROM.get(eepromSize - INV_TAG_SIZE, inventoryTagS);   // retrieve inventory tag
  for (i = 0; i < INV_TAG_SIZE; i++) inventoryTag[i] = inventoryTagS.owner[i]; // copy tag to output
  
  Serial.print("Car number is:"); Serial.println(inventoryTag[i]);
  carNumber = inventoryTag[i-1];                            // load the car model (the last inventoryTag character)
  return inventoryTag;
} */
// ****************** ******************

#endif
