// ropcheck.cpp
//

#include <stdio.h>
#include <windows.h>

#include "ropsettings.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// shows a message box to the user informing him of possible attack
// the user can either terminate the process or continue the execution normally
void ReportPossibleROP(string &report)
{
	string messageboxtext;
	messageboxtext = ""
		"ROPGuard has detected a possible threat.\n"
		"Problem details:\n\n" + report;
	
	WriteLog((char *)messageboxtext.c_str());
	
	if(MessageBoxA(GetForegroundWindow(), 
		messageboxtext.c_str(), "ROPGuard", MB_OKCANCEL) == IDOK)
	{
		ExitProcess(1);
	}
	
}

int PrecededByCall(unsigned char *address)
{
	// check for call opcodes
	if(*(address-5) == 0xE8)
		return 1;
	if(*(address-3) == 0xE8)
		return 1;

	// indirect call
	for(int i=2; i < 8; i++){
		if((*(address-i) == 0xFF) && (((*(address-i+1)) & 0x38) == 0x10))
			return 1;
	}

	return 0;
}

// performs checks on the return address of the critical function
int CheckReturnAddress(DWORD returnAddress, DWORD functionAddress, DWORD *registers)
{
	// is return address preceded by call?
	if(!PrecededByCall((unsigned char *)returnAddress)){
		stringstream errorreport;
		errorreport << "Return address not preceded by call.\n";
		errorreport << "Return address: " << std::hex << returnAddress;
		ReportPossibleROP(errorreport.str());
		return 0;
	}

	return 1;
}

// checks if the address of protected function is on the stack just above the current return address
// if it is, this could mean that we "returned into" the beginning og critical function instead of calling it
int CheckFunctionAddressOnStack(unsigned long functionAddress, unsigned long *stack)
{
	int n = (GetROPSettings()->preserveStack) / sizeof(unsigned long);
	for(int i=0; i < n; i++){
		if((stack[i] ^ ADDR_SCRAMBLE_KEY) == functionAddress){
			stringstream errorreport;
			errorreport << "Address of critical function found on stack.\n";
			errorreport << "Stack address: " << std::hex << (unsigned long)stack;
			errorreport << ", Function address: " << (functionAddress ^ ADDR_SCRAMBLE_KEY);
			ReportPossibleROP(errorreport.str());
			return 0;	
		}
	}
	return 1;
}

// gets the top and bottom address of the stack
void GetStackInfo(unsigned long *stackBottom, unsigned long *stackTop)
{
	char *TIB = (char *)__readfsdword(0x18);
	*stackTop = *((unsigned long *)(TIB+4));
	*stackBottom = *((unsigned long *)(TIB+8));
}

// performs the checks on the stack frames below the frame of the critical function
int CheckStackFrames(DWORD *stackPtr, DWORD *framePtr)
{
	DWORD *originalFramePtr;

	unsigned long stackBottom, stackTop;
	GetStackInfo(&stackBottom, &stackTop);

	originalFramePtr = framePtr;

	// frame pointer must point to the stack
	if(((unsigned long)framePtr < stackBottom) || (stackTop < (unsigned long)framePtr)){
		stringstream errorreport;
		errorreport << "Return address not preceded by call.\n";
		errorreport << "Frame pointer: ";
		errorreport << std::hex << (unsigned long)framePtr;
		ReportPossibleROP(errorreport.str());
		return 0;
	}

	// frame pointer must be "below" the stack pointer
	if(((unsigned long)framePtr) < ((unsigned long)stackPtr)){
		stringstream errorreport;
		errorreport << "Frame pointer is above stack pointer on stack\n";
		errorreport << "Stack pointer: " << std::hex << (unsigned long)stackPtr;
		errorreport << ", Frame pointer: " << std::hex << (unsigned long)framePtr;
		ReportPossibleROP(errorreport.str());
		return 0;
	}

	return 1;
}

// indicates that the checks should be performed
bool protectionEnabled;

// the main function that performs the check, called in the prologue of every critical function
//   functionAddress: the original address of the critical function, used to determine what function are we in
//   registers:       an array containing the register values in the moment of critical function call
void __stdcall ROPCheck(unsigned long functionAddress, unsigned long *registers)
{
	if(!protectionEnabled)
		return; // still initializing protection

	unsigned long framePointer = registers[2];
	unsigned long stackPointer = registers[3];

	// identify the function that is being called
	int i;
	int numFunctions = GetNumGuardedFunctions();
	ROPGuardedFunction *guardedFunctions = GetGuardedFunctions();
	ROPGuardedFunction *currentFunction = NULL;
	for(i=0; i < numFunctions; i++){
		if(guardedFunctions[i].originalAddress == functionAddress){
			currentFunction = &(guardedFunctions[i]);
			break;
		}
	}
	if(!currentFunction){
		WriteLog("Inside ROPCheck, but guarded function not identified");
		return;
	}

	//if(strcmp(currentFunction->functionName, "CreateProcessInternalW") == 0)
	//	return;
	
	// check critical function on stack 
	if(!CheckFunctionAddressOnStack(functionAddress, (unsigned long *)stackPointer))
		return;

	// check return address
	DWORD returnAddress = *((DWORD *)(stackPointer + GetROPSettings()->preserveStack));
	if(!CheckReturnAddress(returnAddress, functionAddress, registers))
		return;
	
	// check ebp
	if(!CheckStackFrames((DWORD *)(stackPointer + GetROPSettings()->preserveStack), (DWORD *)framePointer))
		return;

	return;
}
