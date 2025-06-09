#include <AFMotor.h>
#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <Servo.h>

MPU6050 mpu;
SoftwareSerial BTSerial(9, 10);  // Adjust if you're using other pins
AF_DCMotor BLmotor(4);
AF_DCMotor TLmotor(3);
AF_DCMotor BRmotor(1);
AF_DCMotor TRmotor(2);
Servo LeftClaw;
Servo RightClaw;
int speed = 255;
int leftClawAngle = 90;
int rightClawAngle = 90;
char lastCommand = 'X'; // Track last received command
unsigned long lastClawUpdate = 0;
bool slowspeed = false;
const int clawSpeed = 10; // Degrees to change per update

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);  // Make sure this matches your Bluetooth baud rate
  Wire.begin();
  mpu.initialize();
  LeftClaw.attach(A1);
  RightClaw.attach(A0);

  moveServo(1, leftClawAngle);
  moveServo(2, rightClawAngle);

  Serial.println("HC-05 Bluetooth Test");
  BTSerial.println("Hello from Arduino!");

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected");
  } else {
    Serial.println("MPU6050 connection failed");
  }
  StopMotors();
}

void MoveForward() {
  BLmotor.setSpeed(speed);
  TLmotor.setSpeed(speed);
  BRmotor.setSpeed(speed);
  TRmotor.setSpeed(speed);
  BLmotor.run(FORWARD);
  TLmotor.run(FORWARD);
  BRmotor.run(FORWARD);
  TRmotor.run(FORWARD);
}

void MoveBackward() {
  BLmotor.setSpeed(speed);
  TLmotor.setSpeed(speed);
  BRmotor.setSpeed(speed);
  TRmotor.setSpeed(speed);
  BLmotor.run(BACKWARD);
  TLmotor.run(BACKWARD);
  BRmotor.run(BACKWARD);
  TRmotor.run(BACKWARD);
}

void TurnLeft() {
  BLmotor.setSpeed(speed);
  TLmotor.setSpeed(speed);
  BRmotor.setSpeed(speed);
  TRmotor.setSpeed(speed);
  BLmotor.run(BACKWARD);
  TLmotor.run(BACKWARD);
  BRmotor.run(FORWARD);
  TRmotor.run(FORWARD);
}

void TurnRight() {
  BLmotor.setSpeed(speed);
  TLmotor.setSpeed(speed);
  BRmotor.setSpeed(speed);
  TRmotor.setSpeed(speed);
  BLmotor.run(FORWARD);
  TLmotor.run(FORWARD);
  BRmotor.run(BACKWARD);
  TRmotor.run(BACKWARD);
}

void StopMotors() {
  BLmotor.run(RELEASE);
  TLmotor.run(RELEASE);
  BRmotor.run(RELEASE);
  TRmotor.run(RELEASE);
}

void moveServo(int servoNum, int angle) {
  angle = constrain(angle, 0, 180);

  if (servoNum == 1) {
    LeftClaw.write(angle);
    leftClawAngle = angle;
  } else if (servoNum == 2) {
    RightClaw.write(angle);
    rightClawAngle = angle;
  }
}

void loop() {
  char command = 'X';
  
  if (BTSerial.available()) {
    command = BTSerial.read();
    Serial.print("Received: ");
    Serial.println(command);

    // Handle immediate commands (movement and stop)
    if (command != lastCommand || command == 'X') {
      if (command == 'W') MoveForward();
      else if (command == 'S') MoveBackward();
      else if (command == 'A') TurnLeft();
      else if (command == 'D') TurnRight();
      else if (command == 'X') StopMotors();
      else if (command == 'K') {
        if (slowspeed == true) {
          speed = 255;
          slowspeed = false;
        }
        else if (slowspeed == false) {
          speed = speed * 0.55;
          slowspeed = true;
        }
      }
      
      lastCommand = command;
    }
  } else {
    command = lastCommand; // Keep using the last command if no new one
  }
  
  // Handle continuous claw movement (every 50ms)
  unsigned long currentTime = millis();
  if (currentTime - lastClawUpdate >= 50) {
    lastClawUpdate = currentTime;
    
    // Close claw gradually
    if (command == 'C') {
      // Increase angle for left claw (closing)
      if (leftClawAngle < 180) {
        leftClawAngle += clawSpeed;
        moveServo(1, leftClawAngle);
      }
      
      // Decrease angle for right claw (closing)
      if (rightClawAngle > 0) {
        rightClawAngle -= clawSpeed;
        moveServo(2, rightClawAngle);
      }
    }
    // Open claw gradually
    else if (command == 'G') {
      // Decrease angle for left claw (opening)
      if (leftClawAngle > 0) {
        leftClawAngle -= clawSpeed;
        moveServo(1, leftClawAngle);
      }
      
      // Increase angle for right claw (opening)
      if (rightClawAngle < 180) {
        rightClawAngle += clawSpeed;
        moveServo(2, rightClawAngle);
      }
    }
  }
}