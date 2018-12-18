
//-- EEPROM
#include <EEPROM.h>

/*  Speicherbelegung
 *  ByteNr   ->  Variabel
 *  0 -> usWerte[0]
 *  1 -> usWerte[1]
 *  2 -> usWerte[2]
 *  3 -> usWerte[3]
 *  4 -> kompassStart
 */

//-- Utils
#include "config.h"
#include "util.h"
#include "sensorUtil.h"
#include "Chassis.h"
#include "lustigeAnimationen.h"

//-- Debugging
//#include "LiquidCrystal.h"
//#include "Adafruit_LEDBackpack.h"
//#include "Adafruit_GFX.h"

//-- I²C
#include <Wire.h>
//-- Pixy
#include <SPI.h>  
#include <Pixy.h>

#define SPEED          100  //std 100
#define ROTATIONSPEED  25  //std 32
#define GEGENDREHZEIT  10

#define PixyBallNummer  1
#define PixyTorNummer   2

#define pixyAngeschlossen true
#define linienAngeschlossen true



// Schussmetohde, heißt boom da schuss bereits als name einer variable existiert
void BOOM(){
  digitalWrite(schuss,HIGH);
  delay(28);
  digitalWrite(schuss,LOW);
}

// Interrupt Metohde - Wenn Liniennano ein singal sendet, wird diese ausgeführt
void line(){
  motors.motorenHalt(true);
  lineDect = true;
  lineTimer = millis();
  detachInterrupt(digitalPinToInterrupt(bodenInterrupt));
}

void readPixy(){
  
    deltaX      = 0;
    deltaY      = 0;
    torMaxX     = 0;
    torMinX     = 0;
    greatestBlock = 0;
    highX = 0;
    highY = 0;
    blockAnzahl = 0;

    blocks = pixy.getBlocks();
    
    for (int j=0; j<blocks; j++){

      //checken wo der ball liegt
      if(pixy.blocks[j].signature == PixyBallNummer){
        deltaX += pixy.blocks[j].x;   
        deltaY += pixy.blocks[j].y; 
        if(pixy.blocks[j].height*pixy.blocks[j].width > greatestBlock){
          greatestBlock = pixy.blocks[j].height*pixy.blocks[j].width;
          highX = pixy.blocks[j].x;
          highY = pixy.blocks[j].y; 
        }
        //pixy.blocks[j].print();
        blockAnzahl++;   
      }

      //checken ob das tor frei ist
      if(pixy.blocks[j].signature == PixyTorNummer){
          torMaxX = pixy.blocks[j].x + pixy.blocks[j].width/2;
          torMinX = pixy.blocks[j].x - pixy.blocks[j].width/2;
          if(129 > torMinX && torMaxX > 189){
            schussEnabled=true;
          }
      }
      
    } 

    if(blockAnzahl > 0){
      deltaX = deltaX/blockAnzahl;
      deltaY = deltaY/blockAnzahl;
    }
    
    if(deltaX > 0){   // wenn ball gesehen wird :
      fall = BALLSICHT;
      //digitalWrite(greenLeds[1],HIGH);
      addBool (true,lastFrameBallDetected,sizeof(lastFrameBallDetected));
    }   
    else{         // wenn ball nicht gesehen wird :
      fall = BALLUNSICHTBAR;
      //digitalWrite(greenLeds[2],HIGH);
      addBool (false,lastFrameBallDetected,sizeof(lastFrameBallDetected));
    }   
    //turnLow(greenLeds);
    pixyTimer = millis();
}

