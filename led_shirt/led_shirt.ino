/* 
 *  LED shirt
 *  
 *  Has several different animation modes, 
 *  pushing a button cycles through them
 *  and the mode number is shown on the display.
 *  
 */

// These are some settings for fastLED to work with the esp8266
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
FASTLED_USING_NAMESPACE

// These are some settings to tell fastLED what kind of LEDs you have
// and how we want them to run in terms of brightness etc. 
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  120

// There are four lines of LEDs, A,B,C and D.

// These are the pins for each of them
#define LED_PIN_A   D3
#define LED_PIN_B   D4
#define LED_PIN_C   D5
#define LED_PIN_D   D6

// This is the number of LEDs in each line 
#define NUM_LEDS_A    30
#define NUM_LEDS_B    28
#define NUM_LEDS_C    12
#define NUM_LEDS_D    30

// In total there are 100 LEDs, each LED can be thought of as having an ID of 0-99.
#define NUM_LEDS 100

// These variables store the ID of the first LED in each line.
// A starts at 0 (and goes up to 29)
// Therefore B starts at 30 (and goes up to 57) etc etc
#define START_LEDS_A    0
#define START_LEDS_B    30
#define START_LEDS_C    58
#define START_LEDS_D    70

// This creates an array of the LEDs
CRGB leds[NUM_LEDS];

// Needed to run the display
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
Adafruit_7segment matrix = Adafruit_7segment();


#define BUTTON_PIN D0
int buttonState = 0;  // Holds the readout from the button  
int counter = 3;      // Holds the number as we cycle through them on the display
bool offed = true;    // To check if the button has been released after pushing 
                        // (otherwise it will get pushed tens of times a second) 

// // //

// These variables are for the animation
// During the animation, we move down each line, lighting up each LED in turn
// (We then also slightly darken every LED in the line, so that only LEDs that have been lit up recently look bright)
// These variables hold ID of the LED we're lighting up this turn, 
// here we set them all to start with the first ID in that line.

int numberOfModes = 6;

// Pink and blue 
float lightA = START_LEDS_A;
float lightB = START_LEDS_B;
float lightC = START_LEDS_C;
float lightD = START_LEDS_D;

// Pulse
float brightnessA = 90;
float brightnessB = 60;
float brightnessC = 30;
float brightnessD = 0;
bool directionA = true;
bool directionB = true;
bool directionC = true;
bool directionD = true;
float pulseHue = 203;
bool hueDirection = true;
float pulseSpeed = 1;

// Swipe
float lineALeftBottom = 14;
float lineALeftTop = 0;
float lineARightBottom = 15;
float lineARightTop = 29;
float lightALeft = lineALeftBottom;
float lightARight = lineARightBottom;

float lineBLeftBottom = 43;
float lineBLeftTop = 30;
float lineBRightBottom = 44;
float lineBRightTop = 57;
float lightBLeft = lineBLeftBottom;
float lightBRight = lineBRightBottom;

float lineCLeftBottom = 62;
float lineCLeftTop = 58;
float lineCRightBottom = 63;
float lineCRightTop = 67;
float lineCMiddleBottom = 69;
float lineCMiddleTop = 68;
float lightCLeft = lineCLeftBottom;
float lightCRight = lineCRightBottom;
float lightCMiddle = lineCMiddleBottom;

float lineDLeftBottom = 70;
float lineDLeftTop = 79;
float lineDRightBottom = 90;
float lineDRightTop = 99;
float lineDMiddleBottom = 89;
float lineDMiddleTop = 80;
float lightDLeft = lineDLeftBottom;
float lightDRight = lineDRightBottom;
float lightDMiddle = lineDMiddleBottom;


void setup() {

  // FAST LED STUFF
  // Tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_PIN_A,COLOR_ORDER>(leds, START_LEDS_A, NUM_LEDS_A).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_PIN_B,COLOR_ORDER>(leds, START_LEDS_B, NUM_LEDS_B).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_PIN_C,COLOR_ORDER>(leds, START_LEDS_C, NUM_LEDS_C).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_PIN_D,COLOR_ORDER>(leds, START_LEDS_D, NUM_LEDS_D).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // Display stuff
  #ifndef __AVR_ATtiny85__
    Serial.begin(9600);
  #endif
  matrix.begin(0x70);

  // Button stuff
  pinMode(BUTTON_PIN, INPUT);

}

