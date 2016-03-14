/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <stdio.h>
/*
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/serial/ioss.h>
*/
#include <sys/ioctl.h>
//#include <Foundation/Foundation.h>

/***********************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
typedef int bool;
const int TRUE=1;
const int FALSE=0;
//#define NSLog(x,y)  { }
void NSLog(char*x, void*p)	{ fprintf(stderr, x, p); }
/***********************************/
#include "ArduinoBridge.h"


static int device_file_descriptor = -1;
static const int BUFFER_SIZE = 16;

void response(char* buffer) {
  char readBuffer[BUFFER_SIZE]; // buffer for holding incoming data
  // this will loop unitl the serial port closes
  while(TRUE) {
    // read() blocks until some data is available or the port is closed
    long numBytes = read(device_file_descriptor, readBuffer, BUFFER_SIZE); // read up to the size of the buffer
    if(numBytes <= 0) {
      break; // Stop the thread if there is an error
    } else if (readBuffer[numBytes-1] == '\r') {
      strncat(buffer, readBuffer, numBytes-1);
      break;
    } else {
      strncat(buffer, readBuffer, numBytes);
    }
  }
}

int invoke(char* command, char* buffer, char** error)
{
  //    NSLog(/*@*/"invoke: %s", command);
  long commandLength = strlen(command);
  long wrotenLength = write(device_file_descriptor, command, commandLength);
  if (commandLength != wrotenLength) {
    sprintf(*error, "Error: coudln't write successfully %ld/%ld", wrotenLength, commandLength);
    return -1;
  }

  response(buffer);

  return 0;
}

int openArduino(char* portname, char** error)
{
  if (device_file_descriptor != -1) {
    closeArduino();
  }
  fprintf(stderr,"enter openArduino\n"); //KK

  //    NSLog(/*@*/"openArduino:[%s]",portname);

  const char *bsdPath = portname;
  struct termios gOriginalTTYAttrs;
  speed_t baudRate = 9600;
  unsigned long mics = 3;

  int	serialFileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NDELAY );
  device_file_descriptor = serialFileDescriptor;
  // TIOCEXCL causes blocking of non-root processes on this serial-port
  if ( ioctl(serialFileDescriptor, TIOCEXCL) == -1) {
    *error = "Error: couldn't obtain lock on serial port";
    NSLog(/*@*/"%s", *error);
    return -1;
  }
  if ( fcntl(serialFileDescriptor, F_SETFL, 0) == -1) { 
    // clear the O_NONBLOCK flag; all calls from here on out are blocking for non-root processes
    *error = "Error: couldn't obtain lock on serial port";
    NSLog(/*@*/"%s", *error);
    return -1;
  }
  // Get the current options and save them so we can restore the default settings later.
  if ( tcgetattr(serialFileDescriptor, &gOriginalTTYAttrs) == -1) { 
    *error = "Error: couldn't get serial attributes";
    NSLog(/*@*/"%s", *error);
    return -1;
  }
  // copy the old termios settings into the current
  //   you want to do this so that you get all the control characters assigned
  struct termios options = gOriginalTTYAttrs;
  /*
     cfmakeraw(&options) is equivilent to:
     options->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
     options->c_oflag &= ~OPOST;
     options->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
     options->c_cflag &= ~(CSIZE | PARENB);
     options->c_cflag |= CS8;
     */
  cfmakeraw(&options);

  // set tty attributes (raw-mode in this case)
  if ( tcsetattr(serialFileDescriptor, TCSANOW, &options) == -1) {
    *error = "Error: couldn't get serial attributes";
    NSLog(/*@*/"%s", *error);
    return -1;
  }
  // Set baud rate (any arbitrary baud rate can be set this way)
  /***********************************************/
  //if ( ioctl(serialFileDescriptor, IOSSIOSPEED, &baudRate) == -1) { 
  //    *error = "Error: Baud Rate out of bounds";
  //    NSLog(/*@*/"%s", *error);
  //    return -1;
  //}
  // by KK
  if( cfsetspeed(&options, baudRate) == -1 ) {
    NSLog("Error: Baud Rate out of bounds","");
    return -1;
  }
  /***********************************************/
  // Set the receive latency (a.k.a. don't wait to buffer data)
  //if ( ioctl(serialFileDescriptor, IOSSDATALAT, &mics) == -1) { 
  //    *error = "Error: coudln't set serial latency";
  //    NSLog(/*@*/"%s", *error);
  //    return -1;
  //}

  //wait until Arduino finish the setup
  char buffer[BUFFER_SIZE];
  memset(buffer , '\0' , sizeof(buffer));
  response(buffer);

  return 0;
}

int digitalWrite(int pin, int value, char** error)
{
  char command[16];
  sprintf(command, "d%d,%d\r", pin, value);
  char response[BUFFER_SIZE];
  memset(response , '\0' , sizeof(response));
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"digitalWrite:");
  //    NSLog(/*@*/"[%s]", response);

  return result;
}

int digitalRead(int pin, int* valuePointer, char** error)
{
  char command[16];
  sprintf(command, "D%d\r", pin);
  char response[4];
  memset(response , '\0' , sizeof(response));
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"digitalRead:");
  //    NSLog(/*@*/"[%s]", response);

  *valuePointer = atoi(response);

  return result;
}

int analogWrite(int pin, int value, char** error)
{
  char command[16];
  sprintf(command, "a%d,%d\r", pin, value);
  char response[BUFFER_SIZE];
  memset(response , '\0' , sizeof(response));
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"analoglWrite:");
  //    NSLog(/*@*/"[%s]", response);

  return result;
}

int analogRead(int pin, int* valuePointer, char** error)
{
  char command[16];
  sprintf(command, "A%d\r", pin);
  char response[4];
  memset(response , '\0' , sizeof(response));
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"analogRead:");
  //    NSLog(/*@*/"[%s]", response);
  *valuePointer = atoi(response);

  return result;
}

int pulse(int pin, int ontime, int offtime, char** error)
{
  char command[24];
  sprintf(command, "P%d,%d,%d\r", pin, ontime, offtime);
  char response[10];
  memset(response , '\0' , sizeof(response));
  int result = invoke(command, response, error);
  //    NSLog(/*@*/"analogRead:");
  //    NSLog(/*@*/"[%s]", response);
  return result;
}

int pinMode(int pin, bool isOutput, char** error)
{
  char command[16];
  sprintf(command, "p%d,%d\r", pin, isOutput == TRUE ? 0 : 1);
  char response[BUFFER_SIZE];
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"pinMode:");
  //    NSLog(/*@*/"[%s]", response);

  return result;
}

int delayMicroseconds(int value, char** error)
{
  char command[16];
  sprintf(command, "w%d\r", value);
  char response[BUFFER_SIZE];
  int result = invoke(command, response, error);

  //    NSLog(/*@*/"delayMicroseconds:");
  //    NSLog(/*@*/"[%s]", response);

  return result;
}

int closeArduino()
{
  //    NSLog(/*@*/"closeArduino");
  close(device_file_descriptor);
  return 0;
}
