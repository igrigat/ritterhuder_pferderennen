#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      14   // LEDs: Index 0..13

const int bahnROTin   = 2;
const int bahnGELBin  = 3;
const int bahnGRUENin = 4;
const int bahnBLAUin  = 5;

const int bahnROTout   = 7;
const int bahnGELBout  = 8;
const int bahnGRUENout = 9;
const int bahnBLAUout  = 10;

const int start = 11;

int var = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// ---------------- Hilfsfunktionen ----------------

void setAllOutputsHigh() {
  digitalWrite(bahnROTout, HIGH);
  digitalWrite(bahnGELBout, HIGH);
  digitalWrite(bahnBLAUout, HIGH);
  digitalWrite(bahnGRUENout, HIGH);
}

void setAllOutputsLow() {
  digitalWrite(bahnROTout, LOW);
  digitalWrite(bahnGELBout, LOW);
  digitalWrite(bahnBLAUout, LOW);
  digitalWrite(bahnGRUENout, LOW);
}

// Winner-Animation: abwechselnd (off, Farbe) und (Farbe, off) auf Paaren
void showWinner(uint8_t r, uint8_t g, uint8_t b, uint8_t repeats = 30) {
  for (uint8_t i = 0; i < repeats; i++) {
    // Variante 1: gerade Pixel aus, ungerade an
    for (int k = 0; k < NUMPIXELS - 1; k += 2) {
      pixels.setPixelColor(k,     pixels.Color(0, 0, 0));
      pixels.setPixelColor(k + 1, pixels.Color(r, g, b));
    }
    pixels.show();
    delay(200);

    // Variante 2: gerade Pixel an, ungerade aus
    for (int k = 0; k < NUMPIXELS - 1; k += 2) {
      pixels.setPixelColor(k,     pixels.Color(r, g, b));
      pixels.setPixelColor(k + 1, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
  }
}

// Alle Pixel auf eine Farbe setzen
void fillStrip(uint8_t r, uint8_t g, uint8_t b) {
  for (int k = 0; k < NUMPIXELS; k++) {
    pixels.setPixelColor(k, pixels.Color(r, g, b));
  }
  pixels.show();
}

// ---------------- Setup ----------------

void setup() {
  pixels.begin();
  pixels.show(); // alles aus

  pinMode(start, INPUT_PULLUP);

  pinMode(bahnROTin,   INPUT_PULLUP);
  pinMode(bahnGELBin,  INPUT_PULLUP);
  pinMode(bahnGRUENin, INPUT_PULLUP);
  pinMode(bahnBLAUin,  INPUT_PULLUP);

  pinMode(bahnROTout,   OUTPUT);
  pinMode(bahnGELBout,  OUTPUT);
  pinMode(bahnGRUENout, OUTPUT);
  pinMode(bahnBLAUout,  OUTPUT);

  setAllOutputsHigh();
}

// ---------------- Loop ----------------

void loop() {

  // Grundzustand: alle HIGH
  setAllOutputsHigh();

  // --- ROT gewinnt ---
  if (digitalRead(bahnROTin) == LOW) {
    setAllOutputsLow();
    // Achtung: deine ursprüngliche Farbwahl war (0,100,0) = Grün
    // Wenn du wirklich ROT willst: (100, 0, 0)
    showWinner(0, 100, 0);  // wie im Originalcode
    return;
  }

  // --- GELB gewinnt ---
  if (digitalRead(bahnGELBin) == LOW) {
    setAllOutputsLow();
    showWinner(90, 100, 0); // Gelblich
    return;
  }

  // --- BLAU gewinnt ---
  if (digitalRead(bahnBLAUin) == LOW) {
    setAllOutputsLow();
    showWinner(100, 0, 0);  // in deinem Code war das "BLAU" (eigentlich Rot)
    return;
  }

  // --- GRÜN gewinnt ---
  if (digitalRead(bahnGRUENin) == LOW) {
    setAllOutputsLow();
    showWinner(0, 0, 100);  // in deinem Code "GRUEN" (eigentlich Blau)
    return;
  }

  // --- "Rennen"-Effekt im Hintergrund ---
  var = var + 1;
  if (var >= 2400) {      // >= ist robuster als ==
    var = 0;
    for (int k = 0; k < NUMPIXELS; k++) {
      pixels.setPixelColor(k, pixels.Color(random(0, 100), random(0, 100), random(0, 100)));
    }
    pixels.show();
  }

  // --- Start-Sequenz ---
  if (digitalRead(start) == LOW) {
    // 3x grünes Blinken
    for (int i = 0; i < 3; i++) {
      fillStrip(0, 100, 0);  // Grün
      delay(400);
      fillStrip(0, 0, 0);    // Aus
      delay(400);
    }

    // Blau zum Startsignal
    fillStrip(0, 0, 100);
    delay(200);

    // Bahnen "starten" (LOW, wie bei dir)
    setAllOutputsLow();
  }
}
