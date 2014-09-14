#include <Adafruit_NeoPixel.h>
#include <Firmata.h>

#define PIN 6

#define POTPIN 2    // select the input pin for the potentiometer


int val = 0;       // variable to store the value coming from the sensor
#define WIDTH 8
#define HEIGHT 8

uint16_t screen[WIDTH][HEIGHT];

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

uint16_t start = 0;

void setup() {
  strip.begin();
  //strip.setBrightness(16);
  strip.show(); // Initialize all pixels to 'off'
}

void map() {
  float centerX = WIDTH / 2;
  float centerY = HEIGHT / 2;
  float radialX, radialY, rad;
  rad = WIDTH / 2;
  for(int i=0;i<360;i++){
    radialX = centerX + sin(i) * rad;
    radialY = centerY + cos(i) * rad;
    
  }
}

void loop() {
  Firmata.blinkVersion();
  // Some example procedures showing how to display to the pixels:
  colorChase(strip.Color(255, 0, 0), 25); // Red
  colorUnchase(strip.Color(0, 255, 0), 15); // Red
  //colorWipe(start, strip.Color(0, 0, 0), 2); // Green
  //colorWipe(strip.Color(0, 255, 0), 1); // Blue
  //rainbow(1);
  //val = analogRead(POTPIN);    // read the value from the sensor
  //rainbowCycle(20);
  
  //rainbow(2);
}

void colorChase(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      //colorWipe(i+1, strip.Color(0, 0, 0), 0);
      strip.setPixelColor(i, c);
      strip.setPixelColor(i==0?strip.numPixels()-1:i-1, strip.Color(0, 0, 0));
      strip.show();
      delay(wait);
  }
}

void colorUnchase(uint32_t c, uint8_t wait) {
  for(uint16_t i=strip.numPixels(); i>0; i--) {
      //colorWipe(i+1, strip.Color(0, 0, 0), 0);
      strip.setPixelColor(i-1, c);
      strip.setPixelColor(i==strip.numPixels()-1?0:i, strip.Color(0, 0, 0));
      strip.show();
      delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint16_t s, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      uint16_t p = (s+i) % strip.numPixels();
      strip.setPixelColor(p, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

