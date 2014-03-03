#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import json
import hashlib
import fcntl

class myAccount:
	def __init__(self, hhhh="none"):
		self.hhhh = hhhh
		self.path = "users/" + self.hhhh
	
	def create_new_account(self, user, password):
		self.login_account(user, password)
		self.setdata({}, 1)
		os.chmod(self.path, 0600)
	
	def login_account(self, user, password):
		dumy = "DQ1"
		name = dumy + "," + user + "," + password
		self.__init__(hashlib.sha1(name).hexdigest())
		
	def isexist(self):
		if os.path.isfile(self.path) == False:
			return 0
		return 1
	
	def setdata(self, data, write=0):
		if self.isexist() == 0 and write == 0:
			return None
		file = open(self.path, "w")
		fcntl.flock(file, fcntl.LOCK_EX)
		json.dump(data, file)
		fcntl.flock(file, fcntl.LOCK_UN)
		file.close()
	
	def getdata(self):
		if self.isexist() == 0:
			return None
		file = open(self.path, "r")
		data = file.read()
		file.close()
		return json.loads(data)

if __name__ == '__main__':
	a = myAccount()
	a.create_new_account("hello", "world")
	d = a.getdata()
	print d
	d.setdefault("test", 10)
	a.setdata(d)
	b = myAccount(a.hhhh)
	print b.getdata()

