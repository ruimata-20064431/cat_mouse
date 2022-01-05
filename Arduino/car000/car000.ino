
#define MOVE_FORWARD    1
#define MOVE_BACKWARD   1

typedef struct MotorS {
  uint8_t pinSpeed;                                                             // must be PWM pins for speed control
  uint8_t pinFwd;
  uint8_t pinBkw;
  //uint8_t pinSpeed;
  //uint8_t pinDirection;                                                       // 0:stop; 1:forward; 2:backward
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


/*
// funtion to turn car in a certain angel (clockwise)
bool turn (int degrees){
  // @@@ potential update to detect shortest direction to turn
  null;
}

*/

// handle car movement in a "straight line", forward or backword
bool move(MotorsS pMotors, int distance, int direction){
  Serial.print(pMotors.motor1.pinSpeed);
  Serial.print(pMotors.motor1.pinFwd);
  Serial.println(pMotors.motor1.pinBkw);

  Serial.print(pMotors.motor2.pinSpeed);
  Serial.print(pMotors.motor2.pinFwd);
  Serial.println(pMotors.motor2.pinBkw);

  analogWrite(3, 50);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);

  //analogWrite(pMotors.motor2.pinSpeed, 100);
  //digitalWrite(pMotors.motor2.pinFwd, HIGH);
  //digitalWrite(pMotors.motor2.pinBkw, LOW);
  
  delay(2000);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(2000);

  // off
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  delay(2000);

  analogWrite(9, 50);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);

  //analogWrite(pMotors.motor2.pinSpeed, 100);
  //digitalWrite(pMotors.motor2.pinFwd, HIGH);
  //digitalWrite(pMotors.motor2.pinBkw, LOW);
  
  delay(2000);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  delay(2000);

  // off
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(2000);
  
}


void setup() {
  MotorsType motors = { {9, 7, 6},    //, 0, 0},
                        {3, 4, 5}  }; //, 0, 0} };                              // car engines

  Serial.begin(9600);

  // motor1 pin configuration: OUTPUT
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  // motor2 pin configuration: OUTPUT
  pinMode(3, OUTPUT);
  pinMode(4 , OUTPUT);
  pinMode(5, OUTPUT);
  
  //move(motors, 10, MOVE_FORWARD);

  Serial.print("Motor1: speed control = ");
  Serial.println(motors.motor1.pinSpeed);
  
  Serial.print("Motor1: forward control = ");
  Serial.println(motors.motor1.pinFwd);
  
  Serial.print("Motor1: backward control = ");
  Serial.println(motors.motor1.pinBkw);


  digitalWrite(9, 100);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  delay(3000);
  
  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(3000);
  
  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  delay(3000);


  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(10000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(9, 100);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  delay(3000);
  
  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(3000);
  
  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  delay(3000);


  digitalWrite(9, 100);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(10000);
}
