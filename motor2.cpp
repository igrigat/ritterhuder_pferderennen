#include <AccelStepper.h>

// ---------------------- Pinbelegung ----------------------
const int sensor1Pin = 2;  // Loch 1
const int sensor2Pin = 3;  // Loch 2
const int sensor3Pin = 4;  // Loch 3
const int sensor4Pin = 5; // Sensor Anfangsposition
const int sensor5Pin = 6; // Sensor Endposition
const int taster6Pin = 7; // Start-Taster

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

// ---------------------- Queue-Struktur ----------------------
struct Task { long steps; };
const int MAX_TASKS = 20;
Task queue[MAX_TASKS];
int queueHead = 0;
int queueTail = 0;
bool taskActive = false;
bool endPositionAktiv = false;
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
// --> Neu hinzugefügt
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
  pinMode(sensor1Pin, INPUT_PULLUP);
  pinMode(sensor2Pin, INPUT_PULLUP);
  pinMode(sensor3Pin, INPUT_PULLUP);
  pinMode(sensor4Pin, INPUT_PULLUP);
  pinMode(sensor5Pin, INPUT_PULLUP);
  pinMode(taster6Pin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  stepper.setMaxSpeed(800);
  stepper.setAcceleration(100);
  disableCoils(); // Spulen beim Start aus
}

// ---------------------- Loop ----------------------
void loop() {
  Serial.begin(9600);
  unsigned long now = millis();

  // --- Manuelle Steuerung über Taster ---
  if (digitalRead(taster6Pin) == LOW && !manualRunActive) {
    manualRunActive = true;
    enableCoils(); // Spulen aktivieren
    digitalWrite(ledPin, HIGH);
    stepper.setSpeed(400);
  }

  if (manualRunActive) {
    if (digitalRead(sensor4Pin) == LOW) {
      manualRunActive = false;
      stepper.stop();
      disableCoils(); // Spulen deaktivieren
      digitalWrite(ledPin, LOW);
    } else {
      stepper.runSpeed();
      return;
    }
  }

  // --- Endschalter-Schutz FINISH---
  if (digitalRead(sensor5Pin) == LOW){
    while(digitalRead(sensor4Pin) != LOW){
      stepper.setSpeed(500);
      stepper.run();
      /////hier fehlt noch QUeue leeren ---------------------------------
    }
  }

  // --- Neue Aufgaben nur erlauben, wenn Endschalter nicht aktiv ---
  if (!endPositionAktiv && pause + 4000 <= now) {
    if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
      enqueue(-stepsPerRevolution);
      lastTrigger1 = now; pause = now;
    }
    if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
      enqueue(-2 * stepsPerRevolution);
      lastTrigger2 = now; pause = now;
    }
    if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
      enqueue(-3 * stepsPerRevolution);
      lastTrigger3 = now; pause = now;
    }
  }

  if (endPositionAktiv == 1) {
    queueHead = queueTail;   // alle Aufgaben löschen
    taskActive = false;
    disableCoils();          // Motor komplett stromlos
    digitalWrite(ledPin, LOW);
  }

  // --- Motorsteuerung über Queue ---
  if (!taskActive && dequeue(currentTask)) {
    enableCoils(); // Spulen aktivieren, bevor Motor läuft
    stepper.move(currentTask.steps);
    taskActive = true;
    digitalWrite(ledPin, HIGH);
  }

  if (taskActive) {
    if (stepper.distanceToGo() != 0) {
      stepper.setSpeed(-500);
      stepper.run();
     

    } else {
      taskActive = false;
      disableCoils(); // Bewegung fertig → Spulen aus
      digitalWrite(ledPin, LOW);
    }
  } else {
    disableCoils(); // falls kein Task aktiv ist
  }
}
