/*
 * rudLightsArduino
 * Copyright © 2017 Del Rudolph <me@darthrudolph.com>
 * Released under the GNU General Public License version 3
 *
 * Receives text from Android app and displays it on a string
 * of Christmas lights. Yes, inspired by Stranger Things, as well
 * as the countless other Stranger Things inspired projects out
 * there. This one was created as a Christmas gift for my niece.
 *
 */

bool DEBUG = false;

#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include "daVars.h"

/*
 * Set up the soft serial for sending debug info back to the computer
 */
SoftwareSerial DBGSerial(10, 11);   // rxPin, txPin

/*
 * Set up the WS2812B light strip on digital pin 6
 * LED_COUNT should stay at 26, as that's how many letters are in the alphabet ;-)
 * LED_PIN can be any Arduino digital pin, it doesn't have to be 6. Change it here if
 * you used a different one.
 */
#define LED_COUNT 26
#define LED_PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);      // set up serial to the HC-05
  randomSeed(analogRead(0));

  strip.begin();
  clearDisplay();
  strip.show();
  
  if (DEBUG) {
    DBGSerial.begin(9600);  // set up soft serial back to PC
    DBGSerial.println("DEBUG is true");
  }

  // uncomment this and run the board to nuke existing EEPROM data
  // !!! MAKE SURE TO RE-COMMENT AND UPLOAD OR EEPROM WILL ERASE AT EVERY BOOT!!!
//  for (int i = 0 ; i < EEPROM.length() ; i++) {
//    EEPROM.write(i, 0);
//    Serial.print("EEPROM Erased ");
//    Serial.println(i);
//  }

  // startup animation
  // pixel color 0,128,0 is green. You can change this to any color you want using
  // RGB colors. See https://www.w3schools.com/colors/colors_picker.asp for more color choices
  for(int i=0; i<LED_COUNT+2; i++){
    strip.setPixelColor(i, 0, 128, 0);
    if(i > 1){
      strip.setPixelColor(i-1, 0, 42, 0);
      strip.setPixelColor(i-2, 0, 14, 0);
      strip.setPixelColor(i-3, 0);
    }
    strip.show();
    delay(75);
  }

  clearDisplay();
  delay(3000);

  previousMillis = millis();
}

