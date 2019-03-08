/**
 * LEDFaceMask.ino
 * 
 * This code is what controls the LED Mask
 * created by me and as seen in this video:
 * https://www.youtube.com/watch?v=atzkV1r2o2E&lc=z23ax3jpermvcnqzh04t1aokg5czpio1x0kpnjifeceark0h00410
 * 
 * Many of the light designs were pulled from the 
 * FastLED Examples Library, and the functionality of
 * the 1Sheeld board can also easily be learned by 
 * looking through their tutorials as well.
 */

#include <FastLED.h>

// These need to come before the #include for the OneSheeld
// So it knows just how much of their library needs to be brought in;
// Takes up too much memory otherwise
#define CUSTOM_SETTINGS
#define INCLUDE_KEYBOARD_SHIELD // For keyboard commands via bluetooth
#define INCLUDE_TERMINAL_SHIELD // For debugging
#include <OneSheeld.h>


// Change this value to whichever pin you're using for data
#define DATA_PIN    5
// Change LED type and color order to whatever your LEDs use
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
// Change to however many LEDs you have
#define NUM_LEDS    48
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          50 // Was 96
#define FRAMES_PER_SECOND  80 // Was 120

// Creating member variables
int curColor;
int curColumn;
int hue;
int hueAmount;
int currentPattern;

// Setting up arrays for special designs
const int columns[11][6] = { {0, 21, 22, -1, -1, -1}, {1, 20, 23, -1, -1, -1}, {2, 19, 24, 39, -1, -1}, {3, 18, 25, 38, 40, -1}, {4, 17, 26, 37, 41, 47}, {5, 16, 27, 36, 42, 46}, {6, 15, 28, 35, 43, 45}, {7, 14, 29, 34, 44, -1}, {8, 13, 30, 33, -1, -1}, {9, 12, 31, -1, -1, -1}, {10, 11, 32, -1, -1, -1} };
const int diag[13][6] = { {0, -1, -1, -1, -1, -1}, {1, 21, -1, -1, -1, -1}, {2, 20, 22, -1, -1, -1}, {3, 19, 23, -1, -1, -1}, {4, 18, 24, -1, -1, -1}, {5, 17, 25, 39, -1, -1}, {6, 16, 26, 38, -1, -1}, {7, 15, 27, 37, 40, -1}, {8, 14, 28, 36, 41, -1}, {9, 13, 29, 35, 42, 47}, {10, 12, 30, 34, 43, 46}, {11, 31, 33, 44, 45, -1}, {32}};
const int hiArray[] = {3, 5, 7, 14, 16, 25, 27, 28, 29, 34, 36, 38, 40, 42, 44};
const int checkArray[] = {18, 26, 34, 36, 43};
const int xArray[] = {3, 7, 15, 17, 27, 35, 37, 40, 44};
const int mustArray[] = {13, 16, 19, 25, 26, 28, 29}; // {13, 16, 19, 25, 26, 28, 29}
const int circles[8][12] = {{27}, {16, 26, 28, 36}, {5, 15, 17, 25, 29, 35, 37, 42}, {4, 6, 14, 18, 24, 30, 34, 38, 41, 43, 46}, {3, 7, 13, 19, 23, 31, 33, 39, 40, 44, 45, 47}, {2, 8, 12, 20, 22, 32}, {1, 9, 11, 21}, {0, 10}};

void setup() {
  delay(3000); // Pop a recovery - safe delay for light bootup
  // Setting up variable values
  OneSheeld.begin();
//  Serial.begin(9600); // No can do, Serial is used by 1Sheeld to communicate with phone, can't use Serial Monitor
  currentPattern = 1;
  curColumn = 0;
  curColor = 0;
  hue = 0;
  hueAmount = 1;
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear(); // Clears all lights
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.show(); // Displays current light pattern
  AsciiKeyboard.setOnButtonChange(&keyboardFunction);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList patterns = {turnOff, confetti, candyCane, solidWhite, sayHi, rainbow, checkmark, bigX, collegeColors, mustache, rainbowCircles, scanning};

void loop() {

  // Calls upon the currently selected pattern to update
  patterns[currentPattern](); 
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  EVERY_N_MILLISECONDS( 20 ) { hue += hueAmount; } // slowly cycle the "base color" through the rainbow

}

// Every time a key is pressed on my phone,
// this function is called
void keyboardFunction(char data) {
  if(data == 'Q') {
    changePattern(1); // Confetti
  } else if(data == 'W') {
    changePattern(2); // Candy Cane
  } else if(data == 'E') {
    changePattern(3); // Solid White
  } else if(data == 'R') {
    changePattern(4); // Say Hi
  } else if(data == 'T') {
    changePattern(5, 3); // Rainbow
  } else if(data == 'Y') {
    changePattern(6); // Checkmark  
  } else if(data == 'U') {
    changePattern(7); // Big X  
  } else if(data == 'I') {
    changePattern(8); // College Colors
  } else if(data == 'O') {
    changePattern(9); // Mustache
  } else if(data == 'P') {
    changePattern(10, 5); // Rainbow Circles
  } else if(data == 'A') {
    changePattern(11); // Scanning
  } else { 
    changePattern(0); // Lights Off (Machine is still running though)
  }
}

/**
 * Simple changePattern method simply for
 * changing the pattern with a default
 * hueAmount of 1
 */
void changePattern(int newPattern) {
  // Defaults to changing hue by 1
  changePattern(newPattern, 1);
}

/**
 * Special overloaded changePattern for changing
 * the currentPattern and the hueAmount
 */
void changePattern(int newPattern, int hueDelta) {
  // Only change anything if the selected
  // pattern is not already running
  if(currentPattern != newPattern) {
    FastLED.clear(); // Resets all lights
    hue = 0; // Resets hue
    hueAmount = hueDelta;
    currentPattern = newPattern; // Changes to new pattern
  }
}

void confetti() {
  // Tells the LEDs to fade after a bit
  fadeToBlackBy(leds, NUM_LEDS, 10);
  // Randomly picks an led (random16() goes from 0 to 2^16, 
  // unless given a specification to go to, NUM_LEDS in this case)
  int pos = random16(NUM_LEDS);
  // the random LED is assigned a value with a normal brightness and value 
  // with a psuedo-random hue, based on the current global hue
  leds[pos] += CHSV( hue + random8(64), 200, 255);
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, hue, 7);
}

