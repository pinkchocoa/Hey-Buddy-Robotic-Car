import serial
import time

ser = serial.Serial(port='/dev/ttyAMA0', baudrate=9600, timeout=1, write_timeout=1)

while (1):
	response = ser.readlines(None)
	if response != []:
		print(response)
		break

	print(response)
	time.sleep(2)

print("reading success, now attempt to transmit")
ser.write(ord('a'))

while (1):
	
	response = ser.readlines(None)
	if response != []:
		print(response)
		break
	print(response)
	time.sleep(2)
	ser.write(ord('b'))
