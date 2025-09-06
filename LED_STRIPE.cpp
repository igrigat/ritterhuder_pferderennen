*/
Stripe mit eigenem 24V Netzteil. 24+ an Stripe, GND an Stripe+Board GND, C(old) und W(arm) anschluss einfach mit digital Pin ansteuerbar um warm o kaltweiß zu schalten - hier in der Schaltung nicht berücksichtigt.
PIN 6 an den DI des Stripes gehängt und mit Adafruit_Nepixel Library angesteuert.
*/

#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      100

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
}

void loop() {
// Segmente 1-14, Helligkeit der Farben BLAU, ROT, GRUEN
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
  pixels.show();
}
