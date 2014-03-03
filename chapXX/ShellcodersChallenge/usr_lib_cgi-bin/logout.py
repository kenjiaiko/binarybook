#!/usr/bin/env python
# -*- coding: utf-8 -*-

import auth
import myCookie

if __name__ == '__main__':
	# check auth
	a = auth.check_auth()
	if a == None:
		auth.html("login")
	# print cleard levels from database
	c = myCookie.myCookie()
	print c.deletecookie("session")
	auth.html("login")

