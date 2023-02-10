#include "ActuatorRDV.h"
// using namespace aciones;
SoftwareSerial hc06(TX, RX); /* */
int actLen = 0;
struct ActPins
{
  uint8_t id;
  int py;
  int ps1;
  int ps2;
  bool used;
};
ActPins actpins[ACTPINSLEN];
void initActPins()
{
  
  for (int i = 0, k = 0; i < ACTPINSLEN; i++, k++)
  {
    actpins[i].id = i;
    actpins[i].used = false;
    actpins[i].py = solpins[i];
    actpins[i].ps1 = endcoursepins[k];
    actpins[i].ps2 = endcoursepins[k+1];
    Serial.println("Pino solenoide ["+String(i)+"]:"+String(actpins[i].py)+"; pinos s de c: S1 -> "+String(actpins[i].ps1)+" e  S2 -> "+String(actpins[i].ps2));
    ++k;
  }
}
ActPins *getActPins()
{
  ActPins *ptr = (ActPins *)0;
  for (int i = 0; i < ACTPINSLEN; i++)
  {
    if (!actpins[i].used)
    {
      ptr = &actpins[i];
      actpins[i].used = true;
      break;
    }
  }
  return ptr;
}
//*********************ACTUATOR********************//
void Actuator::setID(int i)
{
  this->id = i;
  this->timeronoff = 0;
}
void Actuator::setActuatorTriggerType(int tt)
{
  if (tt < NOTRIGGER || tt > TEN)
  {
    Serial.println("Não é um tipo de trigger.");
  }
  else
  {
    this->actuateTriggerType = tt;
  }
}
void Actuator::setActuatorTrigger(void *tr)
{
  this->actuateTrigger = tr;
}
void Actuator::setActuatorTrigger(int tr)
{
  this->actTrigCountTgt = tr;
}
void Actuator::actuate()
{
  
  Buttons *b;
  switch (this->actuateTriggerType)
  {
  case NOTRIGGER:
    break;
  case TCN:
    if (*(char *)this->actuateTrigger == ADVANCE)
    {
      this->advance();
    }
    else if (*(char *)this->actuateTrigger == RETREAT)
    {
      this->retreat();
    }
    else if (*(char *)this->actuateTrigger == TURNOFF)
    {
      this->turnAllOff();
    }
    break;
  case TSN:
    if (*(bool *)this->actuateTrigger)
    {
      this->advance();
    }
    else
    {
      this->retreat();
    }
    break;
  case TAN:
  case TRN:
    if (((*(int *)this->actuateTrigger) % this->actTrigCountTgt == 0))
    {
      this->advance();
    }
    else
    {
      this->retreat();
    }
    break;
  case TEN:
    b = (Buttons *)this->actuateTrigger;
    if (b->getState() == 1)
    {
      //Serial.println("Botao direto para o atuador pressionado!");
      this->advance();
    }
    else
    {
      this->retreat();
    }
    break;
  }

  this->timer();
  
  if(
     ((!getSetting(this->timeronoff, TIMERON))&&
     (!getSetting(this->timeronoff, TIMEROFF)))||
	  getSetting(this->timeronoff, AFTERON)
	  )
	  {
  	  	this->update();
	  }
  // this->timerOFF()
}
void Actuator::update()
{
  if(this->readS())
    digitalWrite(this->actps->py, this->y1);
}
void Actuator::turnAllOff()
{
  this->y1 = 0;
  this->y2 = 0;
  digitalWrite(this->actps->py, this->y1);
}
void Actuator::timer()
{
  Buttons *a;
  switch (this->timerTriggerType)
  {
  case NOTRIGGER:
    break;
  case TCN:
    if (*(char *)this->timerTrigger == ADVANCE)
    {
      this->timerIN = true;
      // delay(this->timerOndelay);
    }
    else
    {
      this->timerIN = false;
      // delay(this->timerOffdelay);
    }
    break;
  case TSN:
    if (*(bool *)this->timerTrigger)
    {
      this->timerIN = true;
      // delay(this->timerOndelay);
    }
    else
    {
      this->timerIN = false;
      // delay(this->timerOffdelay);
    }
    break;
  case TRN:
  case TAN:

    if ((*(int *)this->timerTrigger) % this->timerTrigCountTgt == 0)
    {
      Serial.println("TONNNNN!");
      Serial.println(*(int *)this->timerTrigger);
      Serial.println(this->timerTrigCountTgt);
      this->timerIN = true;
      // delay(this->timerOndelay);
    }
    else
    {
      Serial.println("TOFFFFF!");
      Serial.println(*(int *)this->timerTrigger);
      Serial.println(this->timerTrigCountTgt);
      this->timerIN = false;
      // delay(this->timerOffdelay);
    }
    break;
  case TEN:
    a = (Buttons *)this->timerTrigger;
    if (a->getState() == 1)
    {
  //      Serial.println("TONNNNN!");
  //      Serial.println((int)a);
      this->timerIN = true;
      // delay(this->timerOndelay);
    }
    else
    {
  //      Serial.println("TOFFFFF!");
  //      Serial.println((int)a);
      this->timerIN = false;
      // delay(this->timerOffdelay);
    }
    break;
  default:
    break;
  }
  /*******************Timer sem freeRTOS*********************/
  /********Comente este bloco caso for usar freeRTOS*********/
  if(this->timerIN||
    getSetting(this->timeronoff, TIMERON)||
    getSetting(this->timeronoff, TIMEROFF)||
    getSetting(this->timeronoff, AFTERON)
  )
    {
      if(this->timerOndelay>0&&
      (!getSetting(this->timeronoff, TIMEROFF)&&
      !getSetting(this->timeronoff, AFTERON))
      )
      {
        Serial.println("TON");
        
          if(!getSetting(this->timeronoff, TIMERON))
        {
          Serial.println("TON1");
            setSetting(this->timeronoff, TIMERON);
            this->beginON = millis();
        }
        
        if(getSetting(this->timeronoff, TIMERON)&&
          (millis()-this->beginON) >= this->timerOndelay
          )
        {
          Serial.println("TON2");
          unsetSetting(this->timeronoff, 	TIMERON);
          setSetting(this->timeronoff, AFTERON);
          Serial.println("Terminando TON");
          return ;
          //delay(5000);
        }
    }
    
      if(this->timerOffdelay>0)
      {
        Serial.println("TOFF");
      //após o TON desligar para iniciar TOFF
      if(getSetting(this->timeronoff, AFTERON))
      {
        Serial.println("TOFF1");
        unsetSetting(this->timeronoff, AFTERON);
        setSetting(this->timeronoff, TIMEROFF);
        this->beginOFF = millis();
      }
      
      if(getSetting(this->timeronoff, TIMEROFF)&&
        (!getSetting(this->timeronoff, TIMERON))&&
        millis()-this->beginOFF >= this->timerOffdelay
        )
      {
        Serial.println("TOFF2");
        unsetSetting(this->timeronoff, 	TIMEROFF);
        Serial.println("Terminando TOFF");
        //delay(5000);
      }
    }else 
    {
      unsetSetting(this->timeronoff, AFTERON);
    }
  }
  /********************Timer com o uso de freeRTOS********************/
  /**********descomente isso para usar o timer para freeRTOS**********/
  //  if (this->timerIN && this->timerOndelay > 0)
  //  {
  //    Serial.println("TONNNNN!");
  //    Serial.println(*(int *)this->timerTrigger);
  //    xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerOndelay));
  //  }
  //  else if (this->timerOffdelay > 0)
  //  {
  //    Serial.println("TOFFFFF!");
  //    Serial.println(*(int *)this->timerTrigger);
  //    xTaskDelayUntil(&this->xLastWakeTime, pdMS_TO_TICKS(this->timerOffdelay));
  //  }
}
void Actuator::setTimer(int ton, int toff)
{
  /**********Descomente quando for usar freeRTOS**********/
  //this->xLastWakeTime = xTaskGetTickCount();
  if (ton >= 0)
  {
    this->timerOndelay = ton;
  }
  if (toff >= 0)
  {
    this->timerOffdelay = toff;
  }
}
void Actuator::setTimerTriggerType(int tt)
{
  if (tt < NOTRIGGER || tt > TEN)
  {
    Serial.println("Não é um tipo de trigger.");
    hc06.write(ALERT);
  }
  else
  {
    this->timerTriggerType = tt;
  }
}
void Actuator::setTimerTrigger(void *trigger)
{
  // define o gatilho para ativar o timer
  // podendo ser os sensores de fim de curso
  // do próprio atuador, de outros atuadores
  // ou de um sensor externo
  this->timerTrigger = trigger;
}
void Actuator::setTimerTrigger(int trig)
{
  this->timerTrigCountTgt = trig;
}

