import serialComm as s

ser = s.initSerial()
while(1):
	sendToSerial(ser, '1')
