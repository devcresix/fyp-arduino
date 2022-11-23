#include <ESC.h>

#define dirPin 2
#define stepPin 3
#define EN 10
#define stepsPerRevolution 200

ESC RESC(9, 1000, 2000, 500);
ESC LESC(10, 1000, 2000, 500);

int potValue;  // value from the analog pin
int selection=4;

void setup() {
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin, OUTPUT);
   pinMode(EN, OUTPUT);
   digitalWrite(EN, LOW);
   motorCalibration();
   motorArm();
   Serial.begin(9600);

}

void loop() {
   runForward(1200);
   delay(15);
}

/////////////////tests////////////////////////////////////////
int potValForBLDC(){
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 1000, 2000);   // scale it to use it with the servo library (value between 0 and 180)
  Serial.print("BLDC SPEED: ");
  Serial.println(potValue);
  return potValue;
}

int potValForStepper(){
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 500, 5000);   // scale it to use it with the servo library (value between 500 and 5000)
  Serial.print("Stepper SPEED: ");
  Serial.println(potValue);
  return potValue;
}

void testRight(){
   int speed = potValForBLDC();
   RESC.speed(speed);
}

void testLeft(){
   int speed = potValForBLDC();
   LESC.speed(speed);
}

void testShredderForwarrd(){
   int speed = potValForStepper();
   digitalWrite(dirPin, HIGH);
   for (int i = 0; i < 10 * stepsPerRevolution; i++) {
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(speed);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(speed);
   }
}

void testShredderBackwarrd(){
   int speed = potValForStepper();
   digitalWrite(dirPin, LOW);
   for (int i = 0; i < 10 * stepsPerRevolution; i++) {
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(speed);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(speed);
   }
}
/////////////////tests////////////////////////////////////////

////////////////Functions/////////////////////////////////////
void motorCalibration(){
   RESC.calib();
   LESC.calib();
   RESC.stop();
   LESC.stop();
}

void motorArm(){
   RESC.arm();
   LESC.arm();
}

void runForward(int speed){
   motorArm();
   RESC.speed(speed);
   LESC.speed(speed);
}

void turnLeft(int speed){
   motorArm();
   RESC.speed(1.5*speed);
   LESC.speed(speed);
}

void turnRight(int speed) {
   motorArm();
   RESC.speed(speed);
   LESC.speed(1.5*speed);
}

void shred(boolean isForward, int revs, int speed){
   digitalWrite(dirPin, isForward);
   for (int i = 0; i < revs * stepsPerRevolution; i++) {
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(speed);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(speed);
   }
}

void idle(){
   RESC.stop();
   LESC.stop();
   digitalWrite(stepPin, LOW);
   digitalWrite(dirPin, LOW);
}

void serialSignal(){
   while(!Serial.available())
      ;
   int signal = Serial.readString().toInt();
   if(selection!=signal){
      selection = signal;
   }
}

void runBoat(){
   serialSignal();
   switch (selection){
      case 0:
         Serial.println("Running forward...");
         runForward(1200);
         break;
      
      case 1:
         Serial.println("Turning right...");
         turnRight(1200);
         break;

      case 2:
         Serial.println("Turning left...");
         turnLeft(1200);
         break;

      case 3:
         Serial.println("Shredding...");
         shred(true,10,1000);
         break;
      
      default:
         Serial.println("Idle...");
         idle();
         break;
   }
}

////////////////Functions/////////////////////////////////////