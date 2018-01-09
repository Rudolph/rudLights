/*
 * Contents of this file are unabashedly borrowed from the excellent
 * Adafruit NeoPixel "strandtest" example and modified... slightly...
 * to not use delay()
 * See https://github.com/adafruit/Adafruit_NeoPixel (/examples/strandtest/strandtest.ino)
 */

void rainbow() {
  static uint16_t j = 0;

  if(millis() - previousMillis > 20){
    if(j <= 256*4){  // run for 4 iterations
      for(int i=0; i<strip.numPixels(); i++){
        strip.setPixelColor(i, Wheel((i+j) & 255));
      }
      strip.show();
      j++;
      previousMillis = millis();
    }else{
      j = 0;
      clearDisplay();
      previousMillis = millis();
      interval = 5000UL;
      theCommand = 6;
      nextCommand = 0;
    }
  }

// original Adafruit function contents:
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  static uint16_t j = 0;

  if(millis() - previousMillis > 20){
    if(j <= 256*5){  // 5 cycles of all colors on wheel
      for(int i=0; i<strip.numPixels(); i++){
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      j++;
      previousMillis = millis();
    }else{
      j = 0;
      clearDisplay();
      previousMillis = millis();
      interval = 5000UL;
      theCommand = 6;
      nextCommand = 0;
    }
  }
// original Adafruit function contents:
//  uint16_t i, j;
//
//  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//    for(i=0; i< strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow() {
  static uint16_t j, q = 0;

  if(millis() - previousMillis > 75){
    if(j < 256*2) {     // cycle all 256 colors in the wheel
      if(q < 3) {
        for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
        
        for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
        q++;
        previousMillis = millis();
      }else{
        q = 0;
      }
      j++;
    }else{
      j = 0;
      clearDisplay();
      previousMillis = millis();
      interval = 5000UL;
      theCommand = 6;
      nextCommand = 0;
    }
  }

// original Adafruit function contents:
//  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
//    for (int q=0; q < 3; q++) {
//      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
//      }
//      strip.show();
//
//      delay(wait);
//
//      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, 0);        //turn every third pixel off
//      }
//    }
//  }
}

// Unchanged from Adafruit original
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
