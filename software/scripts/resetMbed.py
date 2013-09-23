#!/usr/bin/python

import BBIO.GPIO as GPIO
import time

GPIO.setup("P9_15", GPIO.OUT)	# Set BBB pin as output.
print 'Reseteando el mbed...'
GPIO.output("P9_15", GPIO.LOW)	# Set pin as low to reset the Mbed.
time.sleep(0.5)			# Wait 0.2 seconds.
GPIO.output("P9_15", GPIO.HIGH)	# Set pins as high.
GPIO.cleanup()			# Unexport pin from user space.
