#include <AccelStepper.h>

// ---------------------- Pinbelegung ----------------------
// Lichtschranken
const int sensor1Pin = 2; // Loch 1
const int sensor2Pin = 3; // Loch 2
const int sensor3Pin = 4; // Loch 3
const int sensor4Pin = 5; // Sensor Anfangsposition (Stopper für Taster)
const int sensor5Pin = 6; // Sensor Endposition (Endschalter)
const int taster6Pin = 7; // Start-Taster (Motor läuft bis sensor4 aktiv)

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

// ---------------------- Lauf-Status ----------------------
bool manualRunActive = false;  // true, wenn Motor über Taster läuft

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
  pinMode(sensor4Pin, INPUT_PULLUP);
  pinMode(sensor5Pin, INPUT_PULLUP);
  pinMode(taster6Pin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  stepper.setMaxSpeed(682);      // Schritte/Sekunde
  stepper.setAcceleration(200);  // Schritte/Sekunde²
}

// ---------------------- Loop ----------------------
void loop() {
  unsigned long now = millis();

  // --- Manuelle Steuerung über Taster ---
  if (digitalRead(taster6Pin) == LOW && !manualRunActive) {
    manualRunActive = true;
    digitalWrite(ledPin, HIGH);
    stepper.setSpeed(400); // konstante Drehgeschwindigkeit (vorwärts)
  }

  // Wenn manuell aktiv: Motor läuft, bis sensor4 LOW meldet
  if (manualRunActive) {
    if (digitalRead(sensor4Pin) == LOW) {
      manualRunActive = false;
      stepper.stop();
      digitalWrite(ledPin, LOW);
    } else {
      stepper.runSpeed();
      return; // Queue-Logik überspringen, solange manuell läuft
    }
  }

  // --- Endschalter-Schutz ---
  bool endPositionAktiv = (digitalRead(sensor5Pin) == LOW);

  // --- Neue Aufgaben nur alle 4s zulassen ---
  if (!endPositionAktiv && pause + 4000 <= now) {
    if (digitalRead(sensor1Pin) == LOW && (now - lastTrigger1 > debounceTime)) {
      enqueue(+stepsPerRevolution);   // 1 Umdrehung vorwärts
      lastTrigger1 = now;
      pause = now;
    }

    if (digitalRead(sensor2Pin) == LOW && (now - lastTrigger2 > debounceTime)) {
      enqueue(+2 * stepsPerRevolution);   // 2 Umdrehungen vorwärts
      lastTrigger2 = now;
      pause = now;
    }

    if (digitalRead(sensor3Pin) == LOW && (now - lastTrigger3 > debounceTime)) {
      enqueue(+3 * stepsPerRevolution); // 3 Umdrehungen vorwärts
      lastTrigger3 = now;
      pause = now;
    }
  }

  // --- Wenn Endschalter aktiv: Queue leeren ---
  if (endPositionAktiv) {
    queueHead = queueTail;   // alle noch offenen Aufgaben verwerfen
    taskActive = false;      // falls Motor im Leerlauf
    digitalWrite(ledPin, LOW);
  }

  // --- Motorsteuerung über Queue ---
  if (!taskActive && dequeue(currentTask)) {
    stepper.move(currentTask.steps);
    taskActive = true;
    digitalWrite(ledPin, HIGH);  // LED an beim Start
  }

  if (taskActive) {
    if (stepper.distanceToGo() != 0) {
      stepper.run();
    } else {
      taskActive = false;
      digitalWrite(ledPin, LOW);
    }
  } else {
    stepper.run(); // im Leerlauf weiter aufrufen
  }
}