void loop() {

  // Call the animation function depending on which mode is selected
  if (counter == 0){
    pinkAndBlue();
  }

  else if (counter == 1){
    pulse(); // Pink
  }

  else if (counter == 2){
    pulse(); // Green
  }

  else if (counter == 3){
    pulse(); // Red
  }
  
  else if (counter == 4){
    swipeUp(); // Pink and Blue
  }

  else if (counter == 5){
    swipeUp(); // Green
  }  

  else if (counter == 6){
    swipeUp(); // Orange
  } 

  // Send the 'leds' array out to the actual LED strip and make it show up!
  FastLED.show(); 
  
  // Insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // Read the state of the button
  buttonState = digitalRead(BUTTON_PIN);

  // Checks if the button has been pushed/released, ups the counter by 1 when it is released.
  if (buttonState == HIGH) {
    //Serial.println("ON");
    if (offed){
      counter++;
    }
    offed = false; 
  } else {
    //Serial.println("OFF");
    offed = true;
    matrix.writeDisplay();
  }

  matrix.writeDigitNum(4, counter, false);

  // Loop back to the beginning
  if (counter > numberOfModes){
    counter = 0;
  }
  
}



void pulse(){
  if (hueDirection){
    pulseHue += (pulseSpeed/4);
  }
  else{
    pulseHue -= (pulseSpeed/4);
  }

  
  if (directionA){
    brightnessA += pulseSpeed;
  }
  else{
    brightnessA -= pulseSpeed;
  }
 
  if (directionB){
    brightnessB += pulseSpeed;
  }
  else{
    brightnessB -= pulseSpeed;
  }
 
  if (directionC){
    brightnessC += pulseSpeed;
  }
  else{
    brightnessC -= pulseSpeed;
  }
 
  if (directionD){
    brightnessD += pulseSpeed;
  }
  else{
    brightnessD -= pulseSpeed;
  }

  if (counter == 1){          // pink
    if (pulseHue >= 247){ hueDirection = false;}  
    if (pulseHue <= 200) {hueDirection = true;}
  }
  else if (counter == 2){      // green
    if (pulseHue >= 128){ hueDirection = false;}  
    if (pulseHue <= 64) {hueDirection = true;}
  }
  else if (counter == 3){      // red
    if (pulseHue >= 0){ hueDirection = false;}  
    if (pulseHue <= 32) {hueDirection = true;}
  }

  if (brightnessA >= 200){ directionA = false;}
  if (brightnessA <= 60) {directionA = true;}

  if (brightnessB >= 200){ directionB = false;}
  if (brightnessB <= 60) {directionB = true;}
  
  if (brightnessC >= 200){ directionC = false;}
  if (brightnessC <= 60) {directionC = true;}
  
  if (brightnessD >= 200){ directionD = false;}
  if (brightnessD <= 60) {directionD = true;}

  for (int i = START_LEDS_A; i<START_LEDS_B; i++){
    leds[i] = CHSV( pulseHue, 200, brightnessA);
  }
  for (int i = START_LEDS_B; i<START_LEDS_C; i++){
    leds[i] = CHSV(  pulseHue, 200, brightnessB);
  }
  for (int i = START_LEDS_C; i<START_LEDS_D; i++){
    leds[i] = CHSV(  pulseHue, 200, brightnessC);
  } 
  for (int i = START_LEDS_D; i<NUM_LEDS; i++){
    leds[i] = CHSV(  pulseHue, 200, brightnessD);
  } 

}

