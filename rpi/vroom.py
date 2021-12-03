from serialMSP.serialComm import initSerial, sendToSerialNo
from audio.micRec import micRec
from camera.fileio import file_to_list

#recordTime = 1
recordLong = 3
camWidth = 640
outputFile = "coord.txt"

#dictionary that contains the character to be sent to raspberry pi
serialMsg = {
    "forward" : "w",
    "stop" : "s",
    "left" : "a",
    "right" : "d",
    "follow" : "followme",
    "on red" : "1",
    "off red" : "2",
    "on green" : "3",
    "off green" : "4",
    "on blue" : "5",
    "off blue" : "6",
    "on light" : "7",
    "off light" : "8",
    "on lights" : "7",
    "off lights" : "8",
}

saidMsg = {
    "forward" : ['move', ''],
    "stop" : ['', 'stop'],
    "left" : ['turn', 'left'],
    "right" : ['turn', 'right'],
    "follow" : ['follow', 'me'],
    # not allowing led2 as commands as they are used as indicators
    # "on red" : ['on', 'red'],
    # "off red" : ['off', 'red'],
    # "on green" : ['on', 'green'],
    # "off green" : ['off', 'green'],
    # "on blue" : ['on', 'blue'],
    # "off blue" : ['off', 'blue'],
    "on light" : ['on', 'light'],
    "off light" : ['off', 'light'],
    "on lights" : ['on', 'lights'],
    "off lights" : ['off', 'lights'],
}

def checkInput(said, inputs):
    if said is None or not said:
        return False
    #print("You said this: " + str(said))
    for x in inputs:
        if x not in said:
            return False
    return True

#function currently not in use
#it causes latency that affects the follow me command
def loopUntilStop():
    #print("loopUntilStop")
    said = micRec(recordTime)
    if said is None or not said:
        return False
    print("you said " + said)
    if checkInput(said, saidMsg["stop"]):
        print("exiting loopUntilStop")
        return True
    return False

#after hearing activation word, 
#it tries to hear for a valid command
#cyan LED is on if a valid command is heard
def heyBuddy():
    said = micRec(recordLong)
    if said is None or not said:
        #print("cannot understand ya")
        return
    #print("heard command: " + said)
    for key in saidMsg:
        if checkInput(said, saidMsg[key]):
            sendToSerialNo(sPort, serialMsg['on green'])
            if serialMsg[key] is not "followme":
                sendToSerialNo(sPort, serialMsg[key])
                break
            elif serialMsg[key] is "followme":
                test = False
                while test is False:
                    data = file_to_list(outputFile)
                    try:
                        coord = float(data[0])
                    except:
                        coord = -1
                    if coord == -1:
                        sendToSerialNo(sPort, serialMsg['stop'])
                    elif coord < camWidth/3:
                        sendToSerialNo(sPort, serialMsg["left"])
                    elif coord >= camWidth/3 and coord < camWidth/3*2:
                        sendToSerialNo(sPort, serialMsg["forward"])
                    elif coord <= camWidth:
                        sendToSerialNo(sPort, serialMsg["right"])
                    #test = loopUntilStop()
                if test is True:
                    break
    sendToSerialNo(sPort, serialMsg['off green'])

#init serial port to MSP
sPort = initSerial()

#hears for activation word 'hey buddy'
#blue LED is turned on when heard
while (1):
    said = micRec(recordLong)
    if said is None or not said:
        continue
    #print('heard activation: ' + said)
    if checkInput(said, ['hey', 'buddy']):
        #print("hey buddy!")
        sendToSerialNo(sPort, serialMsg['on blue'])
        heyBuddy()
        sendToSerialNo(sPort, serialMsg['off blue'])
    elif checkInput(said, ['stop']):
        sendToSerialNo(sPort, serialMsg['stop'])
    
