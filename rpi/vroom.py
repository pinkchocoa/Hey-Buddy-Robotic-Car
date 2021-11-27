from serialMSP.serialComm import initSerial, readFromSerial, sendToSerial
from audio.micRec import micRec
from camera.fileio import file_to_list

recordTime = 3
recordLong = 5

serialMsg = {
    "on blue led" : "1",
    "off blue led" : "2",
    "on red led" : "3",
    "off red led" : "4",
    "forward" : "w",
    "back" : "s",
    "left" : "a",
    "right" : "d",
    "follow" : "followme",
}

saidMsg = {
    "on blue led" : ['', 'on'],
    "off blue led" : ['', 'off'],
    "forward" : ['', 'forward'],
    "back" : ['', 'back'],
    "left" : ['', 'left'],
    "right" : ['', 'right'],
    "follow" : ['follow', 'me'],
    "stop" : ['stop', ''],
}

camWidth = 640
outputFile = "camera/coord.txt"
direction = {
    "left" : "l",
    "middle" : "m",
    "right" : "r",
}

def checkInput(said, inputs):
    if said is None or not said:
        return False
    print("You said this: " + str(said))
    for x in inputs:
        if x not in said:
            return False
    return True

def loopUntilStop():
    print("loopUntilStop")
    said = micRec(recordTime)
    if said is None or not said:
        return False
    #print("you said " + said)
    if checkInput(said, saidMsg["stop"]):
        print("exiting loopUntilStop")
        return True
    return False

def heyBuddy():
    said = micRec(recordLong)
    if said is None or not said:
        print("cannot understand ya")
        return
    print("you said " + said)
    for key in saidMsg:
        if checkInput(said, saidMsg[key]):
            if serialMsg[key] is not "followme":
                sendToSerial(sPort, serialMsg['on red led'])
                sendToSerial(sPort, serialMsg[key])
                break
            elif serialMsg[key] is "followme":
                sendToSerial(sPort, serialMsg['on red led'])
                test = False
                while test is False:
                    print("follow me loop")
                    data = file_to_list(outputFile)
                    coord = float(data[0])
                    print(coord)
                    if coord < camWidth/3 :
                        sendToSerial(sPort, direction["left"])
                    elif coord >= camWidth/3 and coord < camWidth/3*2:
                        sendToSerial(sPort, direction["middle"])
                    elif coord <= camWidth:
                        sendToSerial(sPort, direction["right"])
                    test = loopUntilStop()
                if test is True:
                    break
    sendToSerial(sPort, serialMsg['off red led'])
                
sPort = initSerial()
# readFromSerial(sPort)
# ^ for going to sleep message, else not needed
while (1):
    print("waiting for activation word")
    said = micRec(recordLong)
    if said is None or not said:
        continue
    print('you said: ' + said)
    if checkInput(said, ['hey', 'buddy']):
        print("hey buddy!")
        sendToSerial(sPort, serialMsg['on blue led'])
        heyBuddy()
        sendToSerial(sPort, serialMsg['off blue led'])
    