#!/usr/bin/env python3
#
# rudLightsPi
# Copyright © 2018 Del Rudolph <me@darthrudolph.com>
# Released under the GNU General Public License version 3
#
# Receives text strings from app via Bluetooth and displays them on a string
# of Christmas lights. Yes, inspired by Stranger Things, as well as the 
# countless other Stranger Things inspired projects out there. This one was 
# initially created as a Christmas gift for my niece and expanded to what 
# you see now.

from bluetooth import *
import socket
import serial
import logging
import logging.handlers

# Set up logging
l = logging.getLogger(__name__)
l.setLevel(logging.DEBUG)

lHandler = logging.handlers.RotatingFileHandler('/var/log/rudLightsPi.log', maxBytes = 10000, backupCount = 3)

lFormatter = logging.Formatter('%(asctime)s %(levelname)s: %(message)s')

lHandler.setFormatter(lFormatter)

l.addHandler(lHandler)

# set up new server socket using RFCOMM protocol
server_sock = BluetoothSocket(RFCOMM)

# bind it to any port
server_sock.bind(("",PORT_ANY))

# start listening
server_sock.listen(1)

# set up serial connection to Arduino
ser = serial.Serial('/dev/serial0',
                    baudrate = 9600,
                    parity = serial.PARITY_NONE,
                    stopbits = serial.STOPBITS_ONE,
                    bytesize = serial.EIGHTBITS
                    )

while True:
    try:
        # Wait for a connection
        l.info("Waiting for connection")

        # Blocks until a connection is made
        client_sock, client_info = server_sock.accept()
        l.info("Accepted connection from {}".format(client_info))
        
        # Now that there is a connection go to non-blocking
        server_sock.setblocking(0)

        # short version:
        # if there's data in BT
        #     write it to Serial
        # if there's data in Serial
        #     write it to BT

        while True:
            # BT->Serial
            try:
                # Take a non-blocking peek at next byte in the BT pipe.
                btDatum = client_sock.recv(1, socket.MSG_PEEK) # | socket.MSG_DONTWAIT)
                
                # If there is one,
                if len(btDatum) > 0:
                    # there is probably more. Read in the whole lot of it
                    # (no, it should never be that big)
                    theData = client_sock.recv(1024).decode("ascii")
                    l.debug("BT->Ser received {}".format(theData))
                    
                    try:
                        # and forward it on to Serial
                        outData = theData + "\r\n"
                        ser.write(bytes(outData, encoding="ascii"))
                        l.debug("BT->Ser forwarded: {}".format(theData))
                    except serial.SerialException as e:
                        l.debug("BT->Serial SerialException {}".format(e), exc_info=True)
                        pass
                    
            except socket.error as e:
                # Couldn't peek, probably due to lost connection.
                l.debug("BT->Ser socket.error {}".format(e), exc_info=True)
                break
            
            # Serial->BT
            try:
                # If there is anything waiting in Serial
                if (ser.in_waiting > 0):
                    # read it all in and forward it on to BT
                    serData = ser.read(ser.in_waiting)
                    client_sock.sendall(serData)
                
            except IOError as e:
                # Couldn't read from the serial port
                l.debug("Ser->BT IOError {}".format(e), exc_info=True)
                pass

        # end up here when connection is lost
        # re-enable blocking to await next connection
        l.info("Connection closed")
        client_sock.close()
        server_sock.setblocking(1)

    except KeyboardInterrupt:
        l.info("Shutting down")
        server_sock.close()
        ser.close()
        break
        
l.info("rudLightsPi stopped")
