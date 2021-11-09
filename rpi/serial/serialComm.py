import serial
import time

def initSerial():
    ser = serial.Serial(port='/dev/ttyAMA0', baudrate=9600, 
    timeout=1, write_timeout=1, parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)
    return ser

def readFromSerial(serPort):
    while (1):
        response = ser.readlines(None)
        if response != []:
            print(response)
            break
        print(response)
        time.sleep(2)

    print("reading success")

def sendToSerial(serPort, message):
    while (1):
        response = ser.readlines(None)
        if response != []:
            print(response)
            break
        print(response)
        time.sleep(2)
        ser.write(message.encode(encoding='UTF-8'))
    
    print("transmit success")