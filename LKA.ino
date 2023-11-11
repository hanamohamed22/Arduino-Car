//ARDUINO LINE FOLLOWING CAR//
// YOU HAVE TO INSTALL THE AFMOTOR LIBRARY BEFORE UPLOAD THE CODE//
// GO TO SKETCH >> INCLUDE LIBRARY >> ADD .ZIP LIBRARY >> SELECT AF MOTOR ZIP FILE //
 
//including the libraries
#include <Arduino_FreeRTOS.h>
#include <AFMotor.h>

//defining pins and variables
#define left A4
#define right A5
#define led A1
//defining motors
AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

void Task1(void *pvParameters);

void setup() {
  //declaring pin types
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  pinMode(led,OUTPUT);
  //begin serial communication
  Serial.begin(9600);
  xTaskCreate(
  Task1
  ,"Task1"
  ,128
  ,NULL
  ,1
  ,NULL);
  vTaskStartScheduler(); 
//   motor1.setSpeed(80);
//   motor2.setSpeed(80);
// motor3.setSpeed(80);  

// motor4.setSpeed(80);  
}

void loop(){
}

    // motor1.run(FORWARD);
    // motor1.setSpeed(80);
    // motor2.run(FORWARD);
    // motor2.setSpeed(80);  

//Serial.println(digitalRead(left));
// if(digitalRead(left)==0)
// {
//   motor1.run(FORWARD);
//     motor1.setSpeed(80);
//     motor2.run(FORWARD);
//     motor2.setSpeed(80); 
//     motor3.run(FORWARD);
//     motor4.run(FORWARD);
//      motor4.setSpeed(80);
//     motor3.setSpeed(80);
//     //Serial.println("m3 working");
// }

// else if(digitalRead(left)==1)
// {
//   motor1.run(FORWARD);
//     motor1.setSpeed(80);
//     motor2.run(FORWARD);
//     motor2.setSpeed(80); 
//     motor3.run(BACKWARD);
//      motor4.setSpeed(80);
//     motor4.run(BACKWARD);
//     motor3.setSpeed(80);
//     //Serial.println("m4 working");
   
// }

  //printing values of the sensors to the serial monitor
  //Serial.println(digitalRead(left));
  
  //Serial.println(digitalRead(right));

  //line detected by both
 //if(digitalRead(left)==0){
   void Task1(void *pvParameters){
  (void) pvParameters;
  while(1){
 if(digitalRead(left)==0 && digitalRead(right)==0){
    //Forward
    Serial.println("not detecting");
    motor1.run(FORWARD);
    motor1.setSpeed(80);
    motor2.run(FORWARD);
    motor2.setSpeed(80);
    motor3.run(FORWARD);
    motor3.setSpeed(80);
    motor4.run(FORWARD);
    motor4.setSpeed(80);
    digitalWrite(A1, LOW);
  }
  //line detected by left sensor
  //else{
 else if(digitalRead(left)==0 && !digitalRead(right)==0){
    //turn left
    Serial.println("left detecting");
    motor1.run(FORWARD);
    motor1.setSpeed(80);
    motor2.run(FORWARD);
    motor2.setSpeed(80);
    motor3.run(BACKWARD);
    motor3.setSpeed(200);
    motor4.run(BACKWARD);
    motor4.setSpeed(200);
    digitalWrite(A1, HIGH);
// Serial.println("done");    
    
  }
  //line detected by right sensor
  else if(!digitalRead(left)==0 && digitalRead(right)==0){
    //turn right
    Serial.println("right detecting");
    motor1.run(BACKWARD);
    motor1.setSpeed(200);
    motor2.run(BACKWARD);
    motor2.setSpeed(200);
    motor3.run(FORWARD);
    motor3.setSpeed(80);
    motor4.run(FORWARD);
    motor4.setSpeed(80);
    digitalWrite(A1, HIGH);
   
  }
  //line detected by none
  else if(!digitalRead(left)==0 && !digitalRead(right)==0){
  //   //stop
   Serial.println("whatever");
    motor1.run(RELEASE);
    motor1.setSpeed(0);
    motor2.run(RELEASE);
    motor2.setSpeed(0);
    motor3.run(RELEASE);
    motor3.setSpeed(0);
    motor4.run(RELEASE);
    motor4.setSpeed(0);
    digitalWrite(A1, HIGH);
   
  }

  
}
   }


