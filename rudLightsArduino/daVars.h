/*
 * The hard-coded strings the device is to display. Saved PROGMEM style to not use
 * up all the SRAM.
 * See https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
 * 
 * <blink>!!!For each string added here, be sure to add it to the reference table as well!!!</blink>
 *
 * These (and the EEPROM ones) can only be A-Z and space (" ")
 * Space is displayed as a pause between characters being lit up
 * No other characters can be displayed on the alphabet light device
 * Strings entered via the app and saved into EEPROM will be sanitized and uppercased at save time
 *
 * string_0 is the default message string! It is the first string displayed at power on
 */
const char string_0[] PROGMEM = "RUDLIGHTS ALPHABET BOARD";
const char string_1[] PROGMEM = "BE SURE TO DRINK YOUR OVALTINE";
const char string_2[] PROGMEM = "FRIENDS DONT LIE";
const char string_3[] PROGMEM = "HELLO FROM THE UPSIDE DOWN";


/*
 * And the ref table for the above strings
 */
const char* const string_table[] PROGMEM = {string_0, string_1, string_2, string_3};

/*
 * Aand the number of entries in the table above
 * Used when pulling a random string to display
 */
int string_count = 4;

/*
 * Aaand the buffer to use when calling the aforementioned strings into RAM
 * This is also the array for holding the phrase/sentence/whatsit that is currently being
 * displayed on the alphabet light device, so it will be used when pulling a p/s/w from
 * EEPROM as well
 * Each p/s/w is 100 characters (including spaces) so the array is 101 long to hold the 
 * null terminator
 */
const int numChars = 101;
char theLine[numChars];

/*
 * Now a char array to hold what comes in serial over bluetooth
 * Same length as theLine
 * newData is simply a flag that new data is available for putting into recievedChars
 */
char receivedChars[numChars];
boolean newData = false;

/*
 * And now for a wad of vars to hold other things
 * theCommand is, you guessed it, the command sent by the app. Defaults to 5 so we can display
 *  the default line
 * nextCommand only used by command 6, it's the command to run after 6's time is wasted
 * theSlot only works with command 4, and specifies the EEPROM slot in which to save the 
 *  p/s/w. It defaults to 9 which is an invalid slot, to avoid confusion. Hah.
 * whichLine is the p/s/w being displayed. Defaults to 9 which is actually string_0, because
 *  1-8 are the EEPROM slots, so 9 is actually a valid option and the first of the hardcoded lines.
 * whichChar is the character in the p/s/w to be displayed. Defaults to zero, the first char
 *  in the p/s/w.
 * currentMillis is a spot to hold some milliseconds
 * previousMillis, as you can imagine, is a spot to hold some milliseconds
 * waitForIt is just a flag that we need to pause a bit between repeating characters
 * justOneChar is just a flag when displaying a single character, used for timing out
 * interval is how long to display each character on the alphabet board
 */
int theCommand = 5;
int nextCommand;
int theSlot = 9;
int whichLine = 9;
int whichChar = 0;
unsigned long currentMillis;
unsigned long previousMillis;
boolean waitForIt = false;
boolean justOneChar = false;
unsigned long interval = 1500UL;

/*
 * And for something completely different, here's a multidimensional array for the character 
 * colors. For the sake of simplicity we'll only use 6 rainbow colors: red, orange, yellow,
 * green, blue, and purple. Yes, I know, it should be indigo and violet, but I wanted to
 * keep it simple, remember?
 */
byte letterColors[26][3] = {
  {255, 255, 0},  // A = yellow
  {0, 51, 204}, // B = blue
  {255, 26, 26},  // C = red
  {0, 204, 0},  // D = green
  {0, 51, 204}, // E = blue
  {255, 255, 0}, // F = yellow
  {255, 26, 26},  // G = red
  {51, 204, 204}, // H = teal
  {51, 204, 204}, // I = teal
  {255, 26, 26},  // J = red
  {0, 51, 204}, // K = blue
  {51, 204, 204}, // L = teal
  {255, 255, 0}, // M = yellow
  {255, 26, 26},  // N = red
  {255, 26, 26},  // O = red
  {0, 204, 0},  // P = green
  {255, 26, 26},  // Q = red
  {0, 204, 0},  // R = green
  {255, 255, 0}, // S = yellow
  {255, 255, 0}, // T = yellow
  {0, 51, 204}, // U = blue
  {255, 26, 26},  // V = red
  {0, 51, 204}, // W = blue
  {255, 255, 0}, // X = yellow
  {255, 26, 26},  // Y = red
  {255, 26, 26} // Z = red
};
