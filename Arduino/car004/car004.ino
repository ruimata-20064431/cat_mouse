// for the moment, movement takes ownership of arduino for simplicity
// the device will execute one task at a time:
//   a) listen; b) rotate; c) move; d) detect proximity  (may already
//   have bumped into something)
// potentially,  in the future,  if all the rest suffices, and energy
// remains, we may be consider the possibility of  adjusting it to be
// more "aware"... or less unaware... ;)
//
// motors sequence changed: now 0: right and 1: left

// #### INCLUDES ###############
#include <EEPROM.h>
#include "car_info.h"
#include "main_jerry.h"
#include "main_tom.h"

// #### MACROS #################
// To help separating code that each car should run
#define TOM                    0   // the chaser
#define JERRY                  1   // the chased
#define LED_PIN               12   // @@@ mostly for debugging so far
#define ULTRASND_ECHO         11   // @@@ echo pin that determines the ultrassound delay
#define ULTRASND_PIN          10   // @@@ ultrassound transmit pin (to measure distance to objects ahead)
#define STR_STD_SIZE          64   // normalized (not optimized) string size ##
#define MTR_SPEED              0   // max motor speed quantifier index
#define MTR_DIVIDER            1   // max motor speed divider index
int carNumber;                     // digit (0/1) that indicates which car is which (allows easy access to specific car configs)
bool gameIsOver = false;


// #######################
// #### Car functions ####
// #######################
float read_distance(){
  uint32_t duration;
  float distance_cm;
  
  // Clear any possible prev state
  digitalWrite(ULTRASND_PIN, LOW);
  delayMicroseconds(5);
  // Send the ultrassounds then stop
  digitalWrite(ULTRASND_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASND_PIN, LOW);
  // Read from Echo pin, aka result sound wave expressed in microseconds
  duration = pulseIn(ULTRASND_ECHO, HIGH);
  // Calculate distance (using speed of sound, divided by 2 because it goes forth & back)
  // Adding 3% to the result, to get a bit more realistic result
  distance_cm = (duration * 0.034 / 2) * 1.03;
  return distance_cm;
}

