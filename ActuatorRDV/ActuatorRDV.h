#ifndef ACTUATORRDV_H
#define ACTUATORRDV_H
#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
  #else
  #include <WProgram.h>
#endif
#include <Wire.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
//#include <queue.h>
#include <SoftwareSerial.h>
#include <ButtonsAndStuff.h>
#include <TriggersAndCMs.h>
#include <CustomDigitalWiring.h>
#include <ActSettingsRDV.h>
#define TX 3
#define RX 2
//#define addNewActuator(acts, tps, dumLen, actLen, add) {  if (add) {    for (int i = 0, k = dumLen; i < k; i++) {      switch (tps[i]) {        case 'm': acts[actLen + i] = new RSpring(getActPins());          acts[actLen + i]->setup();acts[actLen + i]->setID(actLen+i);          Serial.println("->M");         break;        case 'v': acts[actLen + i] = new DSolenoid(getActPins());          acts[actLen + i]->setup(); acts[actLen + i]->setID(actLen+i);        Serial.println("->V");         break;      } dumLen -= 1;      actLen += 1;    } add = false;  }}
//

//using namespace my_execeptions;
extern SoftwareSerial hc06;
using namespace trs_and_cms;
//namespace aciones {
extern int actLen;
struct ActPins;
extern ActPins actpins[ACTPINSLEN];
void initActPins();
ActPins* getActPins();
//ActPins* getActPins(int i) {
//  ActPins* ptr;
//  --i;
//  if (!actpins[i].used && i < ACTPINSLEN) {
//    ptr = &actpins[i];
//  }
//  return ptr;
//}
class Actuator {
  public:
    virtual void setup(ActPins* p) = 0;
    virtual void advance() =  0;
    virtual void retreat() =   0;
    virtual bool readS() = 0;
    void setID(int i);
    void setActuatorTriggerType(int tt);
    void setActuatorTrigger(void *tr);
    void setActuatorTrigger(int tr);
    void actuate();
    void timer();
    void update();
    void turnAllOff();
    void setTimer(int ton, int toff);
    void setTimerTriggerType(int tt);
    void setTimerTrigger(void *trigger);
    void setTimerTrigger(int trigger);
    //****
    char      getType();
    int         getID();
    //****
    uint8_t    getPY();
    
    bool        getY1();
    bool        getY2();
    bool*      giveY1();
    bool*      giveY2();
    //****
    uint8_t    getPS1();
    uint8_t    getPS2();
    bool        getS1();
    bool        getS2();
    bool*      giveS1();
    bool*      giveS2();
    int        getTON();
    int       getTOFF();
    int   getCounterA();
    int* giveCounterA();
    int   getCounterR();
    int* giveCounterR();
    //****
    uint8_t getActuatorTrigType();
    uint8_t    getTimerTrigType();
    //****
    Actuator* giveThis();
    
    unsigned int timeronoff;
    
  protected:
    int id;//
    char type;
    int counterA, counterR; int actTrigCountTgt;
    ActPins* actps;
    bool y1, y2;
    //uint8_t ps1, ps2;  
    bool s1, s2;
	unsigned long beginON, beginOFF;
    unsigned int timerOndelay, timerOffdelay; 
	int timerTrigCountTgt; bool timerIN;
    //TickType_t xLastWakeTime;//descomente quando usar freeRTOS
    uint8_t timerTriggerType;  void *timerTrigger;
    uint8_t actuateTriggerType; void *actuateTrigger;
};

class RSpring : public Actuator {
  public:
    RSpring();
    ~RSpring();
    void setup(ActPins* p);
    bool readS();
    void advance();
    void retreat();
};

class DSolenoid : public Actuator {
  public:
    DSolenoid();
    ~DSolenoid();
    void setup(ActPins* p);
    bool readS();
    void advance();
    void retreat();
};


extern DSolenoid solbuffer[MAXACTS];
extern RSpring springbuffer[MAXACTS];

void actuatorTaskTemplate(void* vParameters);
Actuator* getActuatorByID(Actuator** acts, int actLen, int id);
/*Actuator**/void getNewActuator(Actuator** acts, char t);

void addNewActuator(Actuator** acts, char tps,/*int& dumLen,*/ /*void** actAddress,*/ /*int& actLen,*/ unsigned int& create) ;
void initActs(Actuator** a);
//}
#endif