void setup() {  

  //deklarieren der pinModes sowie instanziierefn der motorobjekte
  configMotors();
  pinModes();

  
  Wire.begin();
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  
  //matrix.begin(0x70);
  //segment.begin(0x71); 
   
 
  pixy.init();

  // Interrupt Metode wird deklariert
  // wenn der Liniennano ein interrtupt signal aussendet wird line() ausgeführt
  if(linienAngeschlossen){
    attachInterrupt(digitalPinToInterrupt(bodenInterrupt), line, RISING);
  }

  // standartwert von lichtschrankenwert, ball direction und Fall, kompass Nullpunkt, schuss aktivierbarkeit, motor aktivierbarkeit, lesen der Us standartwerte(aus eeprom)
  lsStd         = analogRead(lichtschranke);
  dirBall       = NOWHERE;
  schussEnabled = true;
  fall          = BALLUNSICHTBAR;
  motors.setMutex(true);

  loadUsEeprom();
  loadCompassEeprom();    

  startRambo = digitalRead(topSchalter1);

  while(pixyDebug){
    readPixy();
    Serial.print(" x : ");
    Serial.println(deltaX);
    Serial.print(" y : ");
    Serial.println(deltaY);
    Serial.println();
    delay(200);   
    
  }

  
  if(kompassDebug){
    motorWerte[0] = 40;
    motorWerte[1] = 40;
    motorWerte[2] = 40;
    motors.mA(motorWerte);
  }
  
  abweichungAdd(abweichung(readCompass(),kompassStart));
  delay(100);  
  abweichungAdd(abweichung(readCompass(),kompassStart));  
  delay(100);
  abweichungAdd(abweichung(readCompass(),kompassStart));

  while(kompassDebug){  
    Serial.println(abweichungAdd(abweichung(readCompass(),kompassStart)));
    delay(50);
  }

  if(motorDebug){
    motorWerte[0] =  200;
    motorWerte[1] =    0;
    motorWerte[2] = -200;
    motors.mA(motorWerte);
    while(1){
      ;
    }  
  }
  Serial.println("Setup erfolgreich");
}