void loop() {
  // check for serial data from the app
  doSerial();

  // and figure out if there's a command
  switch(theCommand){
    case 1:
      // just a single letter pressed in the app
      if(isAlpha(receivedChars[0]) && (justOneChar == false)){
        // if we got a usable letter and this is the first loop
        clearDisplay();
        showLetter(receivedChars[0]);
        justOneChar = true;
        previousMillis = millis();
      }else if(justOneChar == true){
        // 12 second time out so we don't display one character forever
        currentMillis = millis();
        if(currentMillis - previousMillis > 12000){
          clearDisplay();
          justOneChar = false;
          previousMillis = millis();
          interval = 5000UL;
          theCommand = 6;
          nextCommand = 0;
        }
      }else{
        theCommand = 0;
      }
      break;

    case 2:
      // one p/s/w manually entered in the app and the go button pressed
      // just hand off to the normal display command, 5, but with a special
      // line so it displays receivedChars instead of a saved line
      clearDisplay();
      whichChar = 0;
      whichLine = 99;
      theCommand = 5;
      break;

    case 3:
      // app has requested either the list of what's stored in EEPROM, or
      // a specific slot of EEPROM to be displayed
      if((theSlot >= 1) and (theSlot <= 8)){
        // if slot is 1-8 the app wants that specific p/s/w
        // just hand off to the normal display command, 5
        clearDisplay();
        whichChar = 0;
        whichLine = theSlot;
        theCommand = 5;
        break;
      }else if(theSlot == 9){
        // If slot is 9 the app is requesting the list of p/s/ws saved in EEPROM
        // Loop through and spit them out to Serial BT connection, with a comma
        // in between so the app can parse as csv row
        for(int i = 0; i <= 707; i+=101){
          char tmpLine[numChars];
          EEPROM.get(i, tmpLine);
          if(isAlpha(tmpLine[0]) or isSpace(tmpLine[0])){
            if(DEBUG) DBGSerial.println(tmpLine);
            Serial.print(tmpLine);
          }else{
            // If the slot is empty, say so, the app understands "Empty" for slot data
            if(DEBUG) DBGSerial.println("Unreadable");
            Serial.print("Empty");
          }
          if(i < 707){
            // Don't put a comma after the last slot
            Serial.print(",");
          }
        }
        Serial.println();
      }
      // back to regularly scheduled programming
      theCommand = 5;
      break;

    case 4:
      // app wants to save a p/s/w to a certain EEPROM slot
      if(theSlot == 1){
        EEPROM.put(0, receivedChars);
      }else if((theSlot >= 2) && (theSlot <=8)){
        int tmpSlot = theSlot-1;
        int tmpSlot2 = tmpSlot*101;
        EEPROM.put(tmpSlot2, receivedChars);
      }

      // saved to EEPROM, now display the new p/s/w
      clearDisplay();
      whichChar = 0;
      whichLine = 99;
      theCommand = 5;
      break;

    case 5:
      // continue display of an in-progress p/s/w
      if(whichLine == 0){
        // if the line is zero just go to the runRunRUN() function
        runRunRUN();
      }else{
        // if we're at character zero we need to grab the line into theLine
        // before going to runDisplay()
        if(whichChar == 0){
          if((whichLine >= 1) && (whichLine <= 8)){
            // if line is 1-8 it's an EEPROM line
            // because they're actually numbered 0-7 we subtract 1
            if(whichLine == 1){
              EEPROM.get(0, theLine);
            }else{
              int tmpLine = whichLine-1;
              int tmpLine2 = tmpLine*101;
              EEPROM.get(tmpLine2, theLine);
            }
            
          }else if((whichLine >=9) && (whichLine <= string_count+8)){
            // it's one of the hardcoded lines, but since they too are actually numbered
            // starting at 0 we subtract the 8 EEPROM lines and an extra 1
            // see https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
            strcpy_P(theLine, (char*)pgm_read_word(&(string_table[whichLine-9])));
            
          }else if(whichLine == 99){
            // whichLine is special here because it's coming from one of the other commands
            strcpy(theLine, receivedChars);
            
          }else{
            // should never get here
            // if we do, go back to command 0 to get a new whichLine
            theCommand = 0;
            break;
          }
        }

        // theLine is populated, go to runDisplay()
        runDisplay();
      }
      break;

    case 6:
      // a command to waste time
      if(millis() - previousMillis > interval){
        // time's up, set the counter back to normal
        interval = 1500UL;
        previousMillis = millis();
        
        // and go on with nextCommand
        theCommand = nextCommand;
      }
      break;

    case 7:
      // "sleep" mode - sets the lights to all off until the button is pressed 
      // again in the app
      static boolean sleepMode = false;
 
      if(theSlot > 0){
        theSlot = 0;
        sleepMode = !sleepMode;
      }
      
      if(sleepMode == true){
        // keep the lights off
        clearDisplay();
      }else{
        // back to business
        theCommand = 0;
        whichLine = 0;
        whichChar = 0;
      }

      break;
      
    case 9:
      // "Special" button commands from the app
      // 9,1 = hand off runRunRUN() to command 5
      if(theSlot == 1){
        theCommand = 5;
        whichLine = 0;
        whichChar = 0;
      }

      // 9,2 shows rainbow of colors on all lights
      if(theSlot == 2){
        rainbow();
      }

      // 9,3 shows a rainbow crawling along the "strand" of lights
      if(theSlot == 3){
        rainbowCycle();
      }

      // 9,4 crawling flashing lights with rainbow effect
      if(theSlot == 4){
        theaterChaseRainbow();
      }
      break;
      
    default:
      // if theCommand == 0, or if no command is set, we're effectively in random mode
      
      // Maximum for random() but with an extra one added, since upper bound is exclusive. 
      // Seriously. If you ask for a random number from one to twenty (random(1,20)) you'll
      // actually get a number from 1 to 19... It wouldn't even make sense if it was asking
      // for a number _between_ one and twenty, because then one wouldn't be eligible either.
      // So, string_count plus 10 (8 "slots" + 1 for "special" commands + the 1 extra)
      int randMax = string_count+10;

      // we actually want 0 as an option since it invokes the special runRunRUN() routine
      whichLine = random(0,randMax);

      // check for "special" command, of which there are four options. Since one is runRunRUN()
      // we'll skip it here (it already had its chance above) so rand(2,5) chooses from 2, 3,
      // and 4.
      if(whichLine == randMax-1){
        clearDisplay();
        theCommand = 9;
        whichLine = 0;
        whichChar = 0;
        theSlot = random(2,5);
        previousMillis = millis();
        break;
      }
      
      // now that we have a line go to command 5 to display it
      clearDisplay();
      whichChar = 0;
      theCommand = 5;
      previousMillis = millis();
      break;
  }
}
