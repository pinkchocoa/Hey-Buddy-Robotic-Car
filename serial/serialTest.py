import serial
import time

ser = serial.Serial(port='/dev/ttyAMA0', baudrate=9600, timeout=1, write_timeout=1)

while (1):
	response = ser.readlines(None)
	print(response)
	time.sleep(2)