void candyCane() {
  // This code means that the lights change
  // every second, alternating
  uint8_t secondHand = (millis() / 1000) % 60;
  // If the second hand is an even number, firstColor becomes Gray. If odd, it becomes Red.
  CRGB firstColor = (secondHand % 2 == 0) ? CRGB::Gray : CRGB::Red;
  // secondColor alternates opposite to firstColor
  CRGB secondColor = (secondHand % 2 == 0) ? CRGB::Red : CRGB::Gray;
  // The values of first and secondColor are put in this array to make the lines double thick
  const CRGB colors[4] = {firstColor, firstColor, secondColor, secondColor};
  int diagOn = 0;

  // Iterates through all the lights and sets the colors accordingly
  for(int i = 0; i < 13; i++) {
    for(int k = 0; k < 6; k++) {
      if(diag[i][k] == -1) { break; }
      leds[diag[i][k]] = colors[diagOn % 4];
    }
    diagOn++;
  }
  
}

void solidWhite() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Gray;
  }
}

void sayHi() {
  // Goes through every index listed in the hiArray
  // and turns that led on
  for(int i = 0; i < ARRAY_SIZE(hiArray); i++) {
    leds[hiArray[i]] = CHSV(hue, 200, 255);
  }
}

void checkmark() {
  // Goes through every index listed in the checkArray
  // and turns that led on
  for(int i = 0; i < ARRAY_SIZE(checkArray); i++) {
    leds[checkArray[i]] = CRGB::Green;
  }
}

void bigX() {
  // Goes through every index listed in the xArray
  // and turns that led on
  for(int i = 0; i < ARRAY_SIZE(xArray); i++) {
    leds[xArray[i]] = CRGB::Red;
  }
}

/**
 * Displays an alternating pattern of the 
 * Austin College school colors
 */
void collegeColors() {
  CRGB firstColor;
  CRGB secondColor;
  int secondHand = (millis() / 1000) % 60;
  if(secondHand % 2 == 0) {
    firstColor = CRGB::White;
    secondColor = CRGB::DarkOrange;
  } else {
    firstColor = CRGB::Red;
    secondColor = CRGB::White;
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    if(i <= 10 || i >= 40) {
      if(secondHand % 2 == 0) {
        leds[i] = CRGB::DarkOrange;
      } else {
        leds[i] = CRGB::Red;
      }
    } else {
      if(i % 2 == 0) {
        leds[i] = firstColor;
      } else {
        leds[i] = secondColor;
      }
    }
  }
}

void mustache() {
  // Goes through every index in the mustArray
  // and turns that led on using a changing gradient
  // of color
  for(int i = 0; i < ARRAY_SIZE(mustArray); i++) {
    leds[mustArray[i]] = CHSV(hue, 200, 255);
  }
}

/**
 * Favorite design yet; the circles array is a two-dimensional array
 * of indeces going from the very center led to expanding circles reaching
 * to the ends of the mask. This method lights up each of those circles as
 * slightly different colors and pushes them through a gradient of color
 */
void rainbowCircles() {
  // Nested for-loops because we're dealing with a two-dimensional array
  for(int i = 0; i < ARRAY_SIZE(circles); i++) {
    for(int k = 0; k < ARRAY_SIZE(circles[i]); k++) {
//      if(circles[i][k] == null) { break; }
      // To make each circle a slightly different color,
      // we multiply the hue by an offset of i, the counter
      // keeping track of which circle we are on
      // We multiply by 30 to make that difference more noticeable
      leds[circles[i][k]] = CHSV(hue + (i*30), 200, 255);
    }
  }
}

/**
 * Creates a scanning motion of lights in vertical lines,
 * sweeping back and forth and changing color
 */
void scanning() {
  fadeToBlackBy(leds, NUM_LEDS, 50);
  // See FastLED Documentation for more info on beatsin8() method
  // Creates a sinusoidal function that oscillates between
  // the values 0 and 10 (inclusive to both) at a rate of 35 bpm
  uint8_t beat = beatsin8(35, 0, 10);

  // Loops through the specified column given to us by beat, 
  // which is given value from beatsin8()
  for(int i = 0; i < ARRAY_SIZE(columns[beat]); i++) {
    // Because some columns have more LEDs than others, I made them all
    // the same length but made empty spots hold a -1, to know when to break
    // from that column
    if(columns[beat][i] == -1) { break; }
    leds[columns[beat][i]] = CHSV(hue, 200, 255);
  }
  
}

void turnOff() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}
