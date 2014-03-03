#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cgi
import os

import auth

def check_answer(pwlt, a):
	# get form data
	form = cgi.FieldStorage()
	if "answer" not in form:
		return 0
	if "level" not in form:
		return 0
	answ = form.getvalue("answer")
	levl = form.getvalue("level")
	if not levl in pwlt.keys():
		return 0
	# compare correct answer
	if answ == pwlt[levl]:
		# update database
		a.setdata(update_dist(a.getdata(), levl))
		return 1
	return 0

def get_ip_address():
	if os.environ.has_key("REMOTE_ADDR"):
		return os.environ["REMOTE_ADDR"]
	return ""

def update_dist(d, database_keywd):
	# updating
	#   flag: clear flag (1=cleard, 0=uncleard)
	#   addr: ip address of first correct player
	if database_keywd in d.keys():
		d[database_keywd]["flag"] = 1
	else:
		d.setdefault(database_keywd, {})
		d[database_keywd].setdefault("flag", 1)
		d[database_keywd].setdefault("addr", get_ip_address())
	return d

if __name__ == '__main__':
	# passwd list
	pwlt = {
		# Reading
		"l0":"16011",  "l1":"523776",   "l2":"3628800", "l3":"strchr",
		"l4":"memset", "l5":"15485863", "l6":"2.38800", "l7":"WindowsProgram",
		"l8":"base64", "l9":"aAaA",
		# Writing
		"l10":"worldtrigger", "l11":"BabySteps", "l12":"MagicaMadoca",
		"l13":"Nyanpath",     "l14":"PazzDra",   "l15":"LAstEsCaPE",
	}
	# check auth
	a = auth.check_auth()
	if a == None:
		auth.html("login")
	# check answer
	if check_answer(pwlt, a) == 0:
		auth.html("err")
	auth.html("ok")

