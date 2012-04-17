//
//  ArduinoBridge.h
//  ArduinoBridge
//
//  Created by Akatsuka Daisuke on 12/03/15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef ArduinoBridge_ArduinoBridge_h
#define ArduinoBridge_ArduinoBridge_h

int openArduino(char* portname, char** error);

int digitalWrite(int pin, int value, char** error);
int digitalRead(int pin, int* valuePointer, char** error);

int analogWrite(int pin, int value, char** error);
int analoglRead(int pin, int* valuePointer, char** error);

//max ontime and offtime is 32766. unit is microseconds
int pulse(int pin, int ontime, int offtime, char** error);

int pinMode(int pin, bool isOutput, char** error);

int delayMicroseconds(int value, char** error);

int closeArduino();

int invoke(char* command, char* response, char** error);

#endif
