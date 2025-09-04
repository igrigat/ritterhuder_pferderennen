#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <AccelStepper.h>

// MCP23017 initialisieren
Adafruit_MCP23017 mcp;

// Motoren über ULN2003 (4 Pins je Motor)
AccelStepper motor1(AccelStepper::HALF4WIRE, 2, 3, 4, 5);
AccelStepper motor2(AccelStepper::HALF4WIRE, 6, 7, 8, 9);
AccelStepper motor3(AccelStepper::HALF4WIRE, 10, 11, 12, 13);
AccelStepper motor4(AccelStepper::HALF4WIRE, A0, A1, A2, A3);

// Sensor-Mapping: 3 Sensoren pro Motor
int motorSensors[4][3] = {
  {0, 1, 2},    // Motor 1 → GPA0–GPA2
  {3, 4, 5},    // Motor 2 → GPA3–GPA5
  {8, 9, 10},   // Motor 3 → GPB0–GPB2
  {11, 12, 13}  // Motor 4 → GPB3–GPB5
};

//Zeitsteuerungsvariable für Motorteuerung
unsigned long time_m1

void setup() {
  Serial.begin(9600);

  // MCP starten (Adresse 0x20)
  mcp.begin();

 /* Original
 // 12 Sensoren als Input mit Pullup  ***pinMode(<1-12>, INPUT_PULLUP)*** was ist mcp?
  for (int i = 0; i < 12; i++) {
    mcp.pinMode(i, INPUT);
    mcp.pullUp(i, HIGH);
  } */

  // Kugeltaster
  const int taster = 1;
  //Millisekunden Motorlaufzeit je Tasterdruck
  const int time_per_taster=1000;
  //Taster Pin
  pinMode(taster, INPUT_PULLUP);
  //Variable für Zustandswechsel
  int taster_state = 0;

  // Max Speed für Motoren
  motor1.setMaxSpeed(1000);
  motor2.setMaxSpeed(1000);
  motor3.setMaxSpeed(1000);
  motor4.setMaxSpeed(1000);
}

void loop() {
/*  Original
// Alle Motoren durchgehen
  for (int m = 0; m < 4; m++) {
    for (int s = 0; s < 3; s++) {
      int sensorPin = motorSensors[m][s];
      if (mcp.digitalRead(sensorPin) == LOW) { // Sensor aktiv
        int steps = s + 1; // 1, 2 oder 3 Schritte
        AccelStepper* motor;

        switch (m) {
          case 0: motor = &motor1; break;
          case 1: motor = &motor2; break;
          case 2: motor = &motor3; break;
          case 3: motor = &motor4; break;
        }

        motor->move(steps);
      }
    }
  }

  // Motoren laufen lassen
  motor1.run();
  motor2.run();
  motor3.run();
  motor4.run();
  */

  //Mit Zeitsteuerung
//Wenn Taster gedrueckt wird, wird die Motorlaufzeit erhoeht
if(digitalRead(taster) == 1 && taster_state=0){
  if(time_m1 <= millis()){
    time_m1=millis()+time_per_taster;
  }
  else{
    time_m1=time_m1+time_per_taster;
  }
  taster_state=1; //damit nur einmal pro tasterdruck Zeit erhoeht wird
}
//Taster wieder frei, reset taster_state
if(digitalRead(taster) == 0){
  taster_state=0;
}
//Motor vorwärts laufen lassen
if(time_m1 >= millis()){
  motor1.run()
}