char Actuator::getType()
{
  return this->type;
}
int Actuator::getID()
{
  return this->id;
}
//****
uint8_t Actuator::getPY()
{
  return this->actps->py;
}

bool Actuator::getY1()
{
  return this->y1;
}
bool Actuator::getY2()
{
  return this->y2;
}
bool *Actuator::giveY1()
{
  return &this->y1;
}
bool *Actuator::giveY2()
{
  return &this->y2;
}
//****
uint8_t Actuator::getPS1()
{
  return this->actps->ps1;
}
uint8_t Actuator::getPS2()
{
  return this->actps->ps2;
}
bool Actuator::getS1()
{
  return this->s1;
}
bool Actuator::getS2()
{
  return this->s2;
}
bool *Actuator::giveS1()
{
  return &this->s1;
}
bool *Actuator::giveS2()
{
  return &this->s2;
}
int Actuator::getTON()
{
  return this->timerOndelay;
}
int Actuator::getTOFF()
{
  return this->timerOffdelay;
}
int Actuator::getCounterA()
{
  return this->counterA;
}
int *Actuator::giveCounterA()
{
  return &this->counterA;
}
int Actuator::getCounterR()
{
  return this->counterR;
}
int *Actuator::giveCounterR()
{
  return &this->counterR;
}
//****
uint8_t Actuator::getActuatorTrigType()
{
  return this->actuateTriggerType;
}
uint8_t Actuator::getTimerTrigType()
{
  return this->timerTriggerType;
}
//****
Actuator *Actuator::giveThis()
{
  return this;
}
//****************RSPRING****************//
RSpring::RSpring()
{
}
RSpring::~RSpring()
{
  this->turnAllOff();
  this->actps->used = false;
}
void RSpring::setup(ActPins *p)
{
  this->actps = p;
  this->actps->used = true;
  this->type = 'M';
  // this->actps->used = true;
  //       this->py1 = py1; // pinos dos solenóides
  this->y1 = 0;
  this->y2 = 0; // estados dos solenóides
  //      this->ps1 = ps1; this->ps2 = ps2;// pinos dos indicadores de fim de curso
  this->s1 = 1;
  this->s2 = 0; // estados dos indicadores de fim curso
  // assumo que a posição inicial seja s1
  this->timerTriggerType = 0;
  this->timerOndelay = 0;
  this->timerOffdelay = 0;
  this->actuateTriggerType = 0;
  this->counterA = 0;
  this->counterR = 0;

  pinMode(this->actps->py, OUTPUT);
  pinMode(this->actps->ps1, INPUT);
  pinMode(this->actps->ps2, INPUT);
  this->turnAllOff();
  this->readS();
}
bool RSpring::readS()
{
  this->s1 = digitalRead(this->actps->ps1);
  this->s2 = digitalRead(this->actps->ps2);
  if ((this->s1 && this->s2) || ((!this->s1)&&(!this->s2)))
  {
    Serial.println("Há algo de errado com os sensores de fim de curso!\n S1(pino"+String(this->actps->ps1)+"): "+String(this->s1)+", S2(pino"+String(this->actps->ps2)+"): "+String(this->s2));
    hc06.write(ALERT);//"!"
    return false;
  }
  //contador
  if (/*this->s1*/ (this->s1) && this->counterA != this->counterR)
  { 
    //sequencia de acionamento dos sensores fim de curso 00,01,11,12...
    counterR++;
  }
  else if (/*this->s2*/ (this->s2) && this->counterA == this->counterR)
  {
    counterA++;
  }
  return true;
}
void RSpring::advance()
{
    //para concordar com o compartamento do relé o avanço e o retorno é invertido lógicamente
  
  if(this->s1){ hc06.write("1");this->y1 = 1;}//!(1);
}
void RSpring::retreat()
{
  
  if(this->s2){ hc06.write("0");this->y1 = 0;}//!(0);
}



