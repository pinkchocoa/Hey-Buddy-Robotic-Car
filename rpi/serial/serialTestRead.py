import serial
serialport = serial.Serial("/dev/ttyAMA0", 9600, timeout=0.5)
serialport.isOpen()
response = serialport.readlines(None)
print (response)
serialport.write("a".encode())
response = serialport.readlines(None)
print (response)
