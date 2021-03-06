#include "SPI.h"
#include "Adafruit_WS2801.h"

/*****************************************************************************
 * Example sketch for driving Adafruit WS2801 pixels!
 * 
 * 
 * Designed specifically to work with the Adafruit RGB Pixels!
 * 12mm Bullet shape ----> https://www.adafruit.com/products/322
 * 12mm Flat shape   ----> https://www.adafruit.com/products/738
 * 36mm Square shape ----> https://www.adafruit.com/products/683
 * 
 * These pixels use SPI to transmit the color data, and have built in
 * high speed PWM drivers for 24 bit color per pixel
 * 2 pins are required to interface
 * 
 * Adafruit invests time and resources providing this open source code, 
 * please support Adafruit and open-source hardware by purchasing 
 * products from Adafruit!
 * 
 * Written by Limor Fried/Ladyada for Adafruit Industries.  
 * BSD license, all text above must be included in any redistribution
 * 
 *****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

boolean standby = true;
boolean resetFlag = true;

int signalFromBase = 5;
int resetSwitch = 4;

// Pin 4 to standby/activate mode switch
// Pin 5 to control panel
void setup() {
  Serial.println("Reboot");

  randomSeed(1);
  pinMode(resetSwitch, INPUT); //For state change from idle to active
  digitalWrite(resetSwitch, HIGH); //Enable internal pullup resistor
  pinMode(signalFromBase, INPUT); // For signal from control panel
  digitalWrite(signalFromBase, HIGH);
  strip.begin();
  strip.show();

}


void loop() {

  startup();
  idleMode();
  preActivation();
  breakdown();
  preActivation();
  showtime();
}

void breakdown(void){
  if(resetFlag == true){
    return;
  }
  Serial.println("In breakdown waiting for base to go low");
  while(digitalRead(signalFromBase) == HIGH){
    // Wait for base to go low
  }
  delay(10);
  Serial.println("BREAKDOWN");
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Color(0,0,0));
  }
  strip.show();

  while(1){
    if(digitalRead(resetSwitch) == LOW){
      delay(10);
      if(digitalRead(resetSwitch) == LOW){
        resetFlag = true;
        return;
      }
    }
    if(digitalRead(signalFromBase) == HIGH){
      delay(10);
      if(digitalRead(signalFromBase) == HIGH){
        Serial.println("Waiting for base to go low before exiting breakdown");
        while(digitalRead(signalFromBase) == HIGH){
        }
        return;
      }
    }
  }
}

void preActivation(void){
  if(resetFlag == true){
    return;
  }
  Serial.println("Entering preactivation stage");
  while(1){
    if(digitalRead(resetSwitch) == LOW){
      delay(10);
      if(digitalRead(resetSwitch) == LOW){
        resetFlag = true;
        return;
      }
    }
    if(digitalRead(signalFromBase) == HIGH){
      delay(10);
      if(digitalRead(signalFromBase) == HIGH){
        return;
      }
    }
    for(int i=0;i<strip.numPixels();i++){
      strip.setPixelColor(random(strip.numPixels()),Color(random(255),random(255),random(255)));
      strip.show();
      delay(8);
    }
  }
}

void startup(void){
  Serial.println("Starting up");
  resetFlag = false;
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Color(255,0,0));
  }
  strip.show();
  delay(2000);
  Serial.println("In startup, waiting for base to go low");
  while(digitalRead(signalFromBase)==HIGH){
    //Wait for base
  }
  long lowCount = 0;
  while(digitalRead(signalFromBase) == LOW){
    if(lowCount++ >=50){
      break;
    }
    delay(10);
  }
  if(lowCount <50){
    resetFlag = true;
    Serial.println("Base didn't stay low long enough. Going around again.");
    return;
  }
  Serial.println("Base is low. Disk set green. Startup complete");
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Color(0,255,0));
  }
  strip.show();
  delay(5000);
}

void showtime(void){
  if(resetFlag == true){
    return;
  }
  Serial.println("SHOWTIME!!!");
  long showStart = millis();
  while(1){
    Serial.print("Elapsed millis since SHOWTIME: ");
    Serial.println(millis() - showStart);
    if(digitalRead(resetSwitch) == LOW){
      delay(10);
      if(digitalRead(resetSwitch) == LOW){
        resetFlag = true;
        return;
      }
    }
    if((millis() - showStart) < 18000){
      int sweepDelay = random(200);
      Serial.print("Color sweep (200): ");
      Serial.println(sweepDelay);
      colorWipe(Color(random(255), random(255), 0), sweepDelay);
      colorWipe(Color(0, random(255), random(255)), sweepDelay);
      colorWipe(Color(random(255), 0, random(255)), sweepDelay);
    }
    else if((millis() - showStart) < 32000){
      Serial.println("Going to rainbow cycle");
      rainbowCycle(5);
    }
    else{
      int sweepDelay = random(50);
      Serial.print("Color sweep (50): ");
      Serial.println(sweepDelay);
      colorWipe(Color(random(255), random(255), 0), sweepDelay);
      colorWipe(Color(0, random(255), random(255)), sweepDelay);
      colorWipe(Color(random(255), 0, random(255)), sweepDelay);
    }
  }
}

void idleMode(void){
  if(resetFlag == true){
    return;
  }
  Serial.println("Entering idleMode");
  while(1){
    if(digitalRead(signalFromBase) == HIGH){
      Serial.println("In idle mode. Base has gone high");
      delay(10);
      if(digitalRead(signalFromBase) == HIGH){
        while(digitalRead(signalFromBase) == HIGH){
        }
        delay(20);
        return;
      }
    }
    fadeToTarget(random(0,35),random(20,255),random(20,255),250);
    fadeToTarget(random(20,255),random(0,35),random(20,255),250);
    fadeToTarget(random(20,255),random(20,255),random(0,35),250);
    if(resetFlag == true){
      return;
    }
  }
}

void fadeToTarget(int rTarget, int gTarget, int bTarget, int interval){
  static int r=0;
  static int g=0;
  static int b=0;
  char targetMsg[90];
  sprintf(targetMsg, "rTarget: %03d, gTarget: %03d, bTarget: %03d", rTarget, gTarget, bTarget);
  Serial.println(targetMsg);

  while(r!=rTarget || g!=gTarget || b!=bTarget){
    if(digitalRead(4) == LOW && standby == true){
      standby = false;
      break;
    }
    r=convergeColorValue(r,rTarget);
    g=convergeColorValue(g,gTarget);
    b=convergeColorValue(b,bTarget);
    for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Color(r,g,b));
    }
    if(standby == false){
      //Serial.print(interval);
      //Serial.print(": ");
    }
    char statusMsg[90];
    sprintf(statusMsg, "r -> rTarget: %d -> %d, g -> gTarget: %d -> %d, b -> bTarget: %d -> %d", r, rTarget, g, gTarget, b, bTarget);
    //Serial.println(statusMsg);
    strip.show();
    if(digitalRead(signalFromBase) == HIGH){
      return;
    }
    if(digitalRead(resetSwitch) == LOW){
      delay(10);
      if(digitalRead(resetSwitch) == LOW){
        resetFlag = true;
        return;
      }
    }
    delay(interval);
  }

}
int convergeColorValue(int current, int target){
  if(current<target){
    current++;
  }
  else if(current>target){
    current--;
  }
  if(standby == false){ // if we have exited standby mode, add an extra increment/decrement to move us towards target faster
    if(current<target){
      current++;
    }
    else if(current>target){
      current--;
    }
  }
  return current;
}

void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  if(resetFlag == true){
    return;
  }

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    if(digitalRead(resetSwitch) == LOW){
      resetFlag = true;
      return;
    }
    delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170; 
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}























