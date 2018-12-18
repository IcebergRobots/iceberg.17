#include "Arduino.h"
#include "Wire.h"
#include "config.h"

//-- Debugging
#include "LiquidCrystal.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
/***********************************************
* Funktionsliste :
* void iteriereKreis()
* void blink()
* void fillMatrix()
* void clearMatrix()
* 
***********************************************/

void iteriereKreis(){
  for(int i=0;i<16;i++){
    matrix.clear();  
    matrix.drawCircle(3,3, 3, LED_ON);
    matrix.drawPixel(matrixAbweichungX[i],matrixAbweichungY[i],LED_OFF);
    matrix.writeDisplay();  // write the changes we just made to the display
    delay(20);     
  } 
}

void blink(){
  for(int i=0;i<3;i++){
      digitalWrite(greenLeds[i],HIGH);
      delay(500);
  }
  for(int i=0;i<3;i++){
      digitalWrite(greenLeds[i],LOW);
      delay(500);
  }
}

void fillMatrix(){
  matrix.clear();
  for(int i=0; i<8;i++){
    matrix.drawRect(0,0,i+1,i+1, LED_ON);
  }
  matrix.writeDisplay();
}

void clearMatrix(){
  matrix.clear();
  for(int i=0; i<8;i++){
    matrix.drawRect(0,0,8,8, LED_OFF);
  }
  matrix.writeDisplay();
}

void keuleMatrix(){
  matrix.clear();
  for(int i=1; i<7;i++){
    matrix.drawRect(i,i, i+1,i+1, LED_ON);
  }
  matrix.writeDisplay();
}
