#!/usr/bin/python

import serial
import socket
import time


try:
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.connect(("gmail.com",80))
	ip = s.getsockname()[0]
	s.close()

	ipToSend = 'IP,' + '{:^16}'.format(ip) + ';'

	mbed = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=3.0)
	mbed.write(ipToSend)

	print ipToSend
except:
	print 'NO hay ip'

