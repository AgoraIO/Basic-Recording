#!/usr/bin/env python

import os
import sys
import glob
import subprocess

HOME = os.path.dirname(os.path.realpath(__file__))
pathEnv=os.getenv('PATH')
os.environ['PATH']= "%s" %(HOME) + ":" + pathEnv 


class AudioClip:
	def __init__(self):
		self.num = 0
		self.filename = []
		self.start_time = []
		self.end_time = []

	def put_file(self, name):
		if not (name in self.filename):
			self.filename.append(name)
			self.start_time.append(0.0)
			self.end_time.append(0.0)
			self.num = self.num + 1
		return self.filename.index(name)

	def max_length(self):
		return max(self.end_time)

	def print_filename(self):
		str = ""
		for i in range(self.num):
			str = str + ("-i %s " % self.filename[i])
		return str

	def print_filter(self):
		str = ""
		allch = ""
		for i in range(self.num):
			tmp = "[%d]adelay=%d[ad%d];" % ( (i), int(self.start_time[i]*1000)+1, (i))
			allch = allch + ("[ad%d]" % i)
			str = str + tmp
		str = str + ("%s amix=inputs=%d:dropout_transition=0.5[audio]" % (allch, self.num))
		return str

	def print_audio_info(self, i):
		print "Audio Clip %d: %s: start_time=%.3f, end_time=%.3f" % (i, self.filename[i], self.start_time[i], self.end_time[i])

	def print_ffmpeg(self, output_file):
		if self.num > 1:
			str = "ffmpeg " + self.print_filename()
			str = str + "-filter_complex \"%s\" " % self.print_filter()
			str = str + "-map \"[audio]\" -to %f -y %s" % (self.max_length(), output_file)
		elif self.num == 1:
			str = "ffmpeg -i %s -c:a copy %s" % (self.filename[0], output_file)
		else:
			str = ""
		return str

class VideoClip:
	def __init__(self):
		self.num = 0
		self.filename = []
		self.start_time = []
		self.end_time = []
		self.width = []
		self.height = []
		self.audio_file = ""
		self.audio_start_time = 0.0
		self.audio_end_time = 0.0

	def put_file(self, name):
		if not (name in self.filename):
			self.filename.append(name)
			self.start_time.append(0.0)
			self.end_time.append(0.0)
			self.width.append(0)
			self.height.append(0)
			self.num = self.num + 1
		return self.filename.index(name)
	
	def max_resolution(self):
		self.max_width = max(self.width)
		self.max_height = max(self.height)
		return self.max_width, self.max_height
	
	def max_length(self):
		return max(max(self.end_time), self.audio_end_time)
	
	def audio_delay_needed(self):
		return self.audio_file != "" and self.audio_start_time > 0.05
	
	def print_filter(self):
		if self.audio_delay_needed():
			audio_delay = int(self.audio_start_time*1000)
			str = "[0]adelay=%d[audio];" % audio_delay
		else:
			str = ""
		source = "1"
		sink = "out2"
		for i in range(self.num):
			sink = "out%d" % (i+2)
			if i == self.num - 1:
				sink = "video"
			tmp = "[%d]scale=%dx%d,setpts=PTS-STARTPTS+%.3f/TB[scale%d];[%s][scale%d]overlay=eof_action=pass[%s];" % \
				( (i+2), self.max_width, self.max_height, self.start_time[i], (i+2), source, (i+2), sink )
			str = str + tmp
			source = sink
		return str[:-1]
	
	def print_filename(self):
		str = ""
		for i in range(self.num):
			str = str + ("-i %s " % self.filename[i])
		return str
	
	def print_ffmpeg(self, output_file):
		if self.audio_file == "":
			str = "ffmpeg -f lavfi -i anullsrc "
		else:
			str = "ffmpeg -i %s " % self.audio_file
		str = str + "-f lavfi -i \"color=black:s=%dx%d:r=15\" " % (self.max_width, self.max_height)
		str = str + self.print_filename()
		str = str + "-filter_complex \"%s\" " % self.print_filter()
		if self.audio_file == "":
			map_option = "-map \"[video]\""
		else:
			if self.audio_delay_needed():
				map_option = "-map \"[audio]\" -map \"[video]\" -c:a aac"
			else:
				map_option = "-map 0:a:0 -map \"[video]\" -c:a copy"
		str = str + "%s -c:v libx264 -preset veryfast -to %f -y %s" % (map_option, self.max_length(), output_file)
		return str
	
	def print_audio_info(self):
		print "Audio Clip: %s: start_time=%.3f, end_time=%.3f" % (self.audio_file, self.audio_start_time, self.audio_end_time)
	
	def print_video_info(self, i):
		print "Video Clip %d: %s: start_time=%.3f, end_time=%.3f, width=%d, height=%d" % \
			(i, self.filename[i], self.start_time[i], self.end_time[i], self.width[i], self.height[i])
	

