#!/usr/bin/env python
# -*- coding: utf-8 -*-

import auth

if __name__ == '__main__':
	# check auth
	a = auth.check_auth()
	if a == None:
		auth.html("login")
	# print cleard levels from database
	d = a.getdata()
	if "howto" in d.keys():
		r = "clear?"
	else:
		r = "cleard"
	for k in d.keys():
		if not k[0] == 'l':
			continue
		try:
			num = int(k[1:])
			r += ("," + str(num))
		except:
			continue
	auth.html(r)

