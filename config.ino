#include "config.h"

void pinModes(){
  //-- Led Pins
  pinMode(redLed1,OUTPUT);
  //pinMode(redLed2,OUTPUT);
  pinMode(redLed3,OUTPUT);
  pinMode(greenLed1,OUTPUT);
  pinMode(greenLed2,OUTPUT);
  pinMode(greenLed3,OUTPUT);

  //-- Schalter Pins
  pinMode(kalibrierenKnopf,INPUT_PULLUP);
  pinMode(schalter1,INPUT_PULLUP);
  pinMode(schalter2,INPUT_PULLUP);
  pinMode(schalter3,INPUT_PULLUP);
  pinMode(topSchalter1,INPUT_PULLUP);
  pinMode(topSchalter2,INPUT_PULLUP);
  pinMode(topSchalter3,INPUT_PULLUP);
  pinMode(topSchalter4,INPUT_PULLUP);
  pinMode(topSchalter5,INPUT_PULLUP);

  //-- Motor Pins
  pinMode(fwd1,OUTPUT);
  pinMode(fwd2,OUTPUT);
  pinMode(fwd3,OUTPUT);
  
  pinMode(bwd1,OUTPUT);
  pinMode(bwd2,OUTPUT);
  pinMode(bwd3,OUTPUT);

  pinMode(pwm1,OUTPUT);
  pinMode(pwm2,OUTPUT);
  pinMode(pwm3,OUTPUT);
  
  //-- Lichtschranke
  pinMode(lichtschranke, INPUT_PULLUP);

  //-- Buzzer Pin
  pinMode(buzzer,OUTPUT);
  pinMode(lautsprecher,OUTPUT);
  
  
  //-- Poti Pins
  pinMode(potiTop,INPUT);
  pinMode(potiBoard,INPUT);

  //-- Interrupt Pin
  pinMode(irInterrupt,OUTPUT);
  pinMode(usInterrupt,OUTPUT);
  pinMode(bodenInterrupt, INPUT);

  //-- Schuss
  pinMode(schuss,OUTPUT);

  //-- Dribbler
  pinMode(dribbler,OUTPUT);

  //-- SerialFixes
  pinMode(15,INPUT_PULLUP);
  pinMode(17,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);
}
void configMotors(){  
  //Hier werden dem Chassis-Objekt die einzelnen Motoren eingespeichert
  
  motors.setMotorGeometrie(0, angel1);   // Achse 26° (vorne rechts)
  motors.setMotorGeometrie(1, angel2);  // Achse 146° (vorne links)
  motors.setMotorGeometrie(2, angel3);  // Achse 270° (hinten)
  
  motors.setMotorPINS(0,fwd1,bwd1,pwm1);   // (id, fwd-Pin, bwd-Pin, pwm-Pin) Motor 2 auf Platine
  motors.setMotorPINS(1,fwd2,bwd2,pwm2);   // (id, fwd-Pin, bwd-Pin, pwm-Pin) Motor 1 auf Platine
  motors.setMotorPINS(2,bwd3,fwd3,pwm3);   // (id, fwd-Pin, bwd-Pin, pwm-Pin) Motor 3 auf Platine

  int myEraser = 7;             // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;          // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  TCCR1B &= ~myEraser;

  int myPrescaler = 1;         // this could be a number in [1 , 6]. In this case, 3 corresponds in binary to 011.   
  TCCR2B |= myPrescaler;       //this operation (0R), replaces the last three bits in TCCR2B with our new value 011
  TCCR1B |= myPrescaler;  

  turn_l[0] =   60;
  turn_l[1] =   60;
  turn_l[2] =   60;

  turn_r[0] =  -60;
  turn_r[1] =  -60;
  turn_r[2] =  -60;

  vor[0]  =    200;
  vor[1]  =   -200;
  vor[2]  =      0;

  rueck[0] =  -150;
  rueck[1] =   150;
  rueck[2] =     0;

  rueckFast[0] = -213;
  rueckFast[1] =  200;
  rueckFast[2] =    0;

  rechts[0] = - 76;
  rechts[1] = -133;
  rechts[2] =  244;

  hrechts[0] = -200;
  hrechts[1] =    0;
  hrechts[2] =  200;

  links[0] =    76;
  links[1] =   133;
  links[2] =  -244;

  hlinks[0] =   200;
  hlinks[1] =     0;
  hlinks[2] =  -200;    

  rambo[0] = 255;
  rambo[1] = -255;
  rambo[2] = 0;
  

}
