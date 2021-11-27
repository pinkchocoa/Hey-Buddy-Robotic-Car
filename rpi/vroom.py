from serialMSP.serialComm import initSerial, readFromSerial, sendToSerial
from audio.micRec import micRec
from camera.fileio import file_to_list

recordTime = 3

serialMsg = {
    "on led" : "1",
    "off led" : "2",
    "forward" : "w",
    "back" : "s",
    "left" : "a",
    "right" : "d",
    "follow" : "followme",
}

saidMsg = {
    "on led" : ['', 'on'],
    "off led" : ['', 'off'],
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
        print("cannot understand ya")
        return False
    #print("you said " + said)
    if checkInput(said, saidMsg["stop"]):
        print("exiting loopUntilStop")
        return True
    return False

sPort = initSerial()
# readFromSerial(sPort)
# ^ for going to sleep message, else not needed
while(1):
    said = micRec(recordTime)
    if said is None or not said:
        print("cannot understand ya")
        continue
    print("you said " + said)
    for key in saidMsg:
        if checkInput(said, saidMsg[key]):
            if serialMsg[key] is not "followme":
                sendToSerial(sPort, serialMsg[key])
                break
            elif serialMsg[key] is "followme": # function handled by raspberrypi
                # check left/middle/right
                # i gotta loop this though?
                # via time? via stop command?
                #loop until stop command..
                print("follow me")
                # test = loopUntilStop()
                test = False
                while test is False:
                    print("follow me loop")
                    data = file_to_list(outputFile)
                    coord = float(data[0])
                    print(coord)
                    if coord < camWidth/3 :
                        print("left")
                        sendToSerial(sPort, direction["left"])
                    elif coord >= camWidth/3 and coord < camWidth/3*2:
                        print("middle")
                        sendToSerial(sPort, direction["middle"])
                    elif coord <= camWidth:
                        print("right")
                        sendToSerial(sPort, direction["right"])
                    test = loopUntilStop()
                if test is True:
                    break
                
