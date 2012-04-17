/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

void setup() {
  Serial.begin(9600);
  Serial.print('\r');
}

void loop() {
  if (Serial.available()) {
      char command = Serial.read();
      switch (command) {
        case 'd': {
          int pin = readInt(2);
          int value = readInt(1);
          digitalWrite(pin, value);
          break;
        }
        case 'D': {
          int pin = readInt(2);
          int result = digitalRead(pin);
          Serial.print(result);
          break;
        }
        case 'a': {
          int pin = readInt(2);
          int value = readInt(3);
          analogWrite(pin, value);
          break;
        }
        case 'A': {
          int pin = readInt(2);
          int result = analogRead(pin);
          Serial.print(result);
          break;
        }
        case 'p': {
          int pin = readInt(2);
          int value = readInt(1);
          if (value == 0) {
            pinMode(pin, OUTPUT);
          } else {
            pinMode(pin, INPUT);
          }
          break;
        }
        case 'P': {
          int pin = readInt(2);
          int ontime = readInt(5);
          int offtime = readInt(5);
          digitalWrite(pin, HIGH);
          if (16382 < ontime) {
            delayMicroseconds(ontime-16382);
            delayMicroseconds(16382);
          } else {
            delayMicroseconds(ontime);
          }
          digitalWrite(pin, LOW);
          if (16382 < offtime) {
            delayMicroseconds(offtime-16382);
            delayMicroseconds(16382);
          } else {
            delayMicroseconds(offtime);
          }
          break;
        }
        case 'w': {
          int value = readInt(6);
          delayMicroseconds(value);
          break;
        }
     }
     Serial.print('\r');
  }
}

int readInt(int maxsize) {
  char chars[maxsize+1];
  for (int i = 0;;) {
    if (! Serial.available() ) {
      continue;
    }
    char c = Serial.read();
    if (c == ',' || c == '\r') {
      break;
    }
    chars[i] = c;
    i += 1;
    chars[i] = '\0';
  }
  return atoi(chars);
}
