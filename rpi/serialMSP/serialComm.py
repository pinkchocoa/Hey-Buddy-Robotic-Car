import serial
import time

def initSerial():
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600, 
    timeout=1, write_timeout=1, parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)
    return ser

def readFromSerial(serPort):
    while (1):
        response = serPort.readlines(None)
        if response != []:
            print(response)
            break
        print(response)
        time.sleep(2)

    print("reading success")

def sendToSerial(serPort, message):
    while (1):
        print("attempting to transmit: " + message)
        serPort.write(message.encode(encoding='UTF-8'))
        response = serPort.readlines(None)
        if response != []:
            print(response)
            break
        print(response)
        time.sleep(2)
    
    print("transmit success")


# uncomment for testing

# ser = initSerial()
# readFromSerial(ser)
# sendToSerial(ser, 'w')
# sendToSerial(ser, '1')
# sendToSerial(ser, '2')
