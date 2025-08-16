#include <Servo.h>

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripperServo;

int baseAngle = 90;
int shoulderAngle = 90;
int elbowAngle = 90;
int gripperAngle = 40;

const int baseHome = 90;
const int shoulderHome = 90;
const int elbowHome = 90;
const int gripperDefault = 40;

const int shoulderDown = 60;
const int shoulderUp = 120;
const int elbowDown = 80;
const int elbowUp = 20;
const int baseTarget = 150;
const int gripperOpen = 50;
const int gripperClosed = 100;

const int basePin = 9;
const int shoulderPin = 10;
const int elbowPin = 11;
const int gripperPin = 12;

#define ENA 3
#define IN1 4
#define IN2 5
#define ENB 6
#define IN3 7
#define IN4 8

#define TRIG_PIN 2
#define ECHO_PIN 13
#define OBSTACLE_DISTANCE 15

int forwardTime = 2000;
int rightTurnTime = 1000;

void setup() {
  Serial.begin(9600);
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Setup complete. Ready to start.");
  delay(1000);
}

void loop() {
  while (true) { 
    if (!detectObstacle()) moveForward(forwardTime);
    stopMotors();
    pickObject();
    moveObject();
    turnRight(rightTurnTime);
    if (!detectObstacle()) moveForward(forwardTime);
    stopMotors();
    dropObject();
    returnHome();
    turnRight(rightTurnTime);
    if (!detectObstacle()) moveForward(forwardTime);
    stopMotors();
    pickObject();
    moveObject();
    turnRight(rightTurnTime);
    if (!detectObstacle()) moveForward(forwardTime);
    stopMotors();
    dropObject();
    returnHome();
    turnRight(rightTurnTime);
    delay(3000);
  }
}

bool detectObstacle() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= OBSTACLE_DISTANCE) {
    Serial.println("Obstacle detected! Stopping movement.");
    stopMotors();
    return true;
  }
  return false;
}

void moveForward(int duration) {
  Serial.println("Moving Forward");
  if (detectObstacle()) return;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(duration);
  stopMotors();
}

void turnRight(int duration) {
  Serial.println("Turning Right");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(duration);
  stopMotors();
}

void stopMotors() {
  Serial.println("Stopping Motors");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveServoSmooth(Servo &servo, int servoPin, int &currentAngle, int targetAngle) {
  targetAngle = constrain(targetAngle, 0, 180);
  servo.attach(servoPin);
  int step = (currentAngle < targetAngle) ? 2 : -2;
  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    servo.write(angle);
    currentAngle = angle;
    delay(5);
  }
  delay(1500);
  servo.detach();
}

void pickObject() {
  Serial.println("Picking object...");
  moveServoSmooth(shoulderServo, shoulderPin, shoulderAngle, shoulderDown);
  moveServoSmooth(elbowServo, elbowPin, elbowAngle, elbowDown);
  moveServoSmooth(gripperServo, gripperPin, gripperAngle, gripperClosed);
}

void moveObject() {
  Serial.println("Lifting and moving object...");
  moveServoSmooth(shoulderServo, shoulderPin, shoulderAngle, shoulderUp);
  moveServoSmooth(elbowServo, elbowPin, elbowAngle, elbowUp);
  moveServoSmooth(baseServo, basePin, baseAngle, baseTarget);
}

void dropObject() {
  Serial.println("Dropping object...");
  moveServoSmooth(shoulderServo, shoulderPin, shoulderAngle, shoulderDown);
  moveServoSmooth(elbowServo, elbowPin, elbowAngle, elbowDown);
  moveServoSmooth(gripperServo, gripperPin, gripperAngle, gripperOpen);
}

void returnHome() {
  Serial.println("Returning to home position...");
  moveServoSmooth(shoulderServo, shoulderPin, shoulderAngle, shoulderHome);
  moveServoSmooth(elbowServo, elbowPin, elbowAngle, elbowHome);
  moveServoSmooth(baseServo, basePin, baseAngle, baseHome);
  moveServoSmooth(gripperServo, gripperPin, gripperAngle, gripperDefault);
}
