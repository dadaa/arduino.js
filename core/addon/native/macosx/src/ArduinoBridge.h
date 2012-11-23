/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

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
