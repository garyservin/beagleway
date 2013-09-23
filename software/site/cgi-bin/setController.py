#!/usr/bin/python

import sys,json
import serial
import time

myjson = json.load(sys.stdin)

param = str(myjson["param"])
value = myjson["value"]

if param == 'Kc':
	param = 'P'
elif param == 'Ti':
	param = 'I'
elif param == 'Td':
	param = 'D'

# Setup the Serial Port
ser = serial.Serial('/dev/mbed', baudrate=115200)

with ser:
	ser.write(param+','+str(value)+';')

ser.close()

result={'success':'true','message':'El parametro ' + param + ' ha sido cambiado al valor ' + str(value)}

print 'Content-Type: application/json\n\n'
print
print json.JSONEncoder().encode(result)
