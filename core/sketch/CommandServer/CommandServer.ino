/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

class Pulse {
  public:
    int pin;
    int ontime;
    int offtime;
};

template<typename Data>
class Vector {
   size_t d_size; // Stores no. of actually stored objects
   size_t d_capacity; // Stores allocated capacity
   Data *d_data; // Stores data
   public:
     Vector() : d_size(0), d_capacity(0), d_data(0) {}; // Default constructor
     Vector(Vector const &other) : d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) { d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); }; // Copy constuctor
     ~Vector() { free(d_data); }; // Destructor
     Vector &operator=(Vector const &other) { free(d_data); d_size = other.d_size; d_capacity = other.d_capacity; d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); return *this; }; // Needed for memory management
     void push_back(Data const &x) { if (d_capacity == d_size) resize(); d_data[d_size++] = x; }; // Adds new value. If needed, allocates more space
     size_t size() const { return d_size; }; // Size getter
     Data const &operator[](size_t idx) const { return d_data[idx]; }; // Const getter
     Data &operator[](size_t idx) { return d_data[idx]; }; // Changeable getter
   private:
     void resize() { d_capacity = d_capacity ? d_capacity*2 : 1; Data *newdata = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(newdata, d_data, d_size * sizeof(Data)); free(d_data); d_data = newdata; };// Allocates double the old space
}; 

Vector<Pulse*> pulse_list;

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
        boolean exist = false;        
        for (int i = 0, n = pulse_list.size(); i < n; i++) {
          Pulse* pulse = pulse_list[i];
          if (pulse->pin == pin) {
              pulse->ontime = ontime;
              pulse->offtime = offtime;
              exist = true;
          } 
        }
        if (exist == false) {
          Pulse* pulse = new Pulse();
          pulse->pin = pin;
          pulse->ontime = ontime;
          pulse->offtime = offtime;
          pulse_list.push_back(pulse);
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
  
  for (int i = 0, n = pulse_list.size(); i < n; i++) {
    Pulse* pulse = pulse_list[i];
    int pin = pulse->pin;
    int ontime = pulse->ontime;
    int offtime = pulse->offtime;
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