void loop() {
  // Bluethoot Anmachen ?
  if(!digitalRead(topSchalter4) && !bluethootPatched){
    Serial1.begin(9600);
    blink();    
    bluethootPatched = true;
  }
  
  if(digitalRead(topSchalter4) && bluethootPatched){
    Serial1.println("Disconnect");
    Serial1.end();
    bluethootPatched = false;
  }
  
  if(millis() - bluethoothTimer > 200 && Serial1){
    Serial1.println(readCompass());
    while(Serial1.available() > 0){
      char in = Serial1.read();
      if(in == '0'){
        //turnLow(greenLeds);
        }else
      if(in == '1'){
        //turnHigh(greenLeds);
      }
    }
    if(! digitalRead(schalter1) && ! digitalRead(topSchalter5))
      Serial1.write('1'); 
    else
      Serial1.write('0'); 
    bluethoothTimer = millis();
  }
    
  // Checken ob motoren an sein sollen
  // -> Schalter 1 aus Platine und Schalter rechts unten auf TopPlatte
  if(! digitalRead(schalter1) && ! digitalRead(topSchalter5)){   
    if(motorsStopped){  
      motors.setMutex(true);
    }  
    motorsStopped = false;
  }
  
  // Kalibrieren
  else if(! digitalRead(kalibrierenKnopf) && ! digitalRead(schalter2)){
    //Serial1.println("!KalibriereN!");
    turnLow(greenLeds);    
    
    delay(400); 
    //Serial1.print("Kompass :  ");
    
    while( digitalRead(kalibrierenKnopf) ){
      turnHigh(greenLeds);  delay(140);
      turnLow(greenLeds);   delay(80); 
    }
    
    kompassStart = readCompass();
    
    /*
    segment.clear();
    segment.print(kompassStart);
    segment.writeDisplay();
    */
    
    //Serial1.println(kompassStart);
    
    delay(500);
    //Serial1.print("Ultraschall - Hinten : ");
    
    while( digitalRead(kalibrierenKnopf) ){
      turnHigh(greenLeds);  delay(140);
      turnLow(greenLeds);   delay(80); 
    }
    
    readUs(false,false,false,true); //hinten 

    /*
    segment.clear();
    segment.print(usWerte[3]);
    segment.writeDisplay();
    */
      
    //Serial1.println(usWerte[3]);
    
    delay(500);
    //Serial1.print("Vorn :");
    
    while( digitalRead(kalibrierenKnopf) ){
      turnHigh(greenLeds);  delay(140);
      turnLow(greenLeds);   delay(80); 
    }
    
    readUs(false,true,false,false); //vorn
    
    /*
    segment.clear();
    segment.print(usWerte[1]);
    segment.writeDisplay();  
    */
    
    //Serial1.println(usWerte[1]);
    
    delay(500);
    //Serial1.print("Links :"); 
   
    while( digitalRead(kalibrierenKnopf) ){
      turnHigh(greenLeds);  delay(140);
      turnLow(greenLeds);   delay(80); 
    }
    
    readUs(true,false,false,false); 
    
    /*
    segment.clear();
    segment.print(usWerte[0]);
    segment.writeDisplay();
    */
    
    //Serial1.println(usWerte[0]);
      
    delay(500);
    //Serial1.print("Rechts :");   
     
    while( digitalRead(kalibrierenKnopf) ){
      turnHigh(greenLeds);  delay(140);
      turnLow(greenLeds);   delay(80); 
    }
    
    readUs(false,false,true,false); 
    
    /*
    segment.clear();
    segment.print(usWerte[2]);
    segment.writeDisplay();
    */
    
    //Serial1.println(usWerte[2]);
      
    writeCompassEeprom();
    writeUsEeprom();
    loadCompassEeprom();
    loadUsEeprom();    
  }
  
  //Stoppen
  else{   
    if(!motorsStopped){
      motors.motorenHalt(false);
      motors.setMutex(false);
      motorsStopped = true;
    }
    //while(digitalRead(schalter1) || digitalRead(topSchalter5)) 
      ;
    
    startRambo = digitalRead(topSchalter1);
  }

  //Lesen der US  
  //-->wenn der Timer aktiv ist( alle 200 ms)
  // doch nicht |--> schaltet rote Leds An
  //  |--> liest US aus
  // doch nicht  |--> schaltet rote Leds Aus
  //  |--> setzt Timer neu
  //
  // keine roten leds wegen SPI korrelation
  
  if(millis() - usTimer > 250){
    
    readUs();

    if(usDebug){
      
      if(usDebugPort == 0){
        for(int i=0; i<4;i++){
          Serial.print(i);  
          Serial.print(" --> ");  
          Serial.println(usWerte[i]);  
        }
      }

      if(usDebugPort == 1){
        for(int i=0; i<4;i++){
          Serial1.print(i);  
          Serial1.print(" --> ");  
          Serial1.println(usWerte[i]);  
        }
      }
      
    }
    
    usTimer = millis();
        
  }

  //bilden eines Durschnittswertes der X koordinaten der Blöcke: 
  schussEnabled = false;

  if(pixyAngeschlossen && millis() - pixyTimer > 60){
    readPixy();
  } 

  // falls lichtschranke aktiviert ist, wird der ballfall aktiviert
  if(analogRead(lichtschranke) > lsStd + lsTol && millis() - lsTimer > 20 ){
    fall = BALLBESITZ;
    //digitalWrite(greenLeds[0],HIGH);
    lsTimer = millis();
  }
  //if(Serial1)Serial1.println(lsStd + lsTol -16- analogRead(lichtschranke) );
    
  //berechnen des gegensteuerwertes
  diffComp = abweichungAdd(abweichung(readCompass(),kompassStart));
    
  //segment.clear();
  //segment.print(diffComp);
  //segment.writeDisplay();
  
  int gegenRotation =
  //map(abweichung(readCompass(),kompassStart), -1800, 1800, -30, 30);
  funktionRotation(diffComp);

  int gegenRotationMitBall = 
  funktionRotationMitBall(diffComp);

  int ramboDelay = 0;

  switch(fall){
    case BALLBESITZ:
      //checken ob schuss betätigt werden darf
      //motors.mA(vor);
      if(millis() - schussTimer  > 900
      //&& schussEnabled
      ){
        schussTimer = millis();
        BOOM();
        /*for(byte i=0;i<3;i++){
          motorWerte[i] = vor[i];
          motors.setRichtung(90);
        }*/
      }
      //delay(100);

    case BALLSICHT:
      // turnLow(greenLeds);
      // steuerwerte angegeben, in abhängigkeit der y werte des balles
      
      //if(millis() - segmentTimer > 100){ 
      //  segmentTimer = millis();
      //  writeSegment(deltaY);
      //  delay(50);
      //}
      // Nah
      /*
      if(false && deltaY <= 50){
      digitalWrite(greenLeds[0],HIGH);
        if(deltaX < 120){
          for(byte i=0;i<3;i++){
            motorWerte[i] = hrechts[i];
            motors.setRichtung(45);
          }
        }else if(deltaX <= 200 && deltaX >= 120){
          for(byte i=0;i<3;i++){
            motorWerte[i] = vor[i];
            motors.setRichtung(90);
          }
        }else if(deltaX > 200){
          for(byte i=0;i<3;i++){
            motorWerte[i] = hlinks[i];
            motors.setRichtung(135);
          }
        }
      }

      // Fern
      else if(true || deltaY > 50 ){
      digitalWrite(greenLeds[0],LOW);
        if(deltaX < 120){
          for(byte i=0;i<3;i++){
            motorWerte[i] = rechts[i];
            motors.setRichtung(0);
          }
        }else if(deltaX <= 200 && deltaX >= 120){
          for(byte i=0;i<3;i++){
            motorWerte[i] = vor[i];
            motors.setRichtung(90);
          }
        }else if(deltaX > 200){
          for(byte i=0;i<3;i++){
            motorWerte[i] = links[i];            
            motors.setRichtung(180);
          }
        }
      }
      */
      if(highX >= 140 && highX <= 200){
        for(byte i=0;i<3;i++){
          motorWerte[i] = vor[i];
        }
        motors.setRichtung(90);
      }
      // links
      if(highX > 200 && highY <= 120){
        for(byte i=0;i<3;i++){
          motorWerte[i] = hlinks[i];
        }
      }
      if(highX > 200 && highY > 120){
          for(byte i=0;i<3;i++){
            motorWerte[i] = links[i];
            motors.setRichtung(180);
          }
      }
      if(highX > 285 && highY < 50){
          for(byte i=0;i<3;i++){
            motorWerte[i] = links[i];
            motors.setRichtung(180);
          }
      }
      
      // rechts
      if(highX < 140 && highY <= 120){
        for(byte i=0;i<3;i++){
          motorWerte[i] = hrechts[i];
        }
        motors.setRichtung(0);
      }
          for(byte i=0;i<3;i++){
            motorWerte[i] = rechts[i];
            motors.setRichtung(0);
          }
      
      if(highX < 25 && highY < 50){
          for(byte i=0;i<3;i++){
            motorWerte[i] = rechts[i];
            motors.setRichtung(90);
          } 
      }

      //Kompassnachbesserung
     /* if(diffComp > 500){
        for(byte i=0;i<3;i++){
          motorWerte[i] += 10;
        }  
      }
      */
      if(diffComp > 250){
        for(byte i=0;i<3;i++){
          motorWerte[i] += 10;
          
          if(motorWerte[i] > 255){
            motorWerte[i] = 255;
          }
        }  
        digitalWrite(redLeds[0],HIGH);
      }
      else{
        digitalWrite(redLeds[0],LOW);        
      }
      /*
      if(diffComp < -500){
        for(byte i=0;i<3;i++){
          motorWerte[i] -= 10;
        }    
      }*/
      
      if(diffComp < -250){
        for(byte i=0;i<3;i++){
          motorWerte[i] -= 10;

          if(motorWerte[i] < -255){
            motorWerte[i] = -255;
          }
        }  
        digitalWrite(redLeds[2],HIGH);
      }
      else{
        digitalWrite(redLeds[2],LOW);        
      }
      
    break;

    case BALLUNSICHTBAR:
        diffComp = abweichung(readCompass(),kompassStart);
        
        if(diffComp>250){
          for(byte i=0;i<3;i++){
            motorWerte[i] = turn_l[i];
          }
        }
        
        else if(diffComp<-250){          
          for(byte i=0;i<3;i++){
            motorWerte[i] = turn_r[i];
          }
        }
        
        else{          
          for(byte i=0;i<3;i++){
            motorWerte[i] = rueck[i];
            motors.setRichtung(270);
          }          
        }
        
    break;
  }

  if(millis() - ansteuerTimer > 40
    && ! motorsStopped
    && ! lineDect){
    //if(Serial1)Serial1.println("ansteuern");
    if(startRambo){
      motors.mA(rambo);
      if(!digitalRead(topSchalter2)){
        ramboDelay = 5000;
      }else{
        ramboDelay = 3000; 
      }
      for (int i=0; i<ramboDelay/300; i++){
        if(analogRead(lichtschranke) > lsStd + lsTol){
          BOOM();                                                                           
        }
        delay(300);
      }
      startRambo = false;
    }
    
    motors.mA(motorWerte);  
    ansteuerTimer = millis();
  }

  // 1. Fahrrichtung und Kompassabweichung speichern 
  // 2. drehen bis Kompassabweichung < 30°
  // 3. stoppen und Ultraschall lesen
  // 4. Fallunterscheidung anhand der Sensorwerte
  
  if(lineDect){

    byte lineSensorNr;
    
    /*
    while(Serial2.read() >= 1){
      lineSensorNr = Serial2.read();
    }
    */
    
    diffComp = abweichung(readCompass(),kompassStart);

    int oldDiff = diffComp;
    int oldDirection = motors._richtung;    

    if(lineDebug){
      if(lineDebugPort == 0){
        Serial.println("Linie gefunden -> ausrichten");
        Serial.print("Alte Fahrrichtung : ");
        Serial.println(oldDirection);
        Serial.print("Alte Drehung : ");
        Serial.print(oldDiff/10);
        Serial.println("°");
      }
    }
    while(abs(diffComp) > 300){    
      if(diffComp>300){  
        motors.mA(turn_l);
      }        
      else if(diffComp < -300){   
        motors.mA(turn_r);
      }
      diffComp = abweichung(readCompass(),kompassStart);
      if(lineDebug){
        Serial.print("Drehung : ");  
        Serial.println(diffComp);
      }
    }

    motors.motorenHalt(true);
    readUs();
    

    if(lineDebug){
      if(lineDebugPort == 0){
        Serial.println("Ausrichten abgeschlossen");
        Serial.println("Us Werte");
        
        Serial.print("rechts : ");
        Serial.println(usWerte[0]);
        Serial.print("vorn : ");
        Serial.println(usWerte[1]);
        Serial.print("links : ");
        Serial.println(usWerte[2]);
        Serial.print("hinten : ");
        Serial.println(usWerte[3]);

        Serial.print("Drehung : ");
        Serial.println(diffComp);
      }
    }
    
    short lineCase;
    bool usDefect[4];
    bool done = false;
    unsigned long outOfTime = millis();
    while(!done && millis() - outOfTime < 2000){
      
      if(lineDebug){
        if(lineDebugPort == 0){
          Serial.println();
          Serial.println();
          Serial.print("rechts : ");
          Serial.println(usWerte[0]);
          Serial.print("vorn : ");
          Serial.println(usWerte[1]);
          Serial.print("links : ");
          Serial.println(usWerte[2]);
          Serial.print("hinten : ");
          Serial.println(usWerte[3]);        
        }
      }
      
      for(int i=0;i<4;i++){
        if(usWerte[i] == 254 || usWerte[i] == 0){
          usDefect[i] = true;
        }
        else{
          usDefect[i] = false;  
        }
      }
      
      if(usWerte[0] < 40 && (usWerte[2] > 50 || usDefect[2])){
        //if(Serial1)Serial1.println("rechts");   
        for(byte i=0;i<3;i++){
          motorWerte[i] = links[i];
        }
        lineCase = 0;
        done = true;
      }
      else if(usWerte[2] < 40 && (usWerte[0] > 50 || usDefect[0])){
        //if(Serial1)Serial1.println("links"); 
        for(byte i=0;i<3;i++){
          motorWerte[i] = rechts[i];
        }
        lineCase = 1;
        done = true;
      }
      else if(usWerte[3] < 40){
        //if(Serial1)Serial1.println("hinten");  
        for(byte i=0;i<3;i++){
          motorWerte[i] = vor[i];
        }
        lineCase = 2;
        done = true;
      }
      else if(usWerte[1] < 40){
        //if(Serial1)Serial1.println("vorn");  
        for(byte i=0;i<3;i++){
          motorWerte[i] = rueck[i];
        }
        lineCase = 3;
        done = true;
      }
      /*else{
        //if(Serial1)Serial1.println("dont know");  
        motors.sB((motors._richtung+180)%180,85,0,motorWerte);  // TODO Richtungssinn der Kompassabweichung überprüfen 
        lineCase = 4;
        done = true;
      }*/
      readUs();
      if(!done){
        delay(250);  
      }
    }
    if(lineDebug){
      if(lineDebugPort == 0){
        Serial.print("Case : ");
        Serial.println( lineCase );
        Serial.println();
        Serial.println();
        delay(1000);
      }
    }


    motors.mA(motorWerte);
    delay(300);
    motors.motorenHalt(true);
    lineDect = false;  

    attachInterrupt(digitalPinToInterrupt(bodenInterrupt), line, RISING);
    
  }

  
}
