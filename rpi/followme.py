from serialMSP.serialComm import initSerial, sendToSerialNo
from camera.fileio import file_to_list

recordTime = 1
recordLong = 3

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

camWidth = 640
outputFile = "coord.txt"
direction = {
    "left" : "l",
    "middle" : "m",
    "right" : "r",
}
                
sPort = initSerial()

while (1):
    data = file_to_list(outputFile)
    try:
        coord = float(data[0])
    except:
        coord = -1
    if coord == -1:
        sendToSerialNo(sPort, serialMsg['stop'])
    elif coord < camWidth/3:
        sendToSerialNo(sPort, direction["left"])
    elif coord >= camWidth/3 and coord < camWidth/3*2:
        sendToSerialNo(sPort, direction["middle"])
    elif coord <= camWidth:
        sendToSerialNo(sPort, direction["right"])
