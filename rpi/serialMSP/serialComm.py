import serial
import time

#init the serial port to MSP
def initSerial():
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600, 
    timeout=1, write_timeout=1, parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)
    return ser

#read from serial
def readFromSerial(serPort):
    while (1):
        response = serPort.readlines(None)
        if response != []:
            print(response)
            break
        print(response)

#sending with a response message expected from uart
def sendToSerial(serPort, message):
    while (1):
        print("attempting to transmit: " + message)
        serPort.write(message.encode(encoding='UTF-8'))
        response = serPort.readlines(None)
        if response != []:
            print(response)
            break
        print(response)
        
#no respond message from uart
def sendToSerialNo(serPort, message):
    #print("attempting to transmit: " + message)
    serPort.write(message.encode(encoding='UTF-8'))
    response = serPort.readlines(None)
    time.sleep(0.5)

