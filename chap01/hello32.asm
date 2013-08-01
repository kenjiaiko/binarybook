extern MessageBoxA

section .text
global main

main:
	push dword 0
	push dword title
	push dword text
	push dword 0
	call MessageBoxA
	ret

section .data
title: db 'MessageBox', 0
text: db 'Hello World!', 0
