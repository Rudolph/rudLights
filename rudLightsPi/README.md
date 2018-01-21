# rudLightsPi
This folder rudLightsPi contains, as you may surmise, a rudimentary Bluetooth to serial "translator" so a Raspberry Pi Zero W can be used in place of the HC-05 Bluetooth module. So the __W__ part is important! The script is in Python 3, and it's my first dabble in Python in several years so it might (will definitely) be crunchy. It also assumes you're running the latest version of Raspbian as of this writing, Stretch I believe it is. I can't promise it will work on Jessie or older. Nor can I promise it will work on a Raspberry Pi 3, let alone any other model without built-in Bluetooth. In fact, I wouldn't use it at all if I was you. At least not at this point.

*This does not replace the Arduino part of this project, just the HC-05 Bluetooth module! The Arduino is still required, and it still needs to be running the rudLightsArduino sketch. I may one day try to make the WS2812B LEDs run directly from the Pi, but not this day.*

## Prerequisites
To use it you'll need to modify a few things on your Pi Zero W that require root access on the Pi. First off, make sure your Raspbian is up to date: `sudo apt-get update && sudo apt-get upgrade`

Then make sure python3-serial is installed (it should already be there): `sudo apt-get install python3-serial`

Then we need to grab [PyBluez](https://github.com/karulis/pybluez) and install it system-wide: `sudo pip3 install pybluez`

Alright, now the fun stuff. We need to edit the Bluez startup file: `sudo nano /etc/systemd/system/dbus-org.bluez.service`. You'll see a line that says `ExecStart=/usr/lib/bluetooth/bluetoothd`. Modify that line to read `ExecStart=/usr/lib/bluetooth/bluetoothd -C` instead. See, we added ` -C` to the end of it.

Now, right below that line add this new line:

`ExecStartPost=/usr/bin/sdptool add SP`

That's it for that file. Double-check for typos, then hit Ctrl-x and follow nano's prompts to save it.

## UART
Next up is the UART. The Pi 3 and the Zero W (and probably the others, but I don't know firsthand) actually have a serial console talking on the TXD/RXD lines (GPIO 14 and GPIO 15) which gets in the way of using it as a UART to talk to an Arduino. To disable this and allow us to use it, takes a few minor steps. First edit /config.txt with `sudo nano /config.txt`. Cursor down all the way to the bottom and add a line at the end that says `enable_uart=1`, all by itself on the last line of the file. Ctrl-x and follow nano's prompts to save it.

Now we need to tweak the file /boot/cmdline.txt: `sudo nano /boot/cmdline.txt` There is a long line in there, and it needs to remain all on one line. It should begin with something like "dwc\_otg.lpm\_enable=0" and it will contain a part that says "console=serial0,115200". We want to remove the "console=serial0,115200" part. You can either delete that part of the line (and only that part) or comment out the whole line (with a \# at the beginning) and rewrite/copy/paste a modified version of the line below it with the necessary edit. Once that's changed, Ctrl-x to save.

Lastly we need to tell systemd to not use that console anymore: `sudo systemctl stop serial-getty@ttyS0.service` followed by `sudo systemctl disable serial-getty@ttyS0.service`

## The actual program
And finally we need to install the actual rudLightsPi Python script and the Systemd service file to run it. Open up a terminal on, or shell into, your Pi W. I'll assume you're in the user pi's home folder, modify what follows appropriately if you're in some other folder. First up we need to clone the script to your Pi: `git clone https://github.com/Rudolph/rudLights.git` then `cd rudLights/rudLightsPi` so you're in the folder with rudLightsPi.py and rudlightspi.service. Copy the .py file to pi's home folder: `cp rudLightsPi.py /home/pi/`. If you want to keep it somewhere else you'll need to edit the .service file to point `Execstart` to the right location. Now copy the .service file so systemd can run the program at boot: `sudo cp rudlightspi.service /etc/systemd/system/` and tell systemd to enable it `sudo systemctl enable rudlightspi.service`. At this point it's probably easiest to simply reboot your Pi, that will ensure that everything is started up the way, and the order in which, it's supposed to.

## That's it (for now)
This little program gets the job done for me for now. I will be adding a (local) web interface someday so my wife's iPhone can play too. I imagine that will result in a complete rewrite of what is here now.