#!/usr/bin/python
# -*- coding: utf-8 -*-

import serial
import MySQLdb
import sys
from config import *

leidos = 0
grabados = 0

while True:
	try:
		# Setup the Serial Port
		with serial.Serial('/dev/mbed', baudrate=115200, timeout=0.5) as ser:
			# Read Serial Port and strip '\r\n'
			line = ser.readline(70).strip('\r\n')
			leidos += 1

		# Close the Serial port
		ser.close()

		values = line.split(',')

		if len(values) == 6:
			grabados += 1
			# Setup the Database connection
			db = MySQLdb.connect(local_host, local_user, local_passwd, local_db)
			with db:
				cur = db.cursor()
				sql = "INSERT INTO lecturas (acelX, gyrX, angleComplementary, angleKalman, controlPID, controlState) VALUES (%s, %s, %s, %s, %s, %s)" % (values[0], values[1], values[2], values[3], values[4], values[5])
				cur.execute(sql)
				db.commit()
			# Close Database connection
			db.close()

	except KeyboardInterrupt:
		print 'Leidos|Grabados|%'
		print str(leidos)+'|'+str(grabados)+'|'+str(float(grabados)/leidos*100.0)+'%'
		sys.exit(0) # or 1, or whatever