//***********************DSOLENOID**********************//
DSolenoid::DSolenoid()
{
}
DSolenoid::~DSolenoid()
{
  this->turnAllOff();
  this->actps->used = false;
}
void DSolenoid::setup(ActPins *p)
{
  this->actps = p;
  this->type = 'V';
  this->actps->used = true;
  // this->actps->used = true;
  //       this->py1 = py1; this->py2 = py2;// pinos dos solenóides
  this->y1 = 0;
  this->y2 = 0; // estados dos solenóides
  //      this->ps1 = ps1; this->ps2 = ps2;// pinos dos indicadores de fim de curso
  this->s1 = 1;
  this->s2 = 0; // estados dos indicadores de fim curso
  // assumo que a posição inicial seja s1
  this->timerTriggerType = 0;
  this->timerOndelay = 0;
  this->timerOffdelay = 0;
  this->actuateTriggerType = 0;
  this->counterA = 0;
  this->counterR = 0;

  pinMode(this->actps->ps1, INPUT);
  pinMode(this->actps->ps2, INPUT);
  pinMode(this->actps->py, OUTPUT);
  this->turnAllOff();
  this->readS();
}
bool DSolenoid::readS()
{
  //      Serial.print("\ny1 = " + String(this->y1) + ", y2 = " + String(this->y2));
  //      Serial.print("\n Comando: " + String(*(char*)this->actuateTrigger));
  this->s1 = digitalRead(this->actps->ps1);
  this->s2 = digitalRead(this->actps->ps2);
  if ((digitalRead(this->actps->ps1) && digitalRead(this->actps->ps2)) || ((!digitalRead(this->actps->ps1))&&(!digitalRead(this->actps->ps2))))
  {
    Serial.println("Há algo de errado com os sensores de fim de curso!\n S1(pino"+String(this->actps->ps1)+"): "+String(this->s1)+", S2(pino"+String(this->actps->ps2)+"): "+String(this->s2));
    hc06.write(ALERT);
    return false;
  }
  //      if (this->s1 && this->s2) {
  //        Serial.println("Há algo de errado com os sensores de fim de curso!");
  //        hc06.write(ALERT);
  //      }
  if (/*this->s1*/ this->s1 && this->counterA != this->counterR)
  { // 00,01,11,12...
    counterR++;
  }
  if (/*this->s2*/ this->s2 && this->counterA == this->counterR)
  {
    counterA++;
  }
  return true;
}
void DSolenoid::advance()
{
  
  if(this->s1)
  {
    hc06.write("1");
    this->y1 = 1;
    this->y2 = 0;
  }
}
void DSolenoid::retreat()
{
  
  if(this->s2)
  {
    hc06.write("0");
    this->y1 = 0;
    this->y2 = 1;
  }
}


