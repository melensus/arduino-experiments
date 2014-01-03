
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define PIXELS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t pixel = 1;            // pixel number that you're changing
uint32_t red = 0;              // red value 
uint32_t green = 0;           // green value
uint32_t blue = 0;            // blue value
uint32_t bri = 40;
uint8_t isReady = 0;

void setup() {
  Serial.setTimeout(5000);
  Serial.begin(38400); 
  strip.setBrightness(40);  
  strip.begin();    
  setAllPixelsColor();
  
  strip.setBrightness(40);  
  strip.show();
}

void loop() {
  
  if(!isReady){
    Serial.println("ready");
  }
  
  if (Serial.available() > 0) {
    // Example color command : "C2,255,0,0" : turns 3rd pixel red.
    int cmd = Serial.read();
    
    isReady = 1;
    if(cmd == 'M'){
      
//      strip.setBrightness(bri);  
      char buffer[3 * PIXELS];
      
      int bytesRead = Serial.readBytes(buffer, 3 * PIXELS);
      for(int i=0;i<bytesRead;i++){
        int j = i*3;
        uint32_t clr = 0;
        clr |= (uint32_t)buffer[j++] << 16 |
              (uint32_t)buffer[j++]  << 8 |
              (uint32_t)buffer[j];
        strip.setPixelColor(i, clr);
      }
    }else if (cmd == 'C') {    // string should start with C
      pixel = Serial.parseInt();   
      red = Serial.parseInt();   
      green = Serial.parseInt(); 
      blue = Serial.parseInt();  
      strip.setPixelColor(pixel, red, green, blue);
    } else if (cmd == 'A') {
      red = Serial.parseInt();     
      green = Serial.parseInt();  
      blue = Serial.parseInt();   
      setAllPixelsColor();
    } else if (cmd == 'B') {
      bri = Serial.parseInt();
      strip.setBrightness(bri);
    }
    //strip.setPixelColor(0,255,0,0);
    
    //strip.setPixelColor(4,0,255,0);
    strip.show();
  }
}

void setAllPixelsColor(){
  for(int i=0;i<PIXELS;i++){
    strip.setPixelColor(i, red, green, blue);
  }
}
