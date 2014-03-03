#!/usr/bin/env python
# -*- coding: utf-8 -*-

import Cookie
import Crypto.Cipher.AES
import base64
import random
import os
import time

class myCookie:
	
	def __init__(self, key="if you can dream it, you can do it."):
		if len(key) > 32:
			key = key[:32]
		key = self.alignment(key, 32)
		self.h = Crypto.Cipher.AES.new(key)
		if "HTTP_COOKIE" in os.environ:
			self.c = Cookie.SimpleCookie(os.environ["HTTP_COOKIE"])
		else:
			self.c = Cookie.SimpleCookie()
	
	def alignment(self, data, size):
		if (len(data) % size) == 0:
			return data
		return data + ("A" * (size - (len(data) % size)))
	
	def encrypt(self, data):
		dumy = str(random.randint(2525, 9999))
		size = "%04d" % len(data)
		data = self.alignment(dumy + size + data, 16)
		return base64.b64encode(self.h.encrypt(data))
	
	def decrypt(self, data):
		data = self.h.decrypt(base64.b64decode(data))
		dumy = data[0:4]
		size = data[4:8]
		data = data[8:8+int(size, 10)]
		return data
	
	def setcookie(self, key, data):
		self.c[key] = self.encrypt(data)
		
	def getcookie(self, key):
		try:
			return self.decrypt(self.c[key].value)
		except(Cookie.CookieError, KeyError):
			pass
		return None
	
	def output(self):
		return self.c.output()

	def deletecookie(self, key):
		self.c[key]["expires"] = "Mon, 31-Dec-2001 00:00:00 GMT"
		return self.c.output([key, "expires"])

if __name__ == '__main__':
	t = ""
	c = myCookie()
	s = c.getcookie("session")
	if s == None:
		t = "you need login, again"
	else:
		if int(s) < int(time.time()):
			t = "timeout, you need login"
		else:
			t = "update session"
	
	c.setcookie("session", str(int(time.time()) + (60 * 3)))
	print c.output()
	print "Content-Type: text/html"
	print
	print t

