import serialComm as s

ser = s.initSerial()
while(1):
	s.sendToSerial(ser, '1')
