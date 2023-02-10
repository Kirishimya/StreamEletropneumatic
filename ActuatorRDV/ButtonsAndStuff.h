#ifndef BUTTONSANDSTUFF_H
#define BUTTONSANDSTUFF_H
#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
  #else
  #include <WProgram.h>
#endif
#include "TriggersAndCMs.h"
using namespace trs_and_cms;
class Buttons {
  public:
    int id;
    uint8_t state;// same as output
    uint8_t type;//Type 1 == NO and type 0 == NC
	
    void setID(int id);
    int getID();
    void setState(uint8_t s);
    uint8_t getState();
    void setType(uint8_t t);
    uint8_t getType();
};
extern int btsLen;
extern Buttons bbuffer[MAXBUTTONS];
void createNewButton (uint8_t t);
Buttons* getButton(int id);

#endif
