#!/usr/bin/env python
# -*- coding: utf-8 -*-

Ans = 7
Val = {
	1:200, 
	2:17*256, 
	3:27*256*256
}
Ret = []

def f(m, v, x):
	if x >= m:
	    if x == m:
	        w = v[:]
	        Ret.append(w)
	    return
	for a in Val.keys():
	    v.append(a)
	    f(m, v, x+a)
	    v.pop()
	return

f(Ans, [], 0)
Y = 0
for li in Ret:
	X = 1
	for v in li:
	    X *= Val[v]
	Y += X
print Y
