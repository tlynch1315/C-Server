#!/usr/bin/env python2.7

import requests
import multiprocessing
import time
import sys
import os

def Usage(ExitCode):
	print '''Usage: thor.py [-p PROCESSES -r REQUESTS -v] URL
	-h			Display help message
	-v			Display verbose output
	
	-p	PROCESSES	Number of processes to utilize (1)
	-r	REQUESTS	Number of requests per process (1)'''
	exit(ExitCode)

def do_request(x):
	ProcessNumber = x
	ProcessSum = 0
	for Hammer in xrange(Requests):
		Start = time.time()
		Response = requests.get(Url)
		End = time.time()
		ProcessSum += (End-Start)
		if Verbose:
			print Response.content
		print "Process: " + str(ProcessNumber) + ", Requests: " + str(Hammer) + ", Elapsed Time: " + str(End-Start)
	print "Process: " + str(ProcessNumber) + ", AVERAGE    , Elapsed Time: " + str(ProcessSum/Requests)
	return (ProcessSum)

Processes =	1
Requests =	1

Verbose = False
Url = ""
argind = 1
if len(sys.argv) == 1:
	Usage(1)

while argind < len(sys.argv):
	arg = sys.argv[argind]
	argind+=1
	if arg == "-h":
		Usage(0)
	elif arg == "-v":
		Verbose = True
	elif arg == "-p":
		Processes = int(sys.argv[argind])
		argind+=1
	elif arg == "-r":
		Requests = int(sys.argv[argind])
		argind+=1
	else:
		Url = arg
pool = multiprocessing.Pool(Processes)
Speed = pool.map(do_request, range(Processes))
	
print "TOTAL AVERAGE ELAPSED TIME: " + str(sum(Speed)/(Processes*Requests))