//*************************************************//

DSolenoid solbuffer[MAXACTS];
RSpring springbuffer[MAXACTS];
//task para o atuador que for adicionado
//case esteja usando freeRTOS
void actuatorTaskTemplate(void *vParameters)
{
  Actuator *act = ((Actuator *)vParameters);
  while (1)
  {
    act->actuate();
  }
}

Actuator *getActuatorByID(Actuator **acts, int actLen, int id)
{
  Actuator *ptr = (Actuator *)0;
  if ((id < actLen) && (id >= 0))
  {
    for (int i = 0; i < actLen; i++)
    {
      if (acts[i]->getID() == id)
      {
        ptr = acts[i];
        Serial.println("Atuador " + String(ptr->getID()) + " encontrado.");
      }
    }
  }
  else
  {
    Serial.println("Atuador " + String(id) + " não encontrado.");
  }
  return ptr;
}
/*Actuator**/ void getNewActuator(Actuator **acts, char t)
{
  //  Actuator* a = (Actuator*)0;
  //  bool ok = false;
  //  char taskName[13] = "Atuador ";
  //  while ((int)a==0) {
  //**** Se a quantidade de atuadores for menor que a quantidade m�xima
  //*******Adicione mais um atuador
  if (actLen < MAXACTS)
  {
    switch (t)
    {
    case TYPESPRING:
      springbuffer[actLen].setup(getActPins());
      springbuffer[actLen].setID(actLen);
      /*a*/ acts[actLen] = &springbuffer[actLen];
      actLen++;
      //ok = true;
      break;
    case TYPESOLENOID:
      solbuffer[actLen].setup(getActPins());
      solbuffer[actLen].setID(actLen);
      /*a*/ acts[actLen] = &solbuffer[actLen];
      actLen++;
      //ok = true;
      break;
    default:
      // a = NULL;
      break;
    }
  }
  else
  {
    hc06.write(ALERT);
  }
  
//  if (ok)
//  {
//    taskName[8] = (char)((acts[actLen]->getID() / 10) + '0');
//    taskName[9] = (char)((acts[actLen]->getID() - (10 * (acts[actLen]->getID() / 10))) + '0');
//
//    xTaskCreate(actuatorTaskTemplate, // Task function
//                taskName,             // Task name for humans
//                170,                  // Stack
//                acts[actLen],         // Task parameter
//                2,                    // Task priority
//                NULL);
//  }
  //  }

  //  return a;
}

