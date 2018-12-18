#include "Arduino.h"
#include "Chassis.h"
#include "Pixy.h"

//-- Debugging
#include "LiquidCrystal.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#include "pitches.h"

#ifndef config_h    // verhindert mehrfaches include
#define config_h 

//--------------------- Debugs
#define usDebug false
#define usDebugPort 0 
//0 -> Serial
//1 -> Serial1

#define lineDebug false
#define lineDebugPort 0 

#define kompassDebug false

#define pixyDebug false

#define motorDebug false

//--------------------- Pins
//-- Led Pins
#define greenLed1 43
#define greenLed2 45
#define greenLed3 47

#define redLed1 49
#define redLed2 51
#define redLed3 53

//-- Display Pins
#define aa 9
#define bb 8
#define cc 7
#define dd 6
#define ee 5
#define ff 4

//-- Schalter Pins
//-- Platine
#define kalibrierenKnopf 41
#define schalter1 39
#define schalter2 35
#define schalter3 37
//-- Top
#define topSchalter1 25
#define topSchalter2 27
#define topSchalter3 29
#define topSchalter4 31
#define topSchalter5 33

//-- Potis
#define potiTop A1
#define potiBoard A2

//-- Buzzer
#define buzzer 52
#define lautsprecher 2

//-- lichtschranke
#define lichtschranke A0

//-- Lipo Spannung
#define liPo A3

//-- Schuss
#define schuss 48

//-- Dribbler
#define dribbler 50

//--------------------- Motors
//-- Winkel
#define angel1 26
#define angel2 146
#define angel3 270

//-- Pins
#define fwd1 34
#define fwd2 30
#define fwd3 38

#define bwd1 36
#define bwd2 32
#define bwd3 40

#define pwm1 11
#define pwm2 10
#define pwm3 12


//--------------------- Ports
//-- Interrupt Ports
#define irInterrupt 22
#define usInterrupt 24
#define bodenInterrupt 3


//--------------------- Konstanten
//-- Pixy
#define drittelBild 53

#define NOWHERE 0
#define LEFT    1
#define MIDDLE  2
#define RIGHT   3

//-- Lichtschranke
#define lsTol   19//Lichtschranke Toleranz

//-- Fall
#define BALLBESITZ  0
#define BALLSICHT   1
#define BALLUNSICHTBAR 2

void pinModes();
void configMotors();
void configInterrupt();

//--------------------- Variablen
//-- Leds
int greenLeds[] = {greenLed1,greenLed2,greenLed3};                      //Led´s in einem Array verwaltet -> alle ansteuern via for schleife
int redLeds[] = {redLed1,redLed2,redLed3};
int schalter[]  = {schalter1,schalter2,schalter3};                      //Schalter in einem Array verwaltet -> alle ansteuern via for schleife
int topSchalter[] = {topSchalter1,topSchalter2,topSchalter3,topSchalter4,topSchalter5};

//-- Start straight forward
bool startRambo;

//-- Matrix kreiselemente
int matrixAbweichungY[] = {0,0,1,2,3,4,5,6,6,6,5,4,3,2,1,0};
int matrixAbweichungX[] = {3,4,5,6,6,6,5,4,3,2,1,0,0,0,1,2};

//-- Matrix history : so lässt sich ein Graph darstellen
int matrixHistory[] = {3,3,3,3,3,3,3,3};

//-- Motorvariabeln : motoren
boolean motorOn[] = {false,false,false};
boolean mutex;
int motorWerte[4];
unsigned long ansteuerTimer;

//-- Schussvariabeln
bool schussEnabled;

//-- Motorobjekt
Chassis motors(3, 255);                                           // Objekt von der Klasse Chassis (für Motoren) 
bool motorsStopped = false;

//-- Motorsteuerwerte
int rueck[3];
int rueckFast[3];
int vor[3];
int turn_r[3];
int turn_l[3];
int rechts[3];
int hrechts[3];  //halb Rechts
int links[3];  
int hlinks[3];   //halb Links
int rambo[3];

//-- Matrixobjekt
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

//-- Segmentobjekt
Adafruit_7segment segment = Adafruit_7segment();

//-- Kompass
int kompassStart;
int abweichungHistory[3];

//-- Lichtschranke
int lsStd;    // Lichtschranke standart
unsigned long lsTimer = 0;

unsigned long  schussTimer = 0;

//-- Ultraschallwerte
byte usWerte[4];
byte usStd[4];
unsigned long usTimer = 0;

//-- Liniensensoren
bool stopped = false;

//-- Ballfall
int fall;

//-- Enum zum eintragen des Ballsektors
byte dirBall;

//-- Kompass
int diffComp;

//-- linientimer
unsigned long lineTimer = 0;
boolean lineDect = false;
int lineWerte[4];

//-- Pixy
Pixy pixy;
uint16_t blocks;
unsigned long pixyTimer = 0;
bool lastFrameBallDetected[10];
int deltaX = 0,
    deltaY = 0,
    highX = 0,
    highY = 0,
    greatestBlock = 0,
    torMaxX = 0,
    torMinX = 0;
byte blockAnzahl = 0;

//-- Bluethooth 
bool bluethootPatched = false;
bool bluethoothStopped = false;
unsigned long bluethoothTimer = 0;

//-- Segment
unsigned long segmentTimer = 0;
#endif
