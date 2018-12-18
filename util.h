#include "Arduino.h"
#include "Wire.h"
#include "config.h"

/***********************************************
 * Funktionsliste :0
 * int  abweichung (int a,int b)
 * int  funktionRotation (int x)
 * void turnLow (int array[])
 * void turnHigh (int array[])
 * void updateLeds(byte dirBall)
 ***********************************************/

int abweichung(int a, int b){
    int diff = b-a;
    if (diff>1800){
      diff = -(3600-diff);
    }
    else if (diff<-1800){
      diff = 3600+diff;
    }
    return diff;
}

int abweichungAdd(int newAbw){
  abweichungHistory[2] = abweichungHistory[1];
  abweichungHistory[1] = abweichungHistory[0];
  abweichungHistory[0] = newAbw;
  return (abweichungHistory[2]+abweichungHistory[1]+abweichungHistory[0])/3;
}

int funktionRotationMitBall(int x){
  if( x > 800 ){
    return 30;
  }
  else if(800 >= x && x > 300){
    return map(x,300,1000,15,20);
  }
  else if(-300 > x && x >= -800){
    return map(x,-300,-1000,-15,-20);
  }
  else if( -800 > x){
    return -30;
  }
  return 0;
}

int funktionRotation(int x){
  if( x > 800 ){
    return 30;
  }
  else if(800 >= x && x > 150){
    return map(x,150,800,15,23);
  }
  else if(-150 > x && x >= -800){
    return map(x,-150,-800,-15,-23);
  }
  else if( -800 > x){
    return -30;
  }
  return 0;
}

void turnLow(int array[]){
  for(int i = 0; i <= sizeof(array); i++){
    digitalWrite(array[i],LOW);
  }
}

void turnHigh(int array[]){
  for(int i = 0; i <= sizeof(array); i++){
    digitalWrite(array[i],HIGH);
  }
}

void updateLeds(byte dirBall){
 switch(dirBall){
    turnLow(greenLeds);
    case LEFT    :
      digitalWrite(greenLeds[0],HIGH);
      break;
    case MIDDLE  :
      digitalWrite(greenLeds[1],HIGH);
      break;
    case RIGHT   :
      digitalWrite(greenLeds[2],HIGH);
      break;
    case NOWHERE :
      break;
  }
}
bool completlyFalse(bool a[],int len){
  for(int i=0;i<len;i++){
    if(a[i])
      return false;  
  }
  return true;
}

void addBool(bool b, bool a[], int len){
  for(int i=0; i<len; i++){
    a[len-i-1] = a[len-i-2];
  }
  a[0] = b;
}

void writeSegment(int x){
    segment.clear();
    segment.print(x);
    segment.writeDisplay();
}

void writeSegment(long x){
    segment.clear();
    segment.print(x);
    segment.writeDisplay();
}

void writeSegment(short x){
    segment.clear();
    segment.print(x);
    segment.writeDisplay();
}
