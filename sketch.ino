#include <ESC.h>

#define LED_PIN (13) 
#define dirPin 2
#define stepPin 3
#define EN 11
#define stepsPerRevolution 200

ESC RESC(9, 1000, 2000, 500);
ESC LESC(10, 1000, 2000, 500);

int potValue;  // value from the analog pin
int selection=4;
int oESC;
int currentSpeed;

void setup() {
   pinMode(LED_PIN, OUTPUT); 
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin, OUTPUT);
   pinMode(EN, OUTPUT);
   digitalWrite(EN, LOW);
   motorArm();
   Serial.begin(9600);
   digitalWrite(LED_PIN, HIGH);
   accelerate();
   delay(1000);
   deacellerate();
   delay(1000);
   accelerate();
   delay(1000);
   deacellerate();
   delay(1000);
}

void loop() {
   runForward(1200);
   delay(10000);
   testShredderForward();
   delay(10000);
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
  return 500;
}

void testRight(){
   RESC.speed(1200);
}

void testLeft(){
   LESC.speed(1200);
}

void testShredderForward(){
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

void testShredderBackward(){
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
   LESC.calib();
   RESC.calib();
   RESC.stop();
   LESC.stop();
}

void motorArm(){
   RESC.arm();
   LESC.arm();
   delay(5000);
}

void accelerate(){
  for (oESC = 1000; oESC <= 1100; oESC += 1) {  // goes from 1000 microseconds to 2000 microseconds
      RESC.speed(oESC);                                    // tell ESC to go to the oESC speed value
      LESC.speed(oESC);
      currentSpeed = oESC;
      delay(10);                                            // waits 10ms for the ESC to reach speed
  }
}

void deacellerate(){
  for (oESC = 1200; oESC >= 1000; oESC -= 1) {  // goes from 2000 microseconds to 1000 microseconds
    RESC.speed(oESC);                                    // tell ESC to go to the oESC speed value
    LESC.speed(oESC); 
    delay(10);                                            // waits 10ms for the ESC to reach speed  
   }
}

void runForward(int speed){
   accelerate();
   delay(10);
}

void turnLeft(int speed){
   RESC.speed(1.5*speed);
   LESC.speed(speed);
}

void turnRight(int speed) {
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