/**
 * MotorController
 * ME421 Winter 2011-12
 * Author: Jason Block
 * Team Members: Andrew Jordan, Trenton Tabor, Thomas Bryce
 */

#define START 0
#define LEFT 1
#define RIGHT 2
#define STOP 3
#define GET 4
#define INITVAL 128

#define TOGGLE = 1

// From Kinect's perspective
#define LEFT_PHOTOGATE_PIN 8
#define LEFT_MID_PHOTOGATE_PIN 9
#define RIGHT_MID_PHOTOGATE_PIN 10
#define RIGHT_PHOTOGATE_PIN 11

#define MOTOR_POWER_PIN 12
#define MOTOR_DIRECTION_PIN 13

// Bits
#define MOVING_LEFT_BIT 0
#define MOVING_RIGHT_BIT 1
#define RIGHT_PHOTOGATE_BIT 2
#define RIGHT_MID_PHOTOGATE_BIT 3
#define LEFT_MID_PHOTOGATE_BIT 4
#define LEFT_PHOTOGATE_BIT 5
#define SWITCHING_DIRECTIONS 6
#define EXPERIMENT_STARTED 7

char prevCommand, currentCommand, currentMotorState;
int sensors; // 16 bit integer that gets beamed back to the mothership
unsigned long prevTime, curTime;

void setup() {
  Serial.begin(19200);
  prevCommand = INITVAL;
  currentMotorState = STOP;
  sensors = 0; // B00000000
  prevTime = 0;
  curTime = 0;
  initializePins();
  moveRight();
  Serial.println(sensors);
  bitWrite(sensors,EXPERIMENT_STARTED,1);
  currentMotorState = RIGHT;
}

void loop() {
  char n = updateSensors();
  Serial.println(sensors,BIN);
  Serial.flush();
  if (n == 1 && Serial.available() > 0) {
    currentCommand = Serial.read() - '0';
    Serial.flush();
    if (prevCommand != currentCommand) {
      //currentCommand == GET ? sendData() : processCommand(prevCommand = currentCommand);
      if (currentCommand == GET) {
        sendData(); 
      } 
      prevCommand = currentCommand;
      //processCommand(currentCommand);
    }
  }
  Serial.print("\r");
  //
  //Serial.println(sensors);
  //  curTime = millis();
  //  if (curTime - prevTime > (long)(1000)) { // 1 second
  //    Serial.println(sensors);
  //    prevTime = curTime;
  //  }
}

void initializePins() {
  // INPUTS
  pinMode(LEFT_PHOTOGATE_PIN, INPUT);
  pinMode(LEFT_MID_PHOTOGATE_PIN, INPUT);
  pinMode(RIGHT_MID_PHOTOGATE_PIN, INPUT);
  pinMode(RIGHT_PHOTOGATE_PIN, INPUT);

  // OUTPUTS
  pinMode(MOTOR_POWER_PIN, OUTPUT);
  pinMode(MOTOR_DIRECTION_PIN, OUTPUT); 
}


