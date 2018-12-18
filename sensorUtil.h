#include "Arduino.h"
#include "Wire.h"
#include "config.h"
#include "EEPROM.h"

/***********************************************
 * Funktionsliste :
 * void readUs()
 * int readCompass()
 * int readPitch()
 * int readRollAngel()
 ***********************************************/

void readUs() {
  while(Serial3.available() > 0){
    Serial3.read();
  }
  digitalWrite(usInterrupt,HIGH);
  delayMicroseconds(5);
  digitalWrite(usInterrupt,LOW);
  delay(5);
  for(int i = 0; i<4; i++){
    if(Serial3.available() > 0){
      usWerte[i] = Serial3.read(); 
    }else{
      usWerte[i]=254; 
    }
  
    if(usWerte[i] == 0){
      usWerte[i] = 254;  
    }
  }
}

void readUs(bool us0, bool us1, bool us2, bool us3) {
  while(Serial3.available() > 0){
    Serial3.read();
  }
  digitalWrite(usInterrupt,HIGH);
  delayMicroseconds(5);
  digitalWrite(usInterrupt,LOW);
  delay(5);
  
  if(us0){
    usWerte[0] = Serial3.read(); 
  }else{
    Serial3.read();
  }
  
  if(us1){
    usWerte[1] = Serial3.read();
  }else{
    Serial3.read();
  }
  
  if(us2){
    usWerte[2] = Serial3.read();    
  }else{
    Serial3.read();
  }
  
  if(us3){
    usWerte[3] = Serial3.read();
  }else{
    Serial3.read();
  }
  
}

void loadUsEeprom(){
  for(int i=0;i<4;i++){
    usStd[i] = EEPROM.read(i);
  }
}

void writeUsEeprom(){
  for(int i=0;i<4;i++){
    EEPROM.write(i, usWerte[i]);
  }
}

int readCompass(){
  
  int value = 0;   
  Wire.beginTransmission(0xC0>>1); // instruct sensor to read echoes
  Wire.write(2);      
  Wire.write(3);   
  Wire.endTransmission();   

  Wire.requestFrom(0xC0>>1, 2);    // request 2 bytes
  if(2 <= Wire.available()) {    
    value = Wire.read() << 8;
    value |= Wire.read();  
  }
  return value;
  
  /*
  int value = 0;   
  Wire.beginTransmission(0xC0>>1); // instruct sensor to read echoes
  Wire.write(1);   
  Wire.endTransmission();   

  delay(10);
  
  Wire.requestFrom(0xC0>>1, 1);    // request 2 bytes
  if(1 <= Wire.available()) {    
    value = Wire.read();
  }
  return map(value,0,255,0,3600);
  */
}

int readPitch(){
  int value = 0;   
  Wire.beginTransmission(0xC0>>1); // instruct sensor to read echoes
  Wire.write(4);   
  Wire.endTransmission();   

  Wire.requestFrom(0xC0>>1, 1);    // request 2 bytes
  if(1 <= Wire.available()) {    
    value = Wire.read();
  }
  return value; 
}

int readRollAngel(){
  int value = 0;   
  Wire.beginTransmission(0xC0>>1); // instruct sensor to read echoes
  Wire.write(5);   
  Wire.endTransmission();   

  Wire.requestFrom(0xC0>>1, 1);    // request 2 bytes
  if(1 <= Wire.available()) {    
    value = Wire.read();
  }
  return value; 
}
 
void loadCompassEeprom(){
  kompassStart = (EEPROM.read(4)*256);
  kompassStart += EEPROM.read(5);
}

void writeCompassEeprom(){
  EEPROM.write(4,kompassStart/256);
  EEPROM.write(5,kompassStart%256);
}
