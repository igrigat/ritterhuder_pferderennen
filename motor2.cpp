#include <AccelStepper.h>

// ---------------------- Pinbelegung ----------------------
// Lichtschranken
const int sensor1Pin = 2;
const int sensor2Pin = 3;
const int sensor3Pin = 4;

// Schrittmotor (28BYJ-48 an ULN2003)
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

// LED für Motoraktivität
const int ledPin = 13;  // interne Arduino-LED

// Richtige Reihenfolge: 1,3,2,4
AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

// Schritte pro Umdrehung für den 28BYJ-48
const int stepsPerRevolution = 4096;

// ---------------------- Queue-Struktur ----------------------
struct Task {
  long steps;
};

const int MAX_TASKS = 20;
Task queue[MAX_TASKS];
int queueHead = 0;
int queueTail = 0;
bool taskActive = false;
Task currentTask;

// ---------------------- Entprellung ----------------------
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
unsigned long lastTrigger3 = 0;
const unsigned long debounceTime = 200; // ms
unsigned long pause = 0;

// ---------------------- Queue-Funktionen ----------------------
void enqueue(long steps) {
  int nextTail = (queueTail + 1) % MAX_TASKS;
  if (nextTail != queueHead) {
    queue[queueTail].steps = steps;
    queueTail = nextTail;
  }
}

bool dequeue(Task &t) {
  if (queueHead == queueTail) return false; // leer
  t = queue[queueHead];
  queueHead = (queueHead + 1) % MAX_TASKS;
  return true;
}

// ---------------------- Setup ----------------------
void setup() {
  pinMode(sensor1Pin, INPUT_PULLUP);
  pinMode(sensor2Pin, INPUT_PULLUP);
  pinMode(sensor3Pin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Stabile Werte für 28BYJ-48
  stepper.setMaxSpeed(682);      // Schritte/Sekunde
  stepper.setAcceleration(200);   // Schritte/Sekunde²
}

// ---------------------- Loop ----------------------
void loop() {
  unsigned long now = millis();

  // --- Neue Aufgaben nur alle 4s zulassen ---
  if (pause + 4000 <= now) {
    if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
      enqueue(+stepsPerRevolution);   // 1 Umdrehung vorwärts
      lastTrigger1 = now;
      pause = now;
    }

    if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
      enqueue(-stepsPerRevolution);   // 1 Umdrehung rückwärts
      lastTrigger2 = now;
      pause = now;
    }

    if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
      enqueue(+2 * stepsPerRevolution); // 2 Umdrehungen vorwärts
      lastTrigger3 = now;
      pause = now;
    }
  }

  // --- Motorsteuerung über Queue ---
  if (!taskActive && dequeue(currentTask)) {
    stepper.move(currentTask.steps);
    taskActive = true;
    digitalWrite(ledPin, HIGH);  // LED an beim Start
  }

  if (taskActive) {
    if (stepper.distanceToGo() != 0) {
      stepper.run(); // Motor Schritt für Schritt bewegen
    } else {
      taskActive = false;         // Aufgabe fertig
      digitalWrite(ledPin, LOW);  // LED aus, wenn Aufgabe abgeschlossen
    }
  } else {
    stepper.run(); // im Leerlauf weiter aufrufen
  }
}
