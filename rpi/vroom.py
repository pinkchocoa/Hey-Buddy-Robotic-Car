from serialMSP.serialComm import initSerial, readFromSerial, sendToSerial
from audio.micRec import micRec

serialMsg = {
    "on led" : "1",
    "off led" : "2",
    "forward" : "w",
    "back" : "s",
    "left" : "a",
    "right" : "d",
}

saidMsg = {
    "on led" : ['', 'on'],
    "off led" : ['', 'off'],
    "forward" : ['', 'forward'],
    "back" : ['', 'back'],
    "left" : ['', 'left'],
    "right" : ['', 'right'],
}

def checkInput(said, inputs):
    if said is None or not said:
        return False
    print("You said this: " + str(said))
    for x in inputs:
        if x not in said:
            return False
    return True

sPort = initSerial()
readFromSerial(sPort)

while(1):
    said = micRec(5)
    if said is None or not said:
        continue
    print("you said " + said)
    for key in saidMsg:
        if checkInput(said, saidMsg[key]):
            sendToSerial(sPort, serialMsg[key])
            break
            