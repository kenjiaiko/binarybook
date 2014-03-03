#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

import auth

def printpdf():
	print "Content-Type: application/pdf"
	print
	f = open('howtostart2.pdf', "rb")
	for data in f:
		sys.stdout.write(data)
	f.close()

if __name__ == '__main__':
	# check auth
	a = auth.check_auth()
	if a == None:
		auth.html("please login.")
	# set reading flag
	d = a.getdata()
        d.setdefault("howto", 1)
	d["howto"] = 1
        a.setdata(d)
	# print PDF file
	printpdf()
	sys.exit(0)

