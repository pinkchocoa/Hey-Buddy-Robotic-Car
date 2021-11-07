import subprocess 
import os 
import time
import signal
import speech_recognition as sr

def recordSound(output, duration, path=""):
	def stopRec(p, duration):
		time.sleep(duration)
		print("stop recording pid: " + str(p.pid))
		p.terminate()
		p = None

	record = 'arecord -d ' + str(duration) +' -r 22050 '
	record += path + output + '.wav'

	p = subprocess.Popen(record, shell=True, preexec_fn=os.setsid)
	print("startRecordingArecord()> p pid= " + str(p.pid))
	print("startRecordingArecord()> recording started")
	print("command ran: " + str(record))
	stopRec(p, duration)

def playSound(input, path=""):
	arg = 'aplay ' + path + input + '.wav'
	print("playing: " + arg)
	subprocess.call(arg, shell=True)

def transribeSound(input, path=""):
	audioFile = path + input + '.wav'
	print(audioFile)
	r = sr.Recognizer()
	with sr.AudioFile(audioFile) as source:
		audio = r.record(source) #read audio file
	try:
		txt =  r.recognize_google(audio)
		txt = txt.replace("'", "")
		txt = txt.replace('"', '')
		print("test" + txt)
		return txt
	except sr.UnknownValueError:
		return "cannot understand" #cannot understand audio ):
	except sr.RequestError as e:
		return "request error" #request error
	return "hello world"

recordSound('out', 5)
print(transribeSound('out'))
