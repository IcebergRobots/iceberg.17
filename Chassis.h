/* 
 * Chassis.h
 * 
 * sammelt motorbezogene Funktionen fuer Soccerbots.
 * Der Konstruktor benoetigt die Motoranzahl und 
 * den maximalen Ansteuerwert fuer die Motoren.
 * Die Geometrie und die Anschluesse der Motoren werden 
 * ueber set-Methoden vereinbart.
 *
 * Autor R. Kreutel (2012)
 */

#ifndef Chassis_h    // verhindert mehrfaches include
#define Chassis_h

#include "Arduino.h"

class Chassis 
{
  public: 
    Chassis(int anzahl, int maxPWM);
    Chassis(int anzahl);
    
    void setMotorGeometrie(int id, int winkel);
    void setMotorPINS(int id, int fwd, int bwd, int pwm);
    void setMotor(int id, int winkel, int fwd, int bwd, int pwm);
    void steuerwerteBerechnen(int richtung, int tempo, int rotation, int *werte);
    void sB(int richtung, int tempo, int rotation, int *werte);
    void mA(int steuerwerte[]);
    void mA(int steuerwerte[], boolean mutex);
    void motorAnsteuern(int id, int pwm);
    void motorenAnsteuern(int steuerwerte[]);
    void motorenAnsteuern(int steuerwerte[], boolean mutex);
    void motorenHalt(boolean brake);
    void schuss();
    void schuss(int delay);
    void setMutex(bool mutex);
    void setRichtung(int richtung);
    

    int _richtung;
    int _rotation;

    
  private:
    int _anzahl;        // der Motoren
    int _maxPWM;        // fuer die Motoren
    int _motoren[4][5]; // x- und y-Richtung des Rades, Drehsinn, fwd-, bwd- und pwm-PIN 
    bool _mutex;
};
#endif
