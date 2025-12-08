/*
Stripe mit eigenem 24V Netzteil. 24+ an Stripe, GND an Stripe+Board GND, C(old) und W(arm) anschluss einfach mit digital Pin ansteuerbar um warm o kaltweiß zu schalten - hier in der Schaltung nicht berücksichtigt.
PIN 6 an den DI des Stripes gehängt und mit Adafruit_Nepixel Library angesteuert.
*/
/* Segmente 1-14, Helligkeit der Farben BLAU, ROT, GRUEN
  pixels.setPixelColor(0, pixels.Color(0, 0, 100));
  pixels.setPixelColor(1, pixels.Color(0, 100, 0));
  pixels.setPixelColor(2, pixels.Color(100, 0, 0));
  pixels.setPixelColor(3, pixels.Color(0, 50, 50));
  pixels.setPixelColor(4, pixels.Color(50, 50, 0));
  pixels.setPixelColor(5, pixels.Color(100, 0, 0));
  pixels.setPixelColor(6, pixels.Color(0, 0, 100));
  pixels.setPixelColor(7, pixels.Color(0, 100, 0));
  pixels.setPixelColor(8, pixels.Color(100, 0, 0));
  pixels.setPixelColor(9, pixels.Color(0, 100, 0));
  pixels.setPixelColor(10, pixels.Color(100, 0, 0));
  pixels.setPixelColor(11, pixels.Color(100, 0, 0));
  pixels.setPixelColor(12, pixels.Color(0, 100, 0));
  pixels.setPixelColor(13, pixels.Color(100, 0, 0));
  pixels.setPixelColor(14, pixels.Color(100, 0, 0));
// bei SHOW werden die Werte gesammelt an den LED Stripe geschickt.
  pixels.show(); */

#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      14   //vorher 100

const int bahnROTin = 2;  
const int bahnGELBin = 3; 
const int bahnGRUENin = 4;
const int bahnBLAUin = 5; 
const int bahnROTout = 7;  
const int bahnGELBout = 8; 
const int bahnGRUENout = 9;
const int bahnBLAUout = 10; 
const int start = 11;
int var = 0;
int var2=0;
int var3=0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pinMode(start, INPUT_PULLUP);
  pinMode(bahnROTin, INPUT_PULLUP);
  pinMode(bahnGELBin, INPUT_PULLUP);
  pinMode(bahnBLAUin, INPUT_PULLUP);
  pinMode(bahnGRUENin, INPUT_PULLUP);
  pinMode(bahnROTout, OUTPUT);
  pinMode(bahnGELBout, OUTPUT);
  pinMode(bahnBLAUout, OUTPUT);
  pinMode(bahnGRUENout, OUTPUT);
}

void loop() {

digitalWrite(bahnROTout, HIGH);
digitalWrite(bahnGELBout, HIGH);
digitalWrite(bahnBLAUout, HIGH);
digitalWrite(bahnGRUENout, HIGH);

//ROT gewinnt
if (digitalRead(bahnROTin) == LOW){
  var2 = 75;
digitalWrite(bahnROTout, LOW);
digitalWrite(bahnGELBout, LOW);
digitalWrite(bahnBLAUout, LOW);
digitalWrite(bahnGRUENout, LOW);
  for (int i = 0; i <= 30; i++) {
    for (int k = 0; k<=14; k+=2) {
    pixels.setPixelColor(k, pixels.Color(0, 0, 0));
  pixels.setPixelColor(k+1, pixels.Color(0, 90, 0));
    }
    pixels.show();
    delay(200);
      for (int k = 0; k<=14; k+=2) {
    pixels.setPixelColor(k, pixels.Color(0, 90, 0));
  pixels.setPixelColor(k+1, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
    } 
    }
//GELB gewinnt
if (digitalRead(bahnGELBin) == LOW){
  var2 = 75;
digitalWrite(bahnROTout, LOW);
digitalWrite(bahnGELBout, LOW);
digitalWrite(bahnBLAUout, LOW);
digitalWrite(bahnGRUENout, LOW);
  for (int i = 0; i <= 30; i++) {
    for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(0, 0, 0));
  pixels.setPixelColor(k+1, pixels.Color(0, 90, 75));
    }
    pixels.show();
    delay(200);
  for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(0, 90, 75));
  pixels.setPixelColor(k+1, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
    } 
    }
if (digitalRead(bahnBLAUin) == LOW){
  var2 = 75;
digitalWrite(bahnROTout, LOW);
digitalWrite(bahnGELBout, LOW);
digitalWrite(bahnBLAUout, LOW);
digitalWrite(bahnGRUENout, LOW);
  for (int i = 0; i <= 30; i++) {
    for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(0, 0, 0));
  pixels.setPixelColor(k+1, pixels.Color(90, 0, 0));
    }
    pixels.show();
    delay(200);
     for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(90, 0, 0));
  pixels.setPixelColor(k+1, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
    } 
    }
