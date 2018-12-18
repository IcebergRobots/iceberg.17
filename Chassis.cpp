#include "Arduino.h"
#include "Chassis.h"

// Sinuswerte von 0..359°, Tabelle ist schneller als rechnen
const int sinus[360] = {0,175,349,523,698,872,1045,1219,1392,1564,1736,1908,2079,2250,2419,2588,2756,2924,3090,3256,3420,3584,3746,3907,4067,4226,4384,4540,4695,4848,5000,5150,5299,5446,5592,5736,5878,6018,6157,6293,6428,6561,6691,6820,6947,7071,7193,7314,7431,7547,7660,7771,7880,7986,8090,8192,8290,8387,8480,8572,8660,8746,8829,8910,8988,9063,9135,9205,9272,9336,9397,9455,9511,9563,9613,9659,9703,9744,9781,9816,9848,9877,9903,9925,9945,9962,9976,9986,9994,9998,10000,9998,9994,9986,9976,9962,9945,9925,9903,9877,9848,9816,9781,9744,9703,9659,9613,9563,9511,9455,9397,9336,9272,9205,9135,9063,8988,8910,8829,8746,8660,8572,8480,8387,8290,8192,8090,7986,7880,7771,7660,7547,7431,7314,7193,7071,6947,6820,6691,6561,6428,6293,6157,6018,5878,5736,5592,5446,5299,5150,5000,4848,4695,4540,4384,4226,4067,3907,3746,3584,3420,3256,3090,2924,2756,2588,2419,2250,2079,1908,1736,1564,1392,1219,1045,872,698,523,349,175,0,-175,-349,-523,-698,-872,-1045,-1219,-1392,-1564,-1736,-1908,-2079,-2250,-2419,-2588,-2756,-2924,-3090,-3256,-3420,-3584,-3746,-3907,-4067,-4226,-4384,-4540,-4695,-4848,-5000,-5150,-5299,-5446,-5592,-5736,-5878,-6018,-6157,-6293,-6428,-6561,-6691,-6820,-6947,-7071,-7193,-7314,-7431,-7547,-7660,-7771,-7880,-7986,-8090,-8192,-8290,-8387,-8480,-8572,-8660,-8746,-8829,-8910,-8988,-9063,-9135,-9205,-9272,-9336,-9397,-9455,-9511,-9563,-9613,-9659,-9703,-9744,-9781,-9816,-9848,-9877,-9903,-9925,-9945,-9962,-9976,-9986,-9994,-9998,-10000,-9998,-9994,-9986,-9976,-9962,-9945,-9925,-9903,-9877,-9848,-9816,-9781,-9744,-9703,-9659,-9613,-9563,-9511,-9455,-9397,-9336,-9272,-9205,-9135,-9063,-8988,-8910,-8829,-8746,-8660,-8572,-8480,-8387,-8290,-8192,-8090,-7986,-7880,-7771,-7660,-7547,-7431,-7314,-7193,-7071,-6947,-6820,-6691,-6561,-6428,-6293,-6157,-6018,-5878,-5736,-5592,-5446,-5299,-5150,-5000,-4848,-4695,-4540,-4384,-4226,-4067,-3907,-3746,-3584,-3420,-3256,-3090,-2924,-2756,-2588,-2419,-2250,-2079,-1908,-1736,-1564,-1392,-1219,-1045,-872,-698,-523,-349,-175};

/**************************************************************************************************
 * Konstruktor(anzahl, maxPWM)
 * Parameter:
 * - Anzahl der Motoren
 * - Maximaler Ansteuerwert des PWM-Signals
 **************************************************************************************************/
Chassis::Chassis(int anzahl, int maxPWMWert) {
  _anzahl = anzahl;
  _maxPWM = maxPWMWert;
  _mutex  = false;
}

/**************************************************************************************************
 * Konstruktor(anzahl, maxPWM)
 * Parameter:
 * - Anzahl der Motoren
 * - Standardwert fuer PWM: 255, entsprechend 8-Bit
 **************************************************************************************************/
Chassis::Chassis(int anzahl) {
  _anzahl = anzahl;
  _maxPWM = 255;
  _mutex  = false;
}

/**************************************************************************************************
 * setMotorGeometrie(id, winkel, drehsinn)
 * Parameter:
 * - id       : Nummer des Motors, beginnend bei 0
 * - winkel   : Winkel der Motorachse zur X-Achse im Linkssinn
 * - drehsinn : Antriebsrichtung des Motors im Vorwärtslauf (von oben)
                +1: positiv (Links herum)
                -1: negativ (im Uhrzeigersinn)
 **************************************************************************************************/
void Chassis::setMotorGeometrie(int id, int winkel) {
  _motoren[id][0] =  sinus[(winkel+540)%360];  // = -sin(x)
  _motoren[id][1] =  sinus[(winkel+450)%360];  // =  cos(x)
}

