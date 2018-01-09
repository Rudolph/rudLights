/*
 * Serial stuff is handled in here
 * 
 * Yes, there's probably a much cleaner, smarter way of doing this, but this is working,
 *  and I need to move forward on this project
 */

String tmpString;
int tmpCommand;
int tmpSlot;

/*
 * The main routine that takes serial input from the app via bluetooth
 * Reads in a "packet" wrapped in startMarker and endMarker, and grabs the command and 
 *  optional command argument (not optional for command 3, 4, 9)
 */
void doSerial(){
  char startMarker = '<';
  char theDelimiter = ',';
  char endMarker = '>';
  char rc;

  // if we're not currently already working on new data going to receivedChars
  while (Serial.available() > 0 && newData == false) {

    rc = Serial.read();
    if (rc == startMarker) {
      // get the command int, first thing after the start marker
      tmpString = Serial.readStringUntil(theDelimiter);
      tmpCommand = tmpString.toInt();

      // if it's command number 3 or 4 get the slot number to read or write
      // if calling command 3 and slot is 9 then it just wants the list o' lines
      // if calling command 7, slot is just > 0 && <= 9 to signify the "sleep" 
      //   button was pressed in the app
      // if calling command 9, slot is which of the "special" routines to run
      if((tmpCommand == 3) or (tmpCommand == 4) or (tmpCommand == 7) or (tmpCommand == 9)){
        tmpString = Serial.readStringUntil(theDelimiter);
        tmpSlot = tmpString.toInt();
      }

      // if it's 5 or 6 it's not allowed to be called by the app
      if((tmpCommand == 5) or (tmpCommand == 6)){
        tmpCommand = 0;
      }

      // now the rest of the input should be the new sentence/phrase/whatsit
      tmpString = Serial.readStringUntil(endMarker);

      newData = true;
      cleanData();
    }
  }
}

/*
 * Clean up the newly acquired p/s/w from Serial
 * theCommand and theSlot are already set in doSerial() but the phrase/sentence/whatsit is only
 *  sitting as an unsanitized string. Here we'll uppercase it, remove useless characters, and
 *  put it into the receivedChars array
 */
void cleanData(){
  if(newData){
    // some temporary holding vars
    int tmpNumChars = tmpString.length();
    char tmpChars[numChars];
    int tmpCharsIndex = 0;

    // put the string to uppercase and break it into an array of chars
    tmpString.toUpperCase();
    tmpString.toCharArray(tmpChars, numChars);

    // now plop the usable chars into the receivedChars array, stopping at 100 chars (0-99) so it
    //  leaves space for the null at the end
    for (int i = 0; i < tmpNumChars; i++){
      if(isAlpha(tmpChars[i]) or isSpace(tmpChars[i])){
        receivedChars[tmpCharsIndex] = tmpChars[i];
        tmpCharsIndex++;
      }
    }

    // append the null terminator
    receivedChars[tmpCharsIndex] = '\0';
    
    // and clean up some things
    memset(tmpChars, '\0', sizeof(tmpChars));
    tmpCharsIndex = 0;
    tmpString = "";
    theCommand = tmpCommand;
    theSlot = tmpSlot;

    // if it's command 0, 2, 3, 4, 9 we want to interrupt a currently-running p/s/w
    // though I'm beginning to wonder, shouldn't _any_ call from the app interrupt?
    if((theCommand == 0) or (theCommand == 2) or (theCommand == 3) or (theCommand == 4) or (theCommand == 7) or (theCommand == 9)){
      whichChar = 0;
      interval = 1500UL;
      previousMillis = currentMillis = millis();
    }
    newData = false;
  }
}

