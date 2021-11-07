#!/usr/bin/env python3

#pip install num2words
#from num2words import num2words #idk why this is needed
from subprocess import call

def speakText(input):
	cmdBegin = 'espeak '
	cmdEnd='" 2>/dev/null'
	cmdOut='"'
	cmdOut+=input
	cmdFinal=cmdBegin+cmdOut+cmdEnd
	call([cmdFinal], shell=True)
	return cmdFinal #to check command

#cmdEnd = ' | aplay /home/pi/Destop/text.wav 2>/dev/null' # play text.wav and dump errors to /dev/null
#cmdOut = '--stdout > /home/pi/Desktop/text.wav ' #store the voice file

#testing
#text = input("Enter text: ")
#print("speaking... ", text)
#text = text.replace(' ', '_') #need to replace spaces with underscores to identify as a command
#cmdOut += text
#print(speakText(text))

