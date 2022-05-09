#ifndef ACTUATOR_H
#define ACTUATOR_H
#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include "MyExceptions.h"
#include "TriggersAndCMs.h"
#define ACTPINSLEN 5
//#define ADDNEWACTS(acts, tps, dumLen, actLen, add) {  if (add) {    for (int i = 0, k = dumLen; i < k; i++) {      switch (tps[i]) {        case 'm': acts[actLen + i] = new RSpring(getActPins());          acts[actLen + i]->setup();acts[actLen + i]->setID(actLen+i);          Serial.println("->M");         break;        case 'v': acts[actLen + i] = new DSolenoid(getActPins());          acts[actLen + i]->setup(); acts[actLen + i]->setID(actLen+i);        Serial.println("->V");         break;      } dumLen -= 1;      actLen += 1;    } add = false;  }}
//

//using namespace my_execeptions;
using namespace trs_and_cms;
SoftwareSerial hc06(10, 11);/* RX pino 11 , TX pino 10 */


namespace aciones {
struct ActPins {
  uint8_t id;
  uint8_t py1; uint8_t py2;
  uint8_t ps1; uint8_t ps2;
  bool used;
};
ActPins actpins[ACTPINSLEN];
void initActPins() {
  for (int i = 0, k = 22; i < ACTPINSLEN; i++) {
    actpins[i].id = i;
    actpins[i].used = false;
    actpins[i].py1 = k++;
    actpins[i].py2 = k++;
    actpins[i].ps1 = k++;
    actpins[i].ps2 = k++;
  }
}
ActPins* getActPins() {
  ActPins* ptr;
  for (int i = 0; i < ACTPINSLEN; i++) {
    if (!actpins[i].used) {
      ptr = &actpins[i];
      actpins[i].used = true;
      break;
    }
  }
  return ptr;
}
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
    virtual void setup() = 0;
    virtual void advance() =  0;
    virtual void retreat() =   0;
    virtual void turnAllOff() = 0;
    virtual void update() =   0;
    virtual void readS() = 0;
    void setID(int i);
    void setActuatorTriggerType(uint8_t tt);
    void setActuatorTrigger(void *tr);
    void setActuatorTrigger(int tr);
    void actuate();
    void timer();
    void setTimer(int ton, int toff);
    void setTimerTriggerType(uint8_t tt);
    void setTimerTrigger(void *trigger);
    void setTimerTrigger(int trigger);
    //****
    char      getType();
    int         getID();
    //****
    uint8_t    getPY1();
    uint8_t    getPY2();
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
  protected:
    int id;//
    char type;
    int counterA, counterR; int actTrigCountTgt;
    ActPins* actps;
    bool y1, y2;
    uint8_t ps1, ps2;
    bool s1, s2;
    int timerOndelay, timerOffdelay; int timerTrigCountTgt; bool timerIN;
    TickType_t xLastWakeTime;
    uint8_t timerTriggerType;  void *timerTrigger;
    uint8_t actuateTriggerType; void *actuateTrigger;
};
void Actuator::setID(int i) {
  this->id = i;
}
void Actuator::setActuatorTriggerType(uint8_t tt) {
  if (tt < NOTRIGGER || tt > TEN) {
    Serial.println("Não é um tipo de trigger.");
  } else {
    this->actuateTriggerType = tt;
  }
}
void Actuator::setActuatorTrigger(void *tr) {
  this->actuateTrigger = tr;
}
void Actuator::setActuatorTrigger(int tr) {
  this->actTrigCountTgt = tr;
}
void Actuator::actuate() {
  switch (this->actuateTriggerType) {
    case NOTRIGGER:
      break;
    case TCN:
      if (*(char*)this->actuateTrigger == ADVANCE) {
        this->advance();
      } else if (*(char*)this->actuateTrigger == RETREAT) {
        this->retreat();
      } else if (*(char*)this->actuateTrigger == TURNOFF) {
        this->turnAllOff();
      }
      break;
    case TSN:
      if (*(bool*)this->actuateTrigger) {
        this->advance();
      } else {
        this->retreat();
      }
      break;
    case TAN:
    case TRN:
      if ((*(int*)this->actuateTrigger) % this->actTrigCountTgt == 0) {
        this->advance();
      } else {
        this->retreat();
      }
      break;
  }
  //Serial.println(this->counterA);
  //this->timerON();
  this->timer();
  this->update();
  //this->timerOFF()
}
void Actuator::timer() {
  switch (this->timerTriggerType) {
    case NOTRIGGER: break;
    case TCN:
      if (*(char*)this->timerTrigger == ADVANCE) {
        //xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerdelay));
        delay(this->timerOndelay);
      } else {
        delay(this->timerOffdelay);
      }
      break;
    case TSN:
      if (*(bool*)this->timerTrigger) {
        //xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerdelay));
        delay(this->timerOndelay);
      } else {
        //xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerdelay));
        delay(this->timerOffdelay);
      }
      break;
    case TRN:
    case TAN:
      if (*(int*)this->timerTrigger % this->timerTrigCountTgt == 0) {
        //xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerdelay));
        delay(this->timerOndelay);
      } else {
        //xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerdelay));
        delay(this->timerOffdelay);
      }
      break;
    default:
      break;
  }
}
void Actuator::setTimer(int ton, int toff) {
  this->xLastWakeTime = xTaskGetTickCount ();
  if (ton >= 0) {
    this->timerOndelay = ton;
  }
  if (toff >= 0) {
    this->timerOffdelay = toff;
  }
}
void Actuator::setTimerTriggerType(uint8_t tt) {
  if (tt < NOTRIGGER || tt > TEN) {
    Serial.println("Não é um tipo de trigger.");
    hc06.write(ALERT);
  } else {
    this->timerTriggerType = tt;
  }
}
void Actuator::setTimerTrigger(void *trigger) {
  //define o gatilho para ativar o timer
  //podendo ser os sensores de fim de curso
  //do próprio atuador, de outros atuadores
  //ou de um sensor externo
  this->timerTrigger = trigger;
}
void Actuator::setTimerTrigger(int trigger) {
  this->timerTrigCountTgt = trigger;
}

