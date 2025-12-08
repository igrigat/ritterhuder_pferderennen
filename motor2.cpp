#include <AccelStepper.h>

// ---------------------- Pinbelegung ----------------------
const int sensor1Pin = 2;  // Loch 1
const int sensor2Pin = 3;  // Loch 2
const int sensor3Pin = 4;  // Loch 3
const int sensor4Pin = 5;  // Sensor Anfangsposition (Start)
const int sensor5Pin = 6;  // Sensor Endposition (Finish)
const int taster6Pin = 7;  // Start-/Reset-Taster

// Schrittmotor (28BYJ-48 an ULN2003)
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

// LED für Motoraktivität
const int ledPin = 13;

AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

// Schritte pro Umdrehung
const int stepsPerRevolution = 2048;

// ---------------------- Motor-Parameter ----------------------
const float MOTOR_SPEED  = 600.0;  // überall gleiche Drehzahl (Schritte/Sek.)
const float MOTOR_ACCEL  = 800.0;  // Beschleunigung für Queue-Fahrten

// ---------------------- Queue-Struktur ----------------------
struct Task { long steps; };
const int MAX_TASKS = 20;
Task queue[MAX_TASKS];
int queueHead = 0;
int queueTail = 0;
bool taskActive = false;
bool endPositionAktiv = false;
bool homingActive = false;     // automatisches Zurückfahren von Finish zu Start
Task currentTask;

// ---------------------- Entprellung ----------------------
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
unsigned long lastTrigger3 = 0;
const unsigned long debounceTime = 200;
unsigned long pause = 0;

// ---------------------- Lauf-Status ----------------------
bool manualRunActive = false;

// ---------------------- Queue-Funktionen ----------------------
void enqueue(long steps) {
  int nextTail = (queueTail + 1) % MAX_TASKS;
  if (nextTail != queueHead) {
    queue[queueTail].steps = steps;
    queueTail = nextTail;
  }
}
bool dequeue(Task &t) {
  if (queueHead == queueTail) return false;
  t = queue[queueHead];
  queueHead = (queueHead + 1) % MAX_TASKS;
  return true;
}

// ---------------------- Coil-Steuerung ----------------------
void disableCoils() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}

void enableCoils() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

// ---------------------- Setup ----------------------
void setup() {
  Serial.begin(9600);

  pinMode(sensor1Pin, INPUT_PULLUP);
  pinMode(sensor2Pin, INPUT_PULLUP);
  pinMode(sensor3Pin, INPUT_PULLUP);
  pinMode(sensor4Pin, INPUT_PULLUP);
  pinMode(sensor5Pin, INPUT_PULLUP);
  pinMode(taster6Pin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  stepper.setMaxSpeed(MOTOR_SPEED);   // überall gleiche Endgeschwindigkeit
  stepper.setAcceleration(MOTOR_ACCEL);

  disableCoils(); // Spulen beim Start aus
}

// ---------------------- Loop ----------------------
void loop() {
  unsigned long now = millis();

  // Endposition-Status (Sensor 5)
  endPositionAktiv = (digitalRead(sensor5Pin) == LOW);

  // -------------------------------------------------------------------
  // 1) Manuelle Steuerung: Taster -> Motor läuft bis Start-Sensor (4)
  // -------------------------------------------------------------------
  if (digitalRead(taster6Pin) == LOW && !manualRunActive && !homingActive && !taskActive) {
    manualRunActive = true;
    enableCoils();
    digitalWrite(ledPin, HIGH);
    stepper.setSpeed(+MOTOR_SPEED);     // Richtung: Start (sensor4)
  }

  if (manualRunActive) {
    if (digitalRead(sensor4Pin) == LOW) {
      // Startposition erreicht
      manualRunActive = false;
      disableCoils();
      digitalWrite(ledPin, LOW);
    } else {
      stepper.runSpeed();               // konstante Geschwindigkeit
      return;                           // andere Logik aussetzen, solange manuell
    }
  }

  // -------------------------------------------------------------------
  // 2) Automatisches Zurückfahren, wenn Finish (5) aktiv, aber nicht an Start (4)
  // -------------------------------------------------------------------
  if (!manualRunActive && !taskActive && !homingActive &&
      (digitalRead(sensor5Pin) == LOW) && (digitalRead(sensor4Pin) == HIGH)) {

    homingActive = true;
    enableCoils();
    digitalWrite(ledPin, HIGH);
    stepper.setSpeed(+MOTOR_SPEED);     // gleiche Geschwindigkeit Richtung Start

    // Queue leeren, damit nichts mehr nachläuft
    queueHead = queueTail;
  }

  if (homingActive) {
    if (digitalRead(sensor4Pin) == LOW) {
      // Startposition erreicht
      homingActive = false;
      disableCoils();
      digitalWrite(ledPin, LOW);
    } else {
      stepper.runSpeed();
      return;                           // während Homing nichts anderes tun
    }
  }

  // -------------------------------------------------------------------
  // 3) Neue Aufgaben nur, wenn Endposition NICHT aktiv und kein Homing
  // -------------------------------------------------------------------
  if (!endPositionAktiv && !homingActive && pause + 4000 <= now) {
    if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
      enqueue(-stepsPerRevolution);          // 1 Umdrehung Richtung Finish
      lastTrigger1 = now; pause = now;
    }
    if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
      enqueue(-2L * stepsPerRevolution);     // 2 Umdrehungen
      lastTrigger2 = now; pause = now;
    }
    if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
      enqueue(-3L * stepsPerRevolution);     // 3 Umdrehungen
      lastTrigger3 = now; pause = now;
    }
  }

  // Falls Endposition aktiv bleibt: Queue leeren, Motor aus
  if (endPositionAktiv && !homingActive) {
    queueHead = queueTail;
    taskActive = false;
    disableCoils();
    digitalWrite(ledPin, LOW);
  }

  // -------------------------------------------------------------------
  // 4) Motorsteuerung über Queue (Positionsmodus mit fester Endgeschwindigkeit)
  // -------------------------------------------------------------------
  if (!taskActive && dequeue(currentTask)) {
    enableCoils();
    stepper.move(currentTask.steps);   // neg. Steps = Richtung Finish
    taskActive = true;
    digitalWrite(ledPin, HIGH);
  }

  if (taskActive) {
    if (stepper.distanceToGo() != 0) {
      stepper.run();                   // fährt bis MOTOR_SPEED hoch, dann konstant
    } else {
      taskActive = false;
      disableCoils();
      digitalWrite(ledPin, LOW);
    }
  } else {
    // Kein Task, kein Homing, keine manuelle Fahrt -> sicherheitshalber Spulen aus
    disableCoils();
  }
}