void swipeUp(){
   
  // In every frame, we increment the variables, 
  // so that the LED we're lighting up will move along the arrow
  // Note that some numbers are incrementing and some are decrementing, 
  // because of the orientation of the LED strip.
  // This makes the swipey fade move upwards
  lightALeft -= 0.20;
  lightARight += 0.20;

  lightBLeft -= 0.20;
  lightBRight += 0.20;
  
  lightCLeft -= 0.10;
  lightCRight += 0.10;
  lightCMiddle -= 0.10;

  lightDLeft += 0.20;
  lightDRight += 0.20;
  lightDMiddle -= 0.20;
    
  // If we've reached the end of a line, go back to the beginning
  if (lightALeft < lineALeftTop){  
    lightALeft = lineALeftBottom;
    lightARight = lineARightBottom;
    // Doing these together keeps it symmetrical, otherwise it somehow seems to get slightly out of sync
  }

  if (lightBLeft < lineBLeftTop){  
    lightBLeft = lineBLeftBottom;
    lightBRight = lineBRightBottom;
    // Doing these together keeps it symmetrical, otherwise it somehow seems to get slightly out of sync
  }  
  
  if (lightCLeft < lineCLeftTop - 6){   // Send the animation off the end of the line to slow it down a bit
    lightCLeft = lineCLeftBottom;
    lightCRight = lineCRightBottom;
    lightCMiddle = lineCMiddleBottom;
    // Doing these together keeps it symmetrical, otherwise it somehow seems to get slightly out of sync
  } 

  if (lightDLeft > lineDLeftTop){  
    lightDLeft = lineDLeftBottom;
    lightDRight = lineDRightBottom;
    lightDMiddle = lineDMiddleBottom;
    // Doing these together keeps it symmetrical, otherwise it somehow seems to get slightly out of sync
  }  

  // Fade the top section slower than the rest so it's lit for longer
  for(int i= 0; i<NUM_LEDS; i++){
    if (i>58 && i<70){
      leds[i].fadeToBlackBy( 15 );     
    }
    else{
      leds[i].fadeToBlackBy( 30 );
    }
  }

  
  if (counter == 4){ // Pink and Blue
    leds[(int)lightALeft] += CHSV( 240, 151, 125);
    leds[(int)lightARight] += CHSV( 240, 151, 125);  
  
    leds[(int)lightBLeft] += CHSV( 190, 151, 125);
    leds[(int)lightBRight] += CHSV( 190, 151, 125);  
  
    if (lightCLeft >= 58){ leds[(int)lightCLeft] += CHSV( 150, 151, 125); }
    if (lightCRight < 68){ leds[(int)lightCRight] += CHSV( 150, 151, 125); }
    if (lightCMiddle >= 68){ leds[(int)lightCMiddle] += CHSV( 150, 151, 125);}
  
    leds[(int)lightDLeft] += CHSV( 170, 200, 125);
    leds[(int)lightDRight] += CHSV( 170, 200, 125); 
    leds[(int)lightDMiddle] += CHSV( 170, 200, 125); 
  }


  if (counter == 5){ // Green
    leds[(int)lightALeft] += CHSV( 64, 200, 100);
    leds[(int)lightARight] += CHSV( 64, 200, 100);  
  
    leds[(int)lightBLeft] += CHSV( 80, 180, 150);
    leds[(int)lightBRight] += CHSV( 80, 180, 150);  
  
    if (lightCLeft >= 58){ leds[(int)lightCLeft] += CHSV( 120, 150, 125); }
    if (lightCRight < 68){ leds[(int)lightCRight] += CHSV( 120, 150, 125); }
    if (lightCMiddle >= 68){ leds[(int)lightCMiddle] += CHSV( 120, 150, 125);}
  
    leds[(int)lightDLeft] += CHSV( 129, 180, 105);
    leds[(int)lightDRight] += CHSV( 129, 180, 105); 
    leds[(int)lightDMiddle] += CHSV( 129, 180, 105); 
  }


   if (counter == 6){ // Orange
    leds[(int)lightALeft] += CHSV( 30, 200, 100);
    leds[(int)lightARight] += CHSV( 30, 200, 100);  
  
    leds[(int)lightBLeft] += CHSV( 36, 200, 100);
    leds[(int)lightBRight] += CHSV( 36, 200, 100);  
  
    if (lightCLeft >= 58){ leds[(int)lightCLeft] += CHSV( 40, 200, 125); }
    if (lightCRight < 68){ leds[(int)lightCRight] += CHSV( 40, 200, 125); }
    if (lightCMiddle >= 68){ leds[(int)lightCMiddle] += CHSV( 40, 200, 125);}
  
    leds[(int)lightDLeft] += CHSV( 32, 200, 100);
    leds[(int)lightDRight] += CHSV( 32, 200, 100); 
    leds[(int)lightDMiddle] += CHSV( 32, 200, 100); 
  }
 
}



void pinkAndBlue(){

  // In every frame, we increment the variables, 
  // so that the LED we're lighting up will move down the line
  lightA += 0.20;
  lightB += 0.20;
  lightC += 0.20;
  lightD += 0.20;

  // If we've reached the end of a line, go back to the beginning
  if (lightA > START_LEDS_B){  
    lightA = START_LEDS_A;
  }
  
  if (lightB >= START_LEDS_C){ 
    lightB = START_LEDS_B;
  }
  
  if (lightC >= START_LEDS_D){ 
    lightC = START_LEDS_C;
  }
  
  if (lightD >= NUM_LEDS){ 
    lightD = START_LEDS_D;
  }

  // Go through ALL the leds, and fade them towards black/dark a bit
  fadeToBlackBy( leds, NUM_LEDS, 20);

  // Light up the current chosen LED in each line, each line does a slightly different colour for fun
  // These colours are in hue, saturation, brightness format (each out of 255) 
  // E.g. in the first one, 247 is the hue, 151 is the saturation, 125 is brightness.
  leds[(int)lightA] += CHSV( 247, 151, 125);
  leds[(int)lightB] += CHSV( 203, 200, 125);
  leds[(int)lightC] += CHSV( 112, 130, 125); 
  leds[(int)lightD] += CHSV( 235, 190, 255);
    
}
