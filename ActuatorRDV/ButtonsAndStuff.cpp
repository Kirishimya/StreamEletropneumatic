#include "ButtonsAndStuff.h"

int btsLen=0;
Buttons bbuffer[MAXBUTTONS];

void Buttons::setID(int id) {
  this->id = id;
}
int Buttons::getID() {
  return this->id;
}
void Buttons::setState(uint8_t s) {
  this->state = s;
}
uint8_t Buttons::getState() {
  return (this->type == 1) ? (this->state) : (!this->state);
}
void Buttons::setType(uint8_t t) {
  this->type = t;
}
uint8_t Buttons::getType() {
  return this->type;
}

void createNewButton (uint8_t t) {
  Serial.println("bbbbb");
  
  bbuffer[btsLen].setID(btsLen);
  bbuffer[btsLen].setState(0);
  bbuffer[btsLen].setType(t);
//  baddresses[btsLen] = (void*)(&bbuffer[btsLen]);
//  if (btsLen >= 1) {
//    if (((int)&bbuffer[btsLen - 1]) != ((int)baddresses[btsLen - 1])) {
//      bbuffer[btsLen - 1] = *(Buttons*)baddresses[btsLen - 1];
//    }
//  }
  btsLen++;
  Serial.println("aaaaa");
}

Buttons* getButton(int id) {
  Buttons* bnull;
  if (btsLen>0) {
      bnull = &bbuffer[id];
    } else {
      bnull = (Buttons*)0;
    }
  return bnull;
}