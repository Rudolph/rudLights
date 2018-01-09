/*
 * Stuff to actually display messages to the alphabet board goes in here
 */

void runDisplay(){

  // if we're pausing between repeating characters, like the "r" in "Merry"
  if(waitForIt){
    currentMillis = previousMillis = millis();
    waitForIt = false;
    whichChar++;
  }else{
    currentMillis = millis();
  }

  // display the current character
  if(isAlpha(theLine[whichChar]) or isSpace(theLine[whichChar])){
    showLetter(theLine[whichChar]);

    // for spaces between words make the display time a wee bit shorter than for
    // isAlpha() characters 
    if(isSpace(theLine[whichChar])){
      interval = 1000UL;
    }else{
      interval = 1500UL;
    }
    
    // check the time;
    if(currentMillis - previousMillis > interval){
      // if it's been long enough, clear the display
      clearDisplay();

      // if the next char is a repeat pause the blank display for a half sec
      if(theLine[whichChar] == theLine[whichChar+1]){
        theCommand = 6;
        nextCommand = 5;
        interval = 500UL;
        waitForIt = true;
      }else{
        whichChar++;
      }
      
      // and reset the counter
      previousMillis = currentMillis;
    }
  }else{
    if((whichChar == 0) && (whichLine >= 1) && (whichLine <=8)){
      // we've encountered an undisplayable line, probably from a not-set EEPROM slot
      // decrease whichLine and try again

      if(whichLine > 2){
        whichLine -= 2;
      }else{
        whichLine--;
      }

      // if it's all the way down to 0 and we have nothing, i.e. none of the slots have
      // a p/s/w in them, re-random by calling command zero again. This prevents runRunRUN()
      // from, well, running so often when the slots are empty
      if(whichLine == 0){
        previousMillis = millis();
        interval = 1500UL;
        theCommand = 0;
      }
      
    }else{
      // we've displayed the last character
      // set whichChar back to 0
      whichChar = 0;

      // clear out theLine
      memset(theLine, '\0', numChars);

      // and kill a few to make it clear we're between sentences
      previousMillis = millis();
      interval = 5000UL;
      theCommand = 6;
      nextCommand = 0;
    }
  }
}

void showLetter(char inLetter){
  // if it's a space (character 32)
  if(inLetter == " "){
    clearDisplay();
  }else{
    // it's a regular letter, subtract 65 to get the location in letterColors array
    int theLetter = inLetter-65;
    if(DEBUG) DBGSerial.print(theLetter);
    strip.setPixelColor(theLetter, letterColors[theLetter][0],letterColors[theLetter][1],letterColors[theLetter][2]);
    strip.show();
  }
}

void clearDisplay(){
  if(DEBUG) DBGSerial.println("");
  for (int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i, 0,0,0);
  }
  strip.show();
}

