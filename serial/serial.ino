#include <Adafruit_NeoPixel.h>

//Start config

//The arduino digital pin the neopixel data in is connected to
#define PIN 6

//Number of neopixels connected to arduino
#define PIXELS 16


//End config

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

// the possible states of the state-machine.  reads ascii characters.
typedef enum {
                NONE, //0 Nothing received.  Awaiting input
                
                GOT_C, //1 Recevied "C".  We now expect to recieve one or more (P)ixel,(R)ed,(G)reen,(B)lue messages. to complete this command. Will display when "D" is received. example : C5,255,255,200,9,255,255,100,D displays 2 diff colors on pixels 5 and 9.
                  GOT_C_P, //2  Recieved pixel number argument.  Expecting (R)ed next.
                  GOT_C_P_R, //3 Recieved pixel and red.  Expecting (G)reen next.
                  GOT_C_P_R_G, //4 Recieved pixel, red, and green.  Expecting (B)lue next.  After blue is received, we'll go straight back to NONE state, so there is no GOT_C_P_R_G_B state. example : example : M255,255,200,255,255,100,D displays 2 diff colors on pixels 0 and 1.
                  
                GOT_M, //5 Received "M" expecting to receive RGB colors for each pixel, starting with the first one.  Will display when "D" is received.
                  GOT_M_R, //6
                  GOT_M_R_G, //7
                  GOT_M_R_G_B, //8
                  
                GOT_B, //9 Received "B" for Brightness.  sets the brightness (0-100) to the number following for every pixel example : B50D sets brightness to 50% for all pixels
                
                GOT_A, //10 Received "A" for All.  Sets all pixels to the following color.  Example : A255,100,50D
                  GOT_A_R, //11
                  GOT_A_R_G, //12
                  GOT_A_R_G_B, //13
                  
                GOT_P, //14 Received "P" for Ping.  replies with "READY".
                
                GOT_D  //15 Received "D".  Will display whatever commands have been sent since the last display.
                
             } states;

// current state-machine state
states state = NONE;
// current partial number
uint32_t currentValue;
uint16_t currentPixel;

void setup ()
{ 
  strip.begin();    
  strip.setBrightness(50); 
  for(int i=0;i<PIXELS;i++){
    strip.setPixelColor(i, 100,100,100);
  }
  //strip.setPixelColor(25, 255, 0, 0);
  //setAllPixelsColor();
  
  Serial.begin (19200);
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

void debugOutput ()
{
  Serial.println(state);
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
  
  debugOutput ();
} // end of processIncomingByte

void loop ()
{ 
}  // end of loop

void serialEvent() {
  while (Serial.available()) {
    processIncomingByte (Serial.read ());
  }
}