if (digitalRead(bahnGRUENin) == LOW){
  var2 = 70;
digitalWrite(bahnROTout, LOW);
digitalWrite(bahnGELBout, LOW);
digitalWrite(bahnBLAUout, LOW);
digitalWrite(bahnGRUENout, LOW);
  for (int i = 0; i <= 30; i++) {
     for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(0, 0, 0));
  pixels.setPixelColor(k+1, pixels.Color(0, 0, 90));
    }
    pixels.show();
    delay(200);
 for (int k = 0; k<=14; k+=2){
    pixels.setPixelColor(k, pixels.Color(0, 0, 90));
  pixels.setPixelColor(k+1, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(200);
    } 
    }
 
 //rennen
var = var+1;
if (var == 6000){
var=0;
switch (var3) {
  case 0:
    for (int k = 0; k < 15; k += 4) {
      pixels.setPixelColor(k,   pixels.Color(0, 0, 90-var2));        // grün
      pixels.setPixelColor(k+1, pixels.Color(0, 90-var2, 0));        // rot
      pixels.setPixelColor(k+2, pixels.Color(0, 90-var2, 80-var2));  // gelb
      pixels.setPixelColor(k+3, pixels.Color(90-var2, 0, 0));        // blau
    }
    var3 = 1;
    break;

  case 1:
    for (int k = 0; k < 15; k += 4) {
      pixels.setPixelColor(k+1, pixels.Color(0, 0, 90-var2));        // grün
      pixels.setPixelColor(k+2, pixels.Color(0, 90-var2, 0));        // rot
      pixels.setPixelColor(k+3, pixels.Color(0, 90-var2, 85-var2));  // gelb
      pixels.setPixelColor(k,   pixels.Color(90-var2, 0, 0));        // blau
    }
    var3 = 2;
    break;

  case 2:
    for (int k = 0; k < 15; k += 4) {
      pixels.setPixelColor(k+2, pixels.Color(0, 0, 90-var2));        // grün
      pixels.setPixelColor(k+3, pixels.Color(0, 90-var2, 0));        // rot
      pixels.setPixelColor(k,   pixels.Color(0, 90-var2, 85-var2));  // gelb
      pixels.setPixelColor(k+1, pixels.Color(90-var2, 0, 0));        // blau
    }
    var3 = 3;
    break;

  case 3:
    for (int k = 0; k < 15; k += 4) {
      pixels.setPixelColor(k+3, pixels.Color(0, 0, 90-var2));        // grün
      pixels.setPixelColor(k,   pixels.Color(0, 90-var2, 0));        // rot
      pixels.setPixelColor(k+1, pixels.Color(0, 90-var2, 85-var2));  // gelb
      pixels.setPixelColor(k+2, pixels.Color(90-var2, 0, 0));        // blau
    }
    var3 = 0;  // zurücksetzen
    break;
}
pixels.show();

}



//Starten
if(digitalRead(start) == LOW){
  var2 = 10;
 for (int i=0; i<=2; i++){
for (int k = 0; k<=14; k++){
 pixels.setPixelColor(k, pixels.Color(0, 100, 0)); 
}
pixels.show();
delay(400);
for (int k = 0; k<=14; k++){
 pixels.setPixelColor(k, pixels.Color(0, 0, 0)); 
}
pixels.show();
delay(400);
}
for (int k = 0; k<=14; k++){
pixels.setPixelColor(k, pixels.Color(0, 0, 100));
}
pixels.show();
//digitalWrite(bahnROTout, LOW);
//digitalWrite(bahnGELBout, LOW);
//digitalWrite(bahnBLAUout, LOW);
//digitalWrite(bahnGRUENout, LOW);
delay(200);
}

}