/**************************************************************************************************
 * setMotorPINS(fwd, bwd, pwm)
 * Parameter:
 * - fwd, bwd: Richtungspins (INput)
 *             (HIGH / LOW) bei Vorwaerts- und (LOW / HIGH) bei Rueckwaertslauf
 * - pwm     : PWM-Pin fuer Motoransteuerung (ENable)
 **************************************************************************************************/
void Chassis::setMotorPINS(int id, int fwd, int bwd, int pwm) {
  pinMode(fwd, OUTPUT);
  pinMode(bwd, OUTPUT);
  pinMode(pwm, OUTPUT);

  _motoren[id][2] = fwd;
  _motoren[id][3] = bwd;
  _motoren[id][4] = pwm;
}

/**************************************************************************************************
 * setMotor(id, winkel, drehsinn, fwd, bwd, pwm)
 *   siehe setMotorGeometrie und setMotorPINS
 **************************************************************************************************/
void Chassis::setMotor(int id, int winkel, int fwd, int bwd, int pwm) {
  setMotorGeometrie(id, winkel);
  setMotorPINS(id, fwd, bwd, pwm);
}

/**************************************************************************************************
 * steuerwerteBerechnen(richtung, tempo, rotation, *werte)
 *   Berechnung der Ansteuerwerte fuer Motoren
 *
 * richtung: Winkel fuer die Bewegung gegen die x-Richtung (rechts) im Linkssinn
 * tempo   : Geschwindigkeit in %
 * rotation: Drehgeschwindigkeit und -richtung in %, positive Werte: Linksdrehung
 * Achtung : tempo + |rotation| darf 100% nicht überschreiten
 * *werte  : int-Array, in dem die berechneten Steuerwerte abgelegt werden.
 *           Referenzparameter, da Arduino-C ein Array als Funktionswert nicht vorsieht
 **************************************************************************************************/
void Chassis::steuerwerteBerechnen(int richtung, int tempo, int rotation, int *werte) {
  long betrag = long (tempo) * _maxPWM/5;
  long ziel[2] = {betrag * sinus[(richtung+90)%360]/100000, betrag * sinus[(richtung)%360]/100000} ;  // verschobener Sinus = Cosinus
  long rotPWM = (rotation * _maxPWM)/100;
  _richtung = richtung;
  _rotation = rotation;

  for (int i=0; i<_anzahl; i++) {
    werte[i] = (ziel[0]*_motoren[i][0] + ziel[1]*_motoren[i][1])/20000 + rotPWM;
  } 
}

void Chassis::sB(int richtung, int tempo, int rotation, int *werte) {
  steuerwerteBerechnen(richtung, tempo, rotation, werte);
}

/**************************************************************************************************
 * motorAnsteuern(id, wert)
 * Parameter:
 * - id   : Nummer des Motors
 * - wert : PWM-Wert, Vorzeichen entscheidet ueber Drehrichtung
 **************************************************************************************************/
void Chassis::motorAnsteuern(int id, int wert) {
     if (wert > 0) {
    digitalWrite(_motoren[id][2], HIGH);
    digitalWrite(_motoren[id][3], LOW);
    analogWrite (_motoren[id][4], wert);
  } else {
    digitalWrite(_motoren[id][2], LOW);
    digitalWrite(_motoren[id][3], HIGH);
    analogWrite (_motoren[id][4], -wert);
  }
}

/**************************************************************************************************
 * motorenAnsteuern(steuerwerte[])
 *   steuert alle Motoren mit den Werten aus dem Steuerwertarray an.
 **************************************************************************************************/
void Chassis::motorenAnsteuern(int steuerwerte[]) {
  if(!_mutex){ 
    return;
  }
  for (int i=0; i<_anzahl; i++) {
    this -> motorAnsteuern(i, steuerwerte[i]);
  }
}

void Chassis::mA(int steuerwerte[]){
  if(!_mutex){ 
    return;
  }
  motorenAnsteuern(steuerwerte);
}

/**************************************************************************************************
 * motorenHalt()
 *   stoppt alle Motoren
 **************************************************************************************************/
void Chassis::motorenHalt(bool brake) {
  if(!_mutex){
    for (int i=0; i<_anzahl; i++) {
      this -> motorAnsteuern(i, 0);
  
      if (brake) {
        digitalWrite(_motoren[i][2], LOW);
        digitalWrite(_motoren[i][3], LOW);
        analogWrite (_motoren[i][4], 255);
      }
    }
  }
}

/**************************************************************************************************
 * setMutex()
 *   Mutex bestimmt ob die Motoren angesteuert werden können
 **************************************************************************************************/
void Chassis::setMutex(bool mutex){
  for (int i=0; i<_anzahl; i++) {
    this -> motorAnsteuern(i, 0);
  }
  this -> motorenHalt(true);
  _mutex = mutex;
}

void Chassis::setRichtung(int richtung){
  _richtung = richtung;  
}
