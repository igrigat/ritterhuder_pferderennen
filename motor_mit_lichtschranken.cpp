#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Pins für die Lichtschranken
const int sensor1Pin = 2;
const int sensor2Pin = 3;
const int sensor3Pin = 4;

// Pins für den Schrittmotor
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

// Zahlen fürs Matrix-Display

uint8_t frame1[8][12] = {
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

uint8_t frame2[8][12] = {
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

uint8_t frame3[8][12] = {
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// Schritte pro Umdrehung für den 28BYJ-48 Motor
const int stepsPerRevolution = 1024;

// Für den nicht-blockierenden Motorlauf
unsigned long lastStepTime = 0;
const int stepDelay = 2;  // ms pro Halbschritt
int stepState = 0;

// Queue-Struktur
struct Task {
  uint8_t (*frame)[12];
  long steps;
};

const int MAX_TASKS = 20;  // Puffergröße
Task queue[MAX_TASKS];
int queueHead = 0;
int queueTail = 0;
bool taskActive = false;

// aktueller Status
Task currentTask;

// Sensor-Entprellung
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
unsigned long lastTrigger3 = 0;
const unsigned long debounceTime = 200; // ms

// ---------------------- Motor ----------------------
void stepMotorNonBlocking() {
  switch (stepState) {
    case 0:
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      break;
    case 1:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      break;
    case 2:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
      break;
    case 3:
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
      break;
  }
  stepState = (stepState + 1) % 4;
}

// ---------------------- Queue ----------------------
void enqueue(uint8_t frame[8][12], long steps) {
  int nextTail = (queueTail + 1) % MAX_TASKS;
  if (nextTail != queueHead) {  // nur, wenn nicht voll
    queue[queueTail].frame = frame;
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
  matrix.begin();

  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
  pinMode(sensor3Pin, INPUT);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

// ---------------------- Loop ----------------------
void loop() {
  unsigned long now = millis();

  // Sensor 1 prüfen
  if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
    enqueue(frame1, stepsPerRevolution);
    lastTrigger1 = now;
  }

  // Sensor 2 prüfen
  if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
    enqueue(frame2, 2 * stepsPerRevolution);
    lastTrigger2 = now;
  }

  // Sensor 3 prüfen
  if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
    enqueue(frame3, 3 * stepsPerRevolution);
    lastTrigger3 = now;
  }

  // Falls kein Task läuft → neuen starten
  if (!taskActive && dequeue(currentTask)) {
    taskActive = true;
    matrix.renderBitmap(currentTask.frame, 8, 12);
  }

  // Falls Task läuft → Motor bewegen
  if (taskActive) {
    if (currentTask.steps > 0) {
      if (now - lastStepTime >= stepDelay) {
        lastStepTime = now;
        stepMotorNonBlocking();
        currentTask.steps--;
      }
    } else {
      // Task fertig → Anzeige löschen
      matrix.clear();
      taskActive = false;
    }
  }
}
