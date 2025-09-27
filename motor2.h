#include <Arduino_LED_Matrix.h>
#include <AccelStepper.h>

ArduinoLEDMatrix matrix;

// Pins für die Lichtschranken
const int sensor1Pin = 2;
const int sensor2Pin = 3;
const int sensor3Pin = 4;

// 28BYJ-48 mit ULN2003 im 4-Draht-Modus
// WICHTIG: Reihenfolge 1,3,2,4
AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 10, 9, 11);

// Schritte pro Umdrehung (angepasst auf 28BYJ-48)
const int stepsPerRevolution = 2048; // oder dein Wert

// Frames
uint8_t frame1[8][12] = { /* dein Bild 1 */ };
uint8_t frame2[8][12] = { /* dein Bild 2 */ };
uint8_t frame3[8][12] = { /* dein Bild 3 */ };

// Task-Struktur für die Queue
struct Task {
  uint8_t (*frame)[12];
  long steps;    // positiv = vorwärts, negativ = rückwärts
};

const int MAX_TASKS = 20;
Task queue[MAX_TASKS];
int queueHead = 0;
int queueTail = 0;
bool taskActive = false;
Task currentTask;

// Sensor-Entprellung
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
unsigned long lastTrigger3 = 0;
const unsigned long debounceTime = 200; // ms

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

  // AccelStepper einstellen
  stepper.setMaxSpeed(1000.0);      // maximale Geschwindigkeit (Schritte/Sek.)
  stepper.setAcceleration(300.0);   // Beschleunigung (Schritte/Sek.^2)
}

// ---------------------- Loop ----------------------
void loop() {
  unsigned long now = millis();

  // Sensor 1 → vorwärts 1 Umdrehung
  if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
    enqueue(frame1, stepsPerRevolution);
    lastTrigger1 = now;
  }

  // Sensor 2 → rückwärts 2 Umdrehungen
  if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
    enqueue(frame2, -2 * stepsPerRevolution);
    lastTrigger2 = now;
  }

  // Sensor 3 → vorwärts 3 Umdrehungen
  if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
    enqueue(frame3, 3 * stepsPerRevolution);
    lastTrigger3 = now;
  }

  // Falls kein Task läuft → neuen starten
  if (!taskActive && dequeue(currentTask)) {
    taskActive = true;
    matrix.renderBitmap(currentTask.frame, 8, 12);
    stepper.move(currentTask.steps); // Bewegung starten
  }

  // Falls Task läuft
  if (taskActive) {
    if (stepper.distanceToGo() != 0) {
      stepper.run();  // AccelStepper abarbeiten
    } else {
      // Task fertig
      matrix.clear();
      taskActive = false;
    }
  }
}