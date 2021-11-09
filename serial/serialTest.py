import serial
import time

ser = serial.Serial(port='/dev/ttyAMA0', baudrate=9600, 
timeout=1, write_timeout=1, parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE,
bytesize=serial.EIGHTBITS);

while (1):
	response = ser.readlines(None)
	if response != []:
		print(response)
		break

	print(response)
	time.sleep(2)

print("reading success, now attempt to transmit")

while (1):
	response = ser.readlines(None)
	if response != []:
		print(response)
		break
	print(response)
	time.sleep(2)
	ser.write("b".encode(encoding='UTF-8'))

while (1):
	response = ser.readlines(None)
	if response != []:
		print(response)
		break
	print(response)
	time.sleep(2)
	ser.write("a".encode(encoding='UTF-8'))