// ##### CAR MOVEMENT FUNCTIONS #####
void move_straight(int direction, int distance, int speed){
  // for the moment, movement takes ownership of arduino for simplicity
  int adjustedSpeed;
  int forward  = (direction == 1) ? HIGH : LOW;
  int backward = (direction == 1) ? LOW : HIGH;

  // set LED (to make movement "visible")
  digitalWrite(13, (direction == 1)? HIGH : LOW);
  Serial.println("Moving...");
  // cycle through motors to handle movement specs
  for (int m = 0; m < MTR_NUM; m++){
    adjustedSpeed = (speed / cars[carNumber][m][direction][MTR_DIVIDER]) * cars[carNumber][m][direction][MTR_SPEED]; // calc adjusted speed based on parameters
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

  //blink(13, 2, 100);                                                          // movement stop visual indicator
}

void rotate_car(int direction, int angle){
  // Directions:
  //   0 => LEFT  (left motor flows backwards; right motor flows forward)
  //   1 => RIGHT (left motor flows forward; right motor flows backwards)
  int speed;

  // loads specific speed information depending on the car
  if (carNumber == TOM){
    speed = 65;
  } else if (carNumber == JERRY) {
    speed = 80;
  } else {
    // just a protection. stops the car and lets us know in case something goes wrong
    speed = 1;
  }

  // for the moment, movement takes ownership of arduino for simplicity
  int adjustedSpeed;
  int forward = (direction == 1) ? HIGH : LOW;
  int backward = (direction == 1) ? LOW : HIGH;
  
  // Previously used for calculation: (1.0* angle/180)*720 (three simple rule)
  // However, this new method uses a single multiplication, much lighter to Arduino
  uint16_t rotateDuration = (4 * angle);
  //Serial.println("Moving...");

  // cycle through motors to handle movement specs
  for (int m = 0; m < MTR_NUM; m++){
    adjustedSpeed = (1.0 * speed / cars[carNumber][m][direction][MTR_DIVIDER]) * cars[carNumber][m][direction][MTR_SPEED]; // calc adjusted speed based on parameters
    
    //Serial.print("Motor"); Serial.print(m);
    //Serial.print("  adjustedSpeed = "); Serial.println(adjustedSpeed);
    
    analogWrite(motors[m].pinSpeed, adjustedSpeed);                           // set motor adjusted speed
    digitalWrite(motors[m].pinFwd, forward  );                                // set motor's forward pin value
    digitalWrite(motors[m].pinBkw, backward );                                // set motor's bakward pin value
    // next motor should use opposite forward value
    forward = !forward;
    // next motor should use opposite backward value
    backward = !backward;
  }

  // "rotateDuration" will translate into the respective degrees
  delay(rotateDuration);                                                     // potential improvement in the future ##
  Serial.println("...Stopping!");
  
  // undo everything
  for (int m = 0; m < MTR_NUM; m++){
    digitalWrite(motors[m].pinSpeed , LOW   );
    digitalWrite(motors[m].pinFwd   , LOW   );
    digitalWrite(motors[m].pinBkw   , LOW   );
  }

  //blink(13, 2, 100);                                                          // movement stop visual indicator
}
// ####

void blink(int led, int times, int duration){
  for (int n = 0; n < times; n++){
    digitalWrite(led, HIGH);
    delay(duration);
    digitalWrite(led, LOW);
    delay(duration);
  }
}

void setup(){
  // initialize Serial comms baudrate
  Serial.begin(19200);
  
  // define pins direction
  for (int m = 0; m < MTR_NUM; m++){
    pinMode (motors[m].pinSpeed , OUTPUT);
    pinMode (motors[m].pinFwd   , OUTPUT);
    pinMode (motors[m].pinBkw   , OUTPUT);
  }

  //inventoryTag = getInventoryTag(text, true);
  carNumber = (EEPROM.read(EEPROM.length()-1)) - 48;           // Load car# from EEPROM (the very last byte written)
  Serial.println("\n\n\n==============================");
  Serial.print("Car Model No.: "); Serial.println(carNumber);
  
  // ## @@@ #######################
  // Ultrassound pins configuration - change accordingly to car
  if (carNumber == TOM) {
    Serial.println("Hello, Tom");
    //setup_tom();
  } else if (carNumber == JERRY) {
    Serial.println("Hello, Jerry");
    //setup_jerry();
  }

  //@@@TODO
  pinMode(ULTRASND_PIN, OUTPUT);
  pinMode(ULTRASND_ECHO, INPUT);
  pinMode(LED_PIN, OUTPUT);

  demoCourse();
}

void demoCourse() {

  // **** FOR DEMONSTRATION PURPOSES **** //
  if (carNumber == TOM) {
    // delay para setup do carro e permitir o Jerry avançar
    delay(1500);
    move_straight(1, 2, 100);
    delay(1500);
    rotate_car(0, 90);
    delay(1500);
    move_straight(1, 1, 100);
    delay(1500);
    rotate_car(0, 90);
    delay(1500);
    move_straight(1, 2, 100);
    delay(1500);
    rotate_car(1, 360);
     
  } else if (carNumber == JERRY) {
    // delay para setup do carro
    delay(1500);
    move_straight(1, 2, 100);
    delay(1500);
    rotate_car(0, 90);
    delay(1500);
    move_straight(1, 1, 100);
    delay(1500);
    rotate_car(0, 90);
    delay(1500);
    move_straight(1, 2, 100);
    delay(1500);
    rotate_car(1, 360);
    
  }
}

void loop()
{
  // ### TEST FUNCTIONS HERE ###
  //delay(2500);
  // ########################
  // TOM   values
  //move_straight(0, 3, 100);
  //rotate_car(0, 70, 360);
  //rotate_car(1, 70, 360);
  // ########################
  // JERRY values
  //move_straight(0, 3, 100);
  //rotate_car(0, 70, 360);
  //rotate_car(1, 70, 360);
  // ########################

  /*
  float distance_cm = read_distance();
  Serial.println(distance_cm);
  if (distance_cm <= 15.0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  */
  
  /* GAME HAPPENS HERE */
  /*if (carNumber == TOM) {
    Serial.println("Executing TOM round...");
    if (!gameIsOver) {
      gameIsOver = execute_tom();
    } else {
      Serial.println("Game is over, Tom is stopped");
    }
  } else if (carNumber == JERRY) {
    Serial.println("Executing JERRY round...");
    execute_jerry();
  } else {
    // for debugging purposes
    Serial.println("Unknown car model number!! Nothing to do.");
  }*/
  
}


/*
  VALORES ENCONTRADOS:
  (0) TOM   (TRANSPARENTE)
   - FRENTE       (direction 1): right: 250; left: 255
   - MARCHA-ATRAS (direction 0): right: 230; left: 25
   - ROTACAO:     rotate_car(1, 70, 90);
   
  (1) JERRY (CASTANHO)
   - ROTACAO:     rotate_car(1, 1, 1); 
*/
