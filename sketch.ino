#include <ESC.h>

#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200

ESC RESC(9, 1000, 2000, 500);
ESC LESC(10, 1000, 2000, 500);

int potValue;  // value from the analog pin

void setup() {
   motorCalibration();
   motorArm();
   Serial.begin(9600);
}

void loop() {
   testRight();
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
   RESC.speed(speed);
   LESC.speed(speed);
}

void turnLeft(int speed){
   RESC.speed(2*speed);
   LESC.speed(speed);
}

void turnRight(int speed) {
   RESC.speed(speed);
   LESC.speed(2*speed);
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

int serialSignal(){
   while(!Serial.available())
      ;
   int signal = Serial.readString().toInt();
   return signal;
}

void runBoat(){
   int cs = serialSignal();

   switch (cs){
      case 0:
         Serial.println("Running forward...");
         runForward(20);
         break;
      
      case 1:
         Serial.println("Turning right...");
         turnRight(10);
         break;

      case 2:
         Serial.println("Turning left...");
         turnLeft(10);
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