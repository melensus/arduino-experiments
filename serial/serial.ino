/*


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





*/














#include <Adafruit_NeoPixel.h>

#define PIN 6
#define PIXELS 16

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);



// Example state machine reading serial input
// Author: Nick Gammon
// Date: 17 December 2011

// the possible states of the state-machine
typedef enum {  NONE, GOT_C, GOT_C_P, GOT_C_P_R, GOT_C_P_R_G, GOT_M, GOT_M_R, GOT_M_R_G, GOT_M_R_G_B, GOT_B, GOT_A, GOT_A_R, GOT_A_R_G, GOT_A_R_G_B, GOT_P, GOT_D } states;

// current state-machine state
states state = NONE;
// current partial number
uint32_t currentValue;
uint16_t currentPixel;

void setup ()
{ 
  strip.begin();    
  strip.setBrightness(100); 
  //strip.setPixelColor(25, 255, 0, 0);
  //setAllPixelsColor();
  
  Serial.begin (115200);
  state = NONE;
  
  strip.show();
}  // end of setup

void processPing ()
{
   Serial.println("READY");
}

void processMultipleColors ()
{
  strip.setPixelColor(currentPixel, currentValue);
}

void processDisplay()
{
  strip.show();
}

void processColor ()
{
  strip.setPixelColor(currentPixel, currentValue);
}

void processBrightness ()
{
  strip.setBrightness(currentValue);
}

void processAllColors ()
{
  for(int i=0;i<PIXELS;i++){
    strip.setPixelColor(i, currentValue);
  }
}

void handlePreviousState ()
{
  switch (state)
  {
    case GOT_D:
      processDisplay();
      break;
    case GOT_P:
      processPing();
      break;
    case GOT_M:
      processDisplay();
      break;
    case GOT_M_R_G_B:
      processMultipleColors ();
      break;
    case GOT_C:
      //char buff[50];
      //sprintf(buff, "val : %d", currentValue << 0);
      //Serial.write(buff);
      Serial.write('\n');
      processColor ();
      break;
    case GOT_B:
      processBrightness ();
      break;
    case GOT_A_R_G_B:
      processAllColors();
      break;
    default:
      return;
  }  // end of switch  

  currentValue = 0x00000000;
} 

void processIncomingByte (const byte c)
{
  
  
  uint32_t v = 0 | c;
  
  switch (c){
    case '\r':
    case '\n':
      handlePreviousState();
      state = NONE;
      //processPing();
      return;
  }
  
  switch (state)
  {
    case NONE:
      switch (c)
      {
        case 'P':
          state = GOT_P;
          break;
        case 'M':
          currentPixel = 0;
          state = GOT_M;
          break;
        case 'C':
          currentPixel = 0;
          state = GOT_C;
          break;
        case 'B':
          state = GOT_B;
          break;
        case 'A':
          state = GOT_A;
          break;
        case 'D':
          state = GOT_D;
          break;
      }
      break;
      
     case GOT_B:
      currentValue *= 10;
      currentValue += v - '0';
      break;
    case GOT_A:
      currentValue |= v << 16;
      state = GOT_A_R;
      break;
    case GOT_A_R:
      currentValue |= v << 8;
      state = GOT_A_R_G;
      break;
    case GOT_A_R_G:
      currentValue |= v;
      state = GOT_A_R_G_B;
      break;
    case GOT_M:
      currentValue |= v << 16;
      state = GOT_M_R;
      break;
    case GOT_M_R:
      currentValue |= v << 8;
      state = GOT_M_R_G;
      break;
    case GOT_M_R_G:
      state = GOT_M_R_G_B;
      currentValue |= v;
      handlePreviousState();
      currentPixel++;
      state = GOT_M;
      break;
    case GOT_C:
      switch(c){
        case ',':
          state = GOT_C_P;
          break;
        default:
          currentPixel *= 10;
          currentPixel += c - '0';
      }
      break;
    case GOT_C_P:
      currentValue |= v << 16;
      state = GOT_C_P_R;
      break;
    case GOT_C_P_R:
      currentValue |= v << 8;
      state = GOT_C_P_R_G;
      break;
    case GOT_C_P_R_G:
      currentValue |= v;
      state = GOT_C;
      break;
  }
} // end of processIncomingByte

void loop ()
{ 
}  // end of loop

void serialEvent() {
  while (Serial.available()) {
    processIncomingByte (Serial.read ());
  }
}


