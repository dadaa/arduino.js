#include <stdio.h>
#include <windows.h>
#include "ArduinoBridge.h"

#ifdef DEBUG
void runlog(char* logstr){
	FILE* fp;
	fp = fopen("/tmp/arduino.js.log", "a");
	fprintf(fp, logstr);
	fclose(fp);
}
#define RUNLOG(x) runlog(x)
#else
#define RUNLOG(x)
#endif

HANDLE handle = NULL;
const int BUFFER_SIZE = 16;
DWORD errors;
COMSTAT status;

void response(char* buffer) {
RUNLOG("#### arduino.js # response start...\n");
	DWORD numBytes;
	ClearCommError(handle, &errors, &status);
RUNLOG("#### arduino.js # response before ReadFile\n");
RUNLOG(buffer);
	if(ReadFile(handle, buffer, BUFFER_SIZE, &numBytes, NULL) == 0){
RUNLOG("#### arduino.js # response FAILED ReadFile\n");
		ClearCommError(handle, &errors, &status);
		return;
	}
RUNLOG("#### arduino.js # response after ReadFile\n");
RUNLOG(buffer);

RUNLOG("#### arduino.js # response return...\n");
	return;
}

int invoke(char* command, char* buffer, char** error)
{
RUNLOG("#### arduino.js # invoke start...\n");
	DWORD wrotenLength;
	long commandLength = strlen(command);
	ClearCommError(handle, &errors, &status);
RUNLOG("#### arduino.js # invoke before WriteFile\n");
RUNLOG(command);
	if(WriteFile(handle, (void *)command, commandLength, &wrotenLength, NULL) == 0){
RUNLOG("#### arduino.js # invoke FAILED WriteFile\n");
		ClearCommError(handle, &errors, &status);
		return -1;
	}
RUNLOG("#### arduino.js # invoke after WriteFile\n");
RUNLOG(command);
#ifdef DEBUG
char s[1024];
sprintf(s, "commandLength = %d, wrotenLength = %d\n", commandLength, wrotenLength);
RUNLOG(s);
#endif
	if(commandLength != wrotenLength){
RUNLOG("#### arduino.js # invoke commandLength != wrotenLength\n");
		sprintf(*error, "Error: coudln't write successfully %ld/%ld", wrotenLength, commandLength);
RUNLOG(*error);
		return -1;
	}

RUNLOG("#### arduino.js # invoke before response\n");
    response(buffer);
RUNLOG("#### arduino.js # invoke after response\n");
    
RUNLOG("#### arduino.js # invoke return...\n");
    return 0;
}

int openArduino(char* portname, char** error)
{
RUNLOG("#### arduino.js # start openArduino...\n");
	DCB dcb = {0};
	COMMTIMEOUTS timeout;

	if(handle != NULL){
		closeArduino();
	}

	handle = CreateFile(portname
		, GENERIC_READ | GENERIC_WRITE
		, 0
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);
	if(handle == INVALID_HANDLE_VALUE){
		return -1;
	}

	GetCommState(handle, &dcb);
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity   = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	timeout.ReadIntervalTimeout         = 10;
	timeout.ReadTotalTimeoutMultiplier  = 1;
	timeout.ReadTotalTimeoutConstant    = 10;
	timeout.WriteTotalTimeoutMultiplier = 1;
	timeout.WriteTotalTimeoutConstant   = 10;
	if(SetCommState(handle, &dcb) == FALSE){
		return -1;
	}
	if(SetCommTimeouts(handle, &timeout) == FALSE){
		return -1;
	}
	Sleep(2000);

	//wait until Arduino finish the setup
	char buffer[BUFFER_SIZE];
	memset(buffer , '\0' , sizeof(buffer));
	response(buffer);

RUNLOG("#### arduino.js # return openArduino...\n");
	return 0;
}

int digitalWrite(int pin, int value, char** error)
{
RUNLOG("#### arduino.js # start digitalWrite...\n");
    char command[16];
    sprintf(command, "d%d,%d\r", pin, value);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);

//    NSLog(@"digitalWrite:");
//    NSLog(@"[%s]", response);

RUNLOG("#### arduino.js # return digitalWrite...\n");
    return result;
}

int digitalRead(int pin, int* valuePointer, char** error)
{
RUNLOG("#### arduino.js # start analogWrite...\n");
    char command[16];
    sprintf(command, "D%d\r", pin);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
    
//    NSLog(@"digitalRead:");
//    NSLog(@"[%s]", response);

    *valuePointer = atoi(response);
    
RUNLOG("#### arduino.js # return analogWrite...\n");
    return result;
}

int analogWrite(int pin, int value, char** error)
{
RUNLOG("#### arduino.js # start analogWrite...\n");
    char command[16];
    sprintf(command, "a%d,%d\r", pin, value);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
    
//    NSLog(@"analoglWrite:");
//    NSLog(@"[%s]", response);
    
RUNLOG("#### arduino.js # return analogWrite...\n");
    return result;
}

int analogRead(int pin, int* valuePointer, char** error)
{
RUNLOG("#### arduino.js # start analogRead...\n");
    char command[16];
    sprintf(command, "A%d\r", pin);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
    
//    NSLog(@"analogRead:");
//    NSLog(@"[%s]", response);
    *valuePointer = atoi(response);
    
RUNLOG("#### arduino.js # return analogRead...\n");
    return result;
}

int pulse(int pin, int ontime, int offtime, char** error)
{
RUNLOG("#### arduino.js # start pulse...\n");
    char command[24];
    sprintf(command, "P%d,%d,%d\r", pin, ontime, offtime);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
//    NSLog(@"analogRead:");
//    NSLog(@"[%s]", response);
RUNLOG("#### arduino.js # return pulse...\n");
    return result;
}

int pinMode(int pin, bool isOutput, char** error)
{
RUNLOG("#### arduino.js # start pinMode...\n");
    char command[16];
    sprintf(command, "p%d,%d\r", pin, isOutput == true ? 0 : 1);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
    
//    NSLog(@"pinMode:");
//    NSLog(@"[%s]", response);
    
RUNLOG("#### arduino.js # return pinMode...\n");
    return result;
}

int delayMicroseconds(int value, char** error)
{
RUNLOG("#### arduino.js # start delayMicroseconds...\n");
    char command[16];
    sprintf(command, "w%d\r", value);
RUNLOG(command);
    char response[BUFFER_SIZE];
    memset(response , '\0' , sizeof(response));
    int result = invoke(command, response, error);
    
//    NSLog(@"delayMicroseconds:");
//    NSLog(@"[%s]", response);
    
RUNLOG("#### arduino.js # return delayMicroseconds...\n");
    return result;
}

int closeArduino()
{
RUNLOG("#### arduino.js # start closeArduino...\n");
	SetCommMask(handle, EV_RXCHAR);
	PurgeComm(handle, PURGE_RXCLEAR);
	CloseHandle(handle);
RUNLOG("#### arduino.js # return closeArduino...\n");
	return 0;
}
