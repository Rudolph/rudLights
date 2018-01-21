# rudLights
The rudLights Alphabet Board

See https://rudforce.com/rudLights/ for more information or visit the [page at Instructables](https://www.instructables.com/id/App-controlled-Alphabet-Board-Inspired-by-Stranger/) to see how to build one.

## rudLightsPi
The folder rudLightsPi contains, as you may surmise, a rudimentary Bluetooth to serial "forwarder" so a Raspberry Pi Zero W can be used in place of the HC-05 Bluetooth module. So the __W__ part is important! The script is in Python 3, and it's my first dabble in Python in several years so it might (will definitely) be crunchy. It also assumes you're running the latest version of Raspbian as of this writing, Stretch I believe it is. I can't promise it will work on Jessie or older. Nor can I promise it will work on a Raspberry Pi 3, let alone any other model without built-in Bluetooth. In fact, I wouldn't use it at all if I was you. At least not at this point.

*It does not replace the Arduino part of this project, just the HC-05 Bluetooth module! The Arduino is still required, and it still needs to be running the rudLightsArduino sketch. I may one day try to make the WS2812B LEDs run directly from the Pi, but not this day.*

See the README in that folder for how to install and use, if you dare.