if len(sys.argv) <= 1:
	print "Usage: python video_convert.py path_of_folder"
	quit()

folder_name = sys.argv[1]
print "Folder name:"+folder_name

if not os.path.isdir(folder_name):
	print "Folder "+folder_name+" does not exit"
	quit()

os.chdir(folder_name)
child_env = os.environ.copy()
all_uid_file = glob.glob("uid_*.txt")

for uid_file in all_uid_file:
	uid = os.path.splitext(uid_file)[0][4:]
	print "UID:"+uid
	
	clip = VideoClip()
	audio_clip = AudioClip()
	with open(uid_file) as f:
		for line in f:
			items = line.split(" ")
			#audio file
			if items[1][-3:] == "aac":
				index = audio_clip.put_file(items[1])
				if items[2] == "create":
					audio_clip.start_time[index] = float(items[0])
				elif items[2] == "close":
					audio_clip.end_time[index] = float(items[0])
			#video file
			if items[1][-3:] == "mp4":
				index = clip.put_file(items[1])
				if items[2] == "create":
					clip.start_time[index] = float(items[0])
				elif items[2] == "info":
					clip.start_time[index] = float(items[0])
					clip.width[index] = int(items[3][6:])
					clip.height[index] = int(items[4][7:])
					rotation = int(items[5][9:])
					if rotation == 90 or rotation == 270:
						clip.width[index], clip.height[index] = clip.height[index], clip.width[index]
				elif items[2] == "close":
					clip.end_time[index] = float(items[0])
			#video file
			if items[1][-4:] == "webm":
				index = clip.put_file(items[1])
				if items[2] == "create":
					clip.start_time[index] = float(items[0])
				elif items[2] == "info":
					clip.start_time[index] = float(items[0])
					clip.width[index] = int(items[3][6:])
					clip.height[index] = int(items[4][7:])
					rotation = int(items[5][9:])
					if rotation == 90 or rotation == 270:
						clip.width[index], clip.height[index] = clip.height[index], clip.width[index]
				elif items[2] == "close":
					clip.end_time[index] = float(items[0])

		
		clip.print_audio_info()
		for i in range(audio_clip.num):
			audio_clip.print_audio_info(i)
		for i in range(clip.num):
			clip.print_video_info(i)
		
	if audio_clip.num > 1:
		print "Generate Audio File"
		tmp_audio = uid+"_tmp.m4a"
		command = audio_clip.print_ffmpeg(tmp_audio)
		clip.audio_file = tmp_audio
		clip.audio_start_time = 0.0
		clip.audio_end_time = audio_clip.max_length()
		print command
		print subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, env=child_env).stdout.read()
	elif audio_clip.num == 1:
		clip.audio_file = audio_clip.filename[0]
		clip.audio_start_time = audio_clip.start_time[0]
		clip.audio_end_time = audio_clip.end_time[0]

	if clip.num > 0:
		print "Generate MP4 file:"
		print "Output resolution:", clip.max_resolution()
		output_file = uid+"_av"+".mp4"
		#print clip.print_filter()
		command =  clip.print_ffmpeg(output_file)
	else:
		tmp_audio = uid+"_tmp.m4a"
		output_file = uid+".m4a"
		if audio_clip.num > 1:
			command = "mv %s %s" % (tmp_audio, output_file)
		elif audio_clip.num == 1:
			command = "ffmpeg -i %s -c:a copy -y %s" % (clip.audio_file, output_file)
	print command
	print subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, env=child_env).stdout.read()
	print "\n\n"

#write a convert done file
f = open("convert-done.txt", "w+")
f.close()

#remove tmp files
os.system('rm -f *_tmp.m4a')