int updateSensors() {
  // iterate through each photogate
  // verbose? yes. Functional? You betcha. 
  int c = 1;
  //Serial.println("Updating Sensors");
  bitWrite(sensors,RIGHT_PHOTOGATE_BIT,digitalRead(RIGHT_PHOTOGATE_PIN));
  bitWrite(sensors,RIGHT_MID_PHOTOGATE_BIT,digitalRead(RIGHT_MID_PHOTOGATE_PIN));
  bitWrite(sensors,LEFT_MID_PHOTOGATE_BIT,digitalRead(LEFT_MID_PHOTOGATE_PIN));
  bitWrite(sensors,LEFT_PHOTOGATE_BIT,digitalRead(LEFT_PHOTOGATE_PIN));
  bitWrite(sensors,SWITCHING_DIRECTIONS,0);
  if (bitRead(sensors, RIGHT_PHOTOGATE_BIT) == LOW) {
    //Serial.println("RIght Photogate is LOW");
    bitWrite(sensors,RIGHT_PHOTOGATE_BIT,0);
    if (bitRead(sensors,MOVING_RIGHT_BIT)) {
      //Serial.println("It's moving right. switch directions.");
      processCommand(LEFT);
      bitWrite(sensors, SWITCHING_DIRECTIONS, 1);
      c = 0;
    }
    c = 1;
  } 
  //  else {
  //    bitWrite(sensors,RIGHT_PHOTOGATE_BIT,1);
  //    c = 1;
  //  }
  //  if (digitalRead(RIGHT_MID_PHOTOGATE_PIN) == LOW) {
  //        Serial.println("RIght Mid Photogate is LOW");
  //    bitWrite(sensors,RIGHT_MID_PHOTOGATE_BIT,0);
  //    bitWrite(sensors,SWITCHING_DIRECTIONS,0);
  //    c = 1;
  //  } 
  //  else {
  //        Serial.println("RIght mid Photogate is HIGH");
  //    bitWrite(sensors,RIGHT_MID_PHOTOGATE_BIT,1);
  //     bitWrite(sensors,SWITCHING_DIRECTIONS,0);
  //    c = 1;
  //  }
  //  if (digitalRead(LEFT_MID_PHOTOGATE_PIN) == LOW) {
  //        Serial.println("Left mid Photogate is LOW");
  //    bitWrite(sensors,LEFT_MID_PHOTOGATE_BIT,0);
  //    bitWrite(sensors,SWITCHING_DIRECTIONS,0);  
  //    c = 1;
  //  } 
  //  else {
  //        Serial.println("left mid Photogate is HIGH");
  //    bitWrite(sensors,LEFT_MID_PHOTOGATE_BIT,1);
  //    bitWrite(sensors,SWITCHING_DIRECTIONS,0);  
  //    c = 1;
  //  }
  if (bitRead(sensors, LEFT_PHOTOGATE_BIT) == LOW) {
    // Serial.println("Left Photogate is low");
    bitWrite(sensors,LEFT_PHOTOGATE_BIT,0);
    if (bitRead(sensors,MOVING_LEFT_BIT)) {
      processCommand(RIGHT);
      bitWrite(sensors, SWITCHING_DIRECTIONS, 1);
      c = 0;
    }
    c = 1;
  } 
  //  else {
  //        Serial.println("Left Photogate is HIGH");
  //    bitWrite(sensors,LEFT_PHOTOGATE_BIT,1);
  //    c = 1;
  //  }
  return c;
}

void processCommand(char command) {
  switch(currentMotorState) { 
  case LEFT:
    switch(command) {
    case RIGHT:
      stopMotor();
      //Serial.write("Delay for 1 second\n");
      delay(100);
      moveRight(); 
      break;
    case STOP:
      stopMotor();
      break;
    }
    break; 
  case RIGHT:
    switch(command) {
    case LEFT:
      stopMotor();
      //Serial.write("Delay for 1 second\n");
      delay(100);
      moveLeft(); 
      break;
    case STOP:
      stopMotor();
      break;
    }
    break;
  case STOP:
  case INITVAL:
    switch(command) {
    case LEFT:
      moveLeft();
      break;
    case RIGHT:
      moveRight();
      break;
    }
    break;
  }
}

void sendData() {
  Serial.println(sensors); 
}

void printCurrentMotorState() {
  Serial.write("Current direction: ");
  if (currentMotorState == LEFT) {
    Serial.write("Left");
  } 
  else if (currentMotorState == RIGHT) {
    Serial.write("Right"); 
  } 
  else {
    Serial.write("Not moving. Off"); 
  }
  Serial.write("\n");
}

void stopMotor() {
  //Serial.write("Stop Motors\n"); 
  currentMotorState = STOP;
  digitalWrite(MOTOR_POWER_PIN,HIGH);
  //sensors = sensors & 252;
  bitWrite(sensors,MOVING_LEFT_BIT,0);
  bitWrite(sensors,MOVING_RIGHT_BIT,0);
}

void moveRight() {
  //Serial.write("Move to the right\n");
  currentMotorState = RIGHT;
  digitalWrite(MOTOR_DIRECTION_PIN,LOW);
  delay(100);
  digitalWrite(MOTOR_POWER_PIN,LOW);
  bitWrite(sensors,MOVING_RIGHT_BIT,1);
  bitWrite(sensors,MOVING_LEFT_BIT,0);
}

void moveLeft() {
  //Serial.write("Move to the left\n");
  currentMotorState = LEFT;
  digitalWrite(MOTOR_DIRECTION_PIN,HIGH);
  delay(100);
  digitalWrite(MOTOR_POWER_PIN,LOW);
  bitWrite(sensors,MOVING_RIGHT_BIT,0);
  bitWrite(sensors,MOVING_LEFT_BIT,1);
}





