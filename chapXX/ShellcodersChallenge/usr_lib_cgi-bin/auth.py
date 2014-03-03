#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cgi
import sys
import time

import myCookie
import myAccount

def html(text):
	print "Content-Type: text/html"
	print
	sys.stdout.write(text)
	sys.exit()

def check_user_pass():
	# get form data
	form = cgi.FieldStorage()
	if "user" not in form or "pass" not in form:
		return None
	user = form.getvalue("user")
	pswd = form.getvalue("pass")
	# check the account info
	a = myAccount.myAccount()
	a.login_account(user, pswd)
	if a.isexist() == 0:
		return None
	return a

def check_auth():
	# get cookie data
	ckie = myCookie.myCookie()
	ssin = ckie.getcookie("session")
	if ssin == None:
		# nothing cookie, next check user/pass
		a = check_user_pass()
		if a == None:
			return None
		k = a.hhhh
	else:
		# check session time
		(t, k) = ssin.split(',')
		if int(t) < int(time.time()):
			# timeout, next check user/pass
			a = check_user_pass()
			if a == None:
				return None
			k = a.hhhh
		else:
			a = myAccount.myAccount(k)
	# update session
	t = str(int(time.time()) + 60 * 30)
	ckie.setcookie("session", t + "," + k)
	print ckie.output()
	return a

if __name__ == '__main__':
	if check_auth() == None:
		html("err")
	else:
		html("ok")

