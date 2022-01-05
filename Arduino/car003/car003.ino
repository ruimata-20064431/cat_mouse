// for the moment, movement takes ownership of arduino for simplicity
// the device will execute one task at a time:
//   a) listen; b) rotate; c) move; d) detect proximity  (may already 
//   have bumped into something)
// potentially,  in the future,  if all the rest suffices, and energy 
// remains, we may be consider the possibility of  adjusting it to be
// more "aware"... or less unaware... ;)

// motors sequence changed: now 0: right and 1: left

#include <EEPROM.h>

#define MOVE_FORWARD      1                                                     // forward direction
#define MOVE_BACKWARD     2                                                     // backward direction
#define STR_STD_SIZE     64                                                     // normalized (not optimized) string size ##
#define INV_TAG_SIZE     15                                                     // inventory tag size
#define MTR_NUM           2                                                     // number of motors available
#define MTR_SPEED         0                                                     // max motor speed quantifier index
#define MTR_DIVIDER       1                                                     // max motor speed divider index
#define PIN_SPEED         0                                                     // speed control pin
#define PIN_DIRECTION_FWD 1                                                     // direction forward control pin
#define PIN_DIRECTION_BKW 2                                                     // direction backward control pin


struct InventoryStruct{                                                         // strcuture of Inventory tags
  char owner[6];
  char yearMonth[5];
  char modelVersion[4];
  char inventoryNum[4];
};
InventoryStruct inventoryTagS = {"", "", "", ""};
char text[STR_STD_SIZE] = "";                                                   // only for inventoryTag memory allocation
char * inventoryTag;                                                            // char* is the easiest way to return a string from a function

typedef struct MotorS {                                                         // DC motor management
  uint8_t pinSpeed;                                                             // must be PWM pins for speed control
  uint8_t pinFwd;
  uint8_t pinBkw;
} MotorType;  

typedef struct MotorsS{
  MotorType motor1;
  MotorType motor2;
} MotorsType;

typedef struct  MovementS
{
  int direction;                                                                // [0..356], being 0:forward && 179:backward
  // .... TBC... or probably not
} MovementType;

MotorsType motors1 = {  {9, 7, 6},    //, 0, 0},                                // @@@ remove?
                        {3, 4, 5}  }; //, 0, 0} };                              // car engines

MotorType motors[MTR_NUM] = { { .pinSpeed = 3,                                  // motor[0] control pins
                                .pinFwd   = 4,
                                .pinBkw   = 5},
                              { .pinSpeed = 9,                                  // motor[1] control pins
                                .pinFwd   = 7, 
                                .pinBkw   = 6} };

int motorConfig[2][2] = { {210, 100},                                           // motor0 (right)  {power limit, divider}
                          {255, 100}};                                          // motor1 (left)

// retrieve information on inventory tag (last 15 bytes of EEPROM.
// Input out[STR_STD_SIZE] only present for memory "allocation"
// This topics could/should go to a lib
char * getInventoryTag(char out[STR_STD_SIZE], boolean displayTag){
  char * inventoryTag = out;                                                    // return String
  int eepromSize = EEPROM.length();                                             // gather EEPROM size
  
  
  EEPROM.get(eepromSize - INV_TAG_SIZE, inventoryTagS);                         // retrieve inventory tag
  for (int i = 0; i < INV_TAG_SIZE; i++) inventoryTag[i] = inventoryTagS.owner[i]; // copy tag to output
  return inventoryTag;
}

void moveStraight(int direction, int distance, int speed){
  // for the moment, movement takes ownership of arduino for simplicity
    int adjustedSpeed;
    int forward  = (direction == 1) ? HIGH : LOW  ;
    int backward = (direction == 1) ? LOW  : HIGH ;

    // set LED (to make movement "visible")
    digitalWrite(13, (direction == 1)? HIGH : LOW);
    Serial.println("Moving...");
    // cycle through motors to handle movement specs
    for (int m = 0; m < MTR_NUM; m++){
      adjustedSpeed = (speed / motorConfig[m][MTR_DIVIDER]) * motorConfig[m][MTR_SPEED]; // calc adjusted speed based on parameters
      Serial.print("Motor"); Serial.print(m);
      Serial.print("  adjustedSpeed = "); Serial.println(adjustedSpeed);        
      analogWrite(motors[m].pinSpeed, adjustedSpeed);                           // set motor adjusted speed
      digitalWrite(motors[m].pinFwd, forward  );                                // set motor's forward pin value
      digitalWrite(motors[m].pinBkw, backward );                                // set motor's bakward pin value
    }
    
    // "distance" is set more like a duration
    delay(distance * 1000);                                                     // potential improvement in the future ##
    Serial.println("...Stopping!");
    // undo everything
    for (int m = 0; m < MTR_NUM; m++){
      digitalWrite(motors[m].pinSpeed , LOW   );
      digitalWrite(motors[m].pinFwd   , LOW   );
      digitalWrite(motors[m].pinBkw   , LOW   );
    }

    blink(13, 2, 100);                                                          // movement stop visual indicator
}

void blink(int led, int times, int duration){
    for (int n = 0; n < times; n++){
      digitalWrite(led, HIGH);
      delay(duration);
      digitalWrite(led, LOW);
      delay(duration);
    }
}

void setup(){
    // initialize Serial comms (monitor, only)
    Serial.begin(9600);
    // define pins direction
    for (int m = 0; m < MTR_NUM; m++){
      pinMode (motors[m].pinSpeed , OUTPUT);
      pinMode (motors[m].pinFwd   , OUTPUT);
      pinMode (motors[m].pinBkw   , OUTPUT);
    }
    
    inventoryTag = getInventoryTag(text, true);
    Serial.println("\n\n\n\n\n\n==============================");
    Serial.print("Inventory Tag: "); Serial.println(inventoryTag);
    Serial.print("\n\n");



/*
  //////////////////////////////////////////////////////////////// 82.5%    
    // adjusted 82.5% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 210;
    moveStraight(1, 3, 100);
    delay(5000);
    
    // adjusted 82.5% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 210;
    moveStraight(1, 3, 100);
    delay(5000);

    
    // adjusted 82.5% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 210;
    moveStraight(1, 3, 100);
    delay(5000);

*/
  //////////////////////////////////////////////////////////////// 80%

    // adjusted 80% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 204;
    moveStraight(1, 3, 100);
    delay(5000);
    
    // adjusted 80% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 200;
    moveStraight(1, 6, 100);
    delay(5000);

    
    // adjusted 80% motor right
    motorConfig[1][0] = 255;
    motorConfig[0][0] = 204;
    moveStraight(1, 3, 100);
    delay(5000);


    //moveStraight(1, 3, 100);
}

void loop(){
  /*
    moveStraight(1, 3, 100);
    
    moveStraight(2, 3, 100);
  */
  

    delay(5000);
}
