/*
 * Special case display, lights up r... u... n... a couple times then
 * just turns on all three for a few seconds.
 */
void runRunRUN(){
  static boolean runPause = false;

  if(runPause){
    currentMillis = previousMillis = millis();
    runPause = false;
    whichChar++;
  }else{
    currentMillis = millis();
  }

  if((whichChar == 0) or (whichChar == 4)){
    clearDisplay();
    showLetter('R');
    whichChar++;
    previousMillis = millis();
    interval = 1500;
    theCommand = 6;
    nextCommand = 5;
  }else if((whichChar == 1) or (whichChar == 5)){
    clearDisplay();
    showLetter('U');
    whichChar++;
    previousMillis = millis();
    interval = 1500;
    theCommand = 6;
    nextCommand = 5;
  }else if((whichChar == 2) or (whichChar == 6)){
    clearDisplay();
    showLetter('N');
    whichChar++;
    previousMillis = millis();
    interval = 1500;
    theCommand = 6;
    nextCommand = 5;
  }else if((whichChar == 3) or (whichChar == 7)){
    clearDisplay();
    whichChar++;
    previousMillis = millis();
    interval = 1500;
    theCommand = 6;
    nextCommand = 5;
  }else if(whichChar == 8){
    clearDisplay();
    strip.setPixelColor(17, 255,0,0); // R-65=17
    strip.setPixelColor(20, 255,0,0); // U-65=20
    strip.setPixelColor(13, 255,0,0); // N-65=13
    strip.show();
    whichChar++;
    previousMillis = millis();
    interval = 3000;
    theCommand = 6;
    nextCommand = 5;
  }else{
    clearDisplay();
    whichChar = 0;
    previousMillis = millis();
    interval = 4000;
    theCommand = 6;
    nextCommand = 0;
  }
}