char Actuator::getType() {
  return this->type;
}
int Actuator::getID  () {
  return this->id;
}
//****
uint8_t Actuator::getPY1  () {
  return this->actps->py1;
}
uint8_t Actuator::getPY2 () {
  return this->actps->py2;
}
bool Actuator::getY1  () {
  return this->y1;
}
bool Actuator::getY2 () {
  return this->y2;
}
bool* Actuator::giveY1  () {
  return &this->y1;
}
bool* Actuator::giveY2  () {
  return &this->y2;
}
//****
uint8_t Actuator::getPS1 () {
  return this->actps->ps1;
}
uint8_t Actuator::getPS2 () {
  return this->actps->ps2;
}
bool Actuator::getS1 () {
  return this->s1;
}
bool Actuator::getS2 () {
  return this->s2;
}
bool* Actuator::giveS1 () {
  return &this->s1;
}
bool* Actuator::giveS2 () {
  return &this->s2;
}
int Actuator::getTON () {
  return this->timerOndelay;
}
int Actuator::getTOFF () {
  return this->timerOffdelay;
}
int Actuator::getCounterA() {
  return this->counterA;
}
int* Actuator::giveCounterA() {
  return &this->counterA;
}
int Actuator::getCounterR() {
  return this->counterR;
}
int* Actuator::giveCounterR() {
  return &this->counterR;
}
//****
uint8_t Actuator::getActuatorTrigType  () {
  return this->actuateTriggerType;
}
uint8_t Actuator::getTimerTrigType() {
  return this->timerTriggerType;
}
//****
Actuator* Actuator::giveThis() {
  return this;
}
class RSpring : public Actuator {
  public:
    RSpring(ActPins* p);
    ~RSpring();
    void setup();
    void readS();
    void advance();
    void retreat();
    void turnAllOff();
    void update();
};
RSpring::RSpring(ActPins* p) {
  this->actps = p;
  this->type = 'M';
  //this->actps->used = true;
  //      this->py1 = py1; // pinos dos solenóides
  this->y1 = 0; this->y2 = 0;      // estados dos solenóides
  //      this->ps1 = ps1; this->ps2 = ps2;// pinos dos indicadores de fim de curso
  this->s1 = 1; this->s2 = 0;      // estados dos indicadores de fim curso
  // assumo que a posição inicial seja s1
  this->timerTriggerType = 0;
  this->timerOndelay       = 0;
  this->timerOffdelay      = 0;
  this->actuateTriggerType = 0;
  this->counterA = 0; this->counterR = 0;
}
RSpring::~RSpring() {
  this->turnAllOff();
  this->actps->used = false;
}
void RSpring::setup() {
  pinMode(this->actps->py1, OUTPUT);
  pinMode(this->actps->ps1, INPUT);
  pinMode(this->actps->ps2, INPUT);
  this->readS();
}
void RSpring::readS() {
  this->s1 = digitalRead(this->actps->ps1);
  this->s2 = digitalRead(this->actps->ps2);
  if (this->s1 && this->s2) {
    Serial.println("Há algo de errado com os sensores de fim de curso!");
    hc06.write(ALERT);
  }
  if (/*this->s1*/!(this->y1) && this->counterA != this->counterR) { //00,01,11,12...
    counterR++;
  }
  else if (/*this->s2*/(this->y1) && this->counterA == this->counterR) {
    counterA++;
  }
}
void RSpring::advance() {
  this->y1 = 1;
}
void RSpring::retreat() {
  this->y1 = 0;
}
void RSpring::turnAllOff() {
  this->y1 = 0;
  digitalWrite(this->actps->py1, this->y1);
}
void RSpring::update() {
  this->readS();
  digitalWrite(this->actps->py1, this->y1);
}
class DSolenoid : public Actuator {
  public:
    DSolenoid(ActPins* p);
    ~DSolenoid();
    void setup();
    void readS();
    void advance();
    void retreat();
    void turnAllOff();
    void update();
};
DSolenoid::DSolenoid(ActPins* p) {
  this->actps = p;
  this->type = 'V';
  //this->actps->used = true;
  //      this->py1 = py1; this->py2 = py2;// pinos dos solenóides
  this->y1 = 0; this->y2 = 0;      // estados dos solenóides
  //      this->ps1 = ps1; this->ps2 = ps2;// pinos dos indicadores de fim de curso
  this->s1 = 1; this->s2 = 0;      // estados dos indicadores de fim curso
  // assumo que a posição inicial seja s1
  this->timerTriggerType = 0;
  this->timerOndelay       = 0;
  this->timerOffdelay      = 0;
  this->actuateTriggerType = 0;
  this->counterA = 0; this->counterR = 0;
}
DSolenoid::~DSolenoid() {
  this->turnAllOff();
  this->actps->used = false;
}