void addNewActuator(Actuator **acts, char tps, /*int& dumLen,*/ /*void **actAddress,*/ /*int& actLen,*/ unsigned int& create)
{ //
  if (create /* && (dumLen > 0)*/)
  {
    //    for (int i = 0, k = dumLen; dumLen != 0; i++) {
    //    char taskName[20] = "Atuador ";
    //    acts[actLen] = getNewActuator(tps, actLen);//[tps[(dumLen - 1)] == '0' || tps[(dumLen - 1)] == 0 ? (dumLen - 2) : (dumLen - 1)]

    getNewActuator(acts, tps);
    unsetSetting(create, CREATE);

    // switch (tps[dumLen - 1]) {case 'm':Serial.println(actLen);acts[actLen] = new RSpring(getActPins());acts[actLen]->setup();acts[actLen]->setID(actLen + (dumLen - 1));actAdress[actLen] = (void *)acts[actLen]->giveThis();dumLen -= 1;actLen += 1;Serial.println("->M");break;case 'v':acts[actLen] = new DSolenoid(getActPins());acts[actLen]->setup();acts[actLen]->setID(actLen + (dumLen - 1));/*      if (acts[actLen]->getID() != actLen + (dumLen - 1)) {Serial.println("Erro!");acts[actLen] = actAdress[i];}*/actAdress[actLen] = (void *)acts[actLen]->giveThis();dumLen -= 1;actLen += 1;       Serial.println("->V");        break;      default:Serial.println("[ERRO!]Lista de dummies:");        for (int i = 0; i < dumLen; i++) {         Serial.print(tps[i]);      }       Serial.println();     break;}
  }

  //  for (int i = 0; i < actLen; i++) {
  //    if (((int)acts[i]->giveThis()) != ((int)actAddress[i])) {//checkup para verificar se há endereços embaralhados
  //      acts[i] = (Actuator*)actAddress[i];
  //    }
  //  }
  //  if (dumLen == 0) {
  //    create = false;
  //  }
}
void initActs(Actuator **a)
{
  for (int i = 0; i < MAXACTS; i++)
  {
    a[i] = &solbuffer[i];
  }
}
