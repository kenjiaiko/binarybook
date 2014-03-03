#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cgi
import httplib
import hashlib

import auth
import myAccount

def checkcapt(idnt, ans):
	c = httplib.HTTPConnection("captchator.com")
	c.request("GET", "/captcha/check_answer/" + idnt + "/" + ans)
	r = c.getresponse()
	if r.status != 200:
		return 1
	if r.read() != "1":
		return 1
	return 0

def makecode(sid):
	magic = "DQ8"
	tmp = hashlib.sha1(magic + sid).hexdigest()
	return hashlib.sha1(magic + tmp + sid).hexdigest()

def checkcode(idnt, code):
	if makecode(idnt) != code:
		return 1
	return 0

if __name__ == '__main__':
	a = myAccount.myAccount()
	form = cgi.FieldStorage()
	
	# make code routine
	if "sid" in form:
		auth.html(makecode(form.getvalue("sid")))
	
	# regist routine
	#   check code and idnt
	if "idnt" not in form or "code" not in form:
		auth.html("err")
	idnt = form.getvalue("idnt")
	code = form.getvalue("code")
	if checkcode(idnt, code) == 1:
		auth.html("err")
	
	#   check captcha
	if "capt" not in form:
		auth.html("err")
	capt = form.getvalue("capt")
	if checkcapt(idnt, capt) == 1:
		auth.html("err")
	
	#   check the account yet,
	if "user" not in form or "pass" not in form:
		auth.html("err")
	user = form.getvalue("user")
	pswd = form.getvalue("pass")
	a.login_account(user, pswd)
	if a.isexist() == 1:
		auth.html("yet")
	
	#   create the account info
	a.create_new_account(user, pswd)
	d = a.getdata()
	d.setdefault("user", user)
	a.setdata(d)
	auth.html("ok")

