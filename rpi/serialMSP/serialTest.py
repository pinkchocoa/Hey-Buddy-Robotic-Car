import serialComm as s

ser = s.initSerial()
while(1):
        s.sendToSerialNo(ser, 'w')