void DSolenoid::setup() {
  pinMode(this->actps->ps1, INPUT);
  pinMode(this->actps->ps2, INPUT);
  pinMode(this->actps->py1, OUTPUT);
  pinMode(this->actps->py2, OUTPUT);
  this->readS();
}
void DSolenoid::readS() {
  //      Serial.print("\ny1 = " + String(this->y1) + ", y2 = " + String(this->y2));
  //      Serial.print("\n Comando: " + String(*(char*)this->actuateTrigger));
  this->s1 = digitalRead(this->actps->ps1);
  this->s2 = digitalRead(this->actps->ps2);
  //      if (this->s1 && this->s2) {
  //        Serial.println("Há algo de errado com os sensores de fim de curso!");
  //        hc06.write(ALERT);
  //      }
  if (/*this->s1*/this->y2 && this->counterA != this->counterR) { //00,01,11,12...
    counterR++;
  }
  if (/*this->s2*/this->y1 && this->counterA == this->counterR) {
    counterA++;
  }
}
void DSolenoid::advance() {
  this->y1 = 1;
  this->y2 = 0;
}
void DSolenoid::retreat() {
  this->y1 = 0;
  this->y2 = 1;
}
void DSolenoid::turnAllOff() {
  this->y1 = 0;
  this->y2 = 0;
  digitalWrite(this->actps->py1, this->y1);
  digitalWrite(this->actps->py2, this->y2);
}
void DSolenoid::update() {
  this->readS();
  digitalWrite(this->actps->py1, this->y1);
  digitalWrite(this->actps->py2, this->y2);
}
Actuator* getActuatorByID(Actuator** acts, int actLen, int id) {
  Actuator* ptr;
  if ((id < actLen) && (id >= 0)) {
    for (int i = 0; i < actLen; i++) {
      if (acts[i]->getID() == id) {
        ptr = acts[i];
        Serial.println("Atuador " + String(ptr->getID()) + " encontrado.");
      }
    }
  } else {
    Serial.println("Atuador " + String(id) + " não encontrado.");
    ptr = NULL;
  }
  return ptr;
}
Actuator* getNewActuator(char t) {
  Actuator* a;
  switch (t) {
    case TYPESPRING:
      a = new RSpring(getActPins());
      break;
    case TYPESOLENOID:
      a = new DSolenoid(getActPins());
      break;
    default:
      a = NULL;
      break;
  }
  if (((int)a) == 0) {
    a = NULL;
  }
  return a;
}
void ADDNEWACTS(Actuator** acts, char tps[], int& dumLen, void** actAddress, int& actLen, bool& create) {             //
  if (create && (dumLen > 0)) {
    //for (int i = 0, k = dumLen; dumLen != 0; i++) {
    acts[actLen] = getNewActuator(tps[tps[(dumLen - 1)] == '0' || tps[(dumLen - 1)] == 0 ? (dumLen - 2) : (dumLen - 1)]);
    if (acts[actLen] != NULL || ((int)acts[actLen]) != 0) {
      acts[actLen]->setup();
      acts[actLen]->setID(actLen + (dumLen - 1));
      actAddress[actLen] = (void *)acts[actLen]->giveThis();
      dumLen -= tps[(dumLen - 1)] == '0' || tps[(dumLen - 1)] == 0 ? 2 : 1;
      actLen += 1;
    }
    //switch (tps[dumLen - 1]) {case 'm':Serial.println(actLen);acts[actLen] = new RSpring(getActPins());acts[actLen]->setup();acts[actLen]->setID(actLen + (dumLen - 1));actAdress[actLen] = (void *)acts[actLen]->giveThis();dumLen -= 1;actLen += 1;Serial.println("->M");break;case 'v':acts[actLen] = new DSolenoid(getActPins());acts[actLen]->setup();acts[actLen]->setID(actLen + (dumLen - 1));/*      if (acts[actLen]->getID() != actLen + (dumLen - 1)) {Serial.println("Erro!");acts[actLen] = actAdress[i];}*/actAdress[actLen] = (void *)acts[actLen]->giveThis();dumLen -= 1;actLen += 1;       Serial.println("->V");        break;      default:Serial.println("[ERRO!]Lista de dummies:");        for (int i = 0; i < dumLen; i++) {         Serial.print(tps[i]);      }       Serial.println();     break;}
  }
  for (int i = 0; i < actLen; i++) {
    if (((int)acts[i]->giveThis()) != ((int)actAddress[i])) {//checkup para verificar se há endereços embaralhados
      acts[i] = (Actuator*)actAddress[i];
    }
  }
  if (dumLen == 0) {
    create = false;
  }
}
}
#endif
