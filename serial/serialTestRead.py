import serial
serialport = serial.Serial("/dev/tty1", 9600, timeout=0.5)
response = serialport.readlines(None)
print (response)
serialport.write("a".encode())
response = serialport.readlines(None)
print (response)
