#include "Actuator.h"
using namespace aciones;

int actLen = 0, dumLen = 0; char dummies[] = {};
void* actAddress[10];
Actuator** actuators;
Actuator* actuator; /*= new Actuator(5, 6, 9, 8);*/

struct Settings {
  bool setting;
  bool advance;
  bool retreat;
  bool timer;
  bool value;
  bool actuator;
  bool trigger;
  bool trigtype;
  bool selected;
  bool rspring;
  bool rsolenoid;
  bool readingN;
  bool shifting;
  bool create;
} actSettings;

char l = 'f';
int sid = 0;
//SoftwareSerial hc06(10, 11);/* RX pino 11 , TX pino 10 */

void setup() {
  actuators = new Actuator*[10];
  initSettings();
  initActPins();
  for (int i = 0; i < 1; i++) {
    actuators[i] = new DSolenoid(getActPins()); actuators[i]->setID(i);
    actuators[i]->setup();
    actuators[i]->setActuatorTriggerType(TCN);//tcc=1
    actuators[i]->setActuatorTrigger(&l);
    actuators[i]->actuate();
    actAddress[i] = (void *)actuators[i]->giveThis();
    actLen += 1;
  }
  actuator = actuators[0]->giveThis();
  actSettings.selected = true;
  //actLen = 2;
  Serial.begin(115200);
  while (!Serial) {
  }

  Serial.println("ENTER AT Commands:");
  //Initialize Bluetooth Serial Port
  hc06.begin(9600);
}
unsigned long bfore = 0;
void xactSettings(void* vParameter = NULL);
void loop() {
  //Write data from HC06 to Serial Monitor
  //+++++++++código para o acionamento de um único atuador
  readData(0);

  selectActuator(0);
  xactSettings();
  ADDNEWACTS(actuators, dummies, dumLen, actAddress, actLen, actSettings.create);
  //teste individual

  if (millis() - bfore > 3000UL) {
    Serial.println("quantidade de atuadores: " + String(actLen));
    Serial.println("quantidade de atuadores dummies: " + String(dumLen));
    Serial.println("Lista de atuadores:");
    for (int i = 0; i < actLen; i++) {
      Serial.print((int)actAddress[i]);
      Serial.print(" ");
      //      if (actuators[0]->giveThis() != (Actuator*)actAddress[0]) {
      //        actuators[0] = (Actuator*)actAdress[0];
      //      }
    }
    Serial.println();
    Serial.println("Lista de dummies:");
    for (int i = 0; i < dumLen; i++) {
      Serial.print(dummies[i]);
      Serial.print(" ");
    }
    Serial.println("\nControlando atuador #" + String((!(actSettings.selected)) ? 99 : actuator->getID()));
    Serial.println("Estado dos sensores: {S1: " + String((!(actSettings.selected)) ? 99 : actuator->getS1()) + ", S2: " + String(!(actSettings.selected) ? 99 : actuator->getS2()) + "}");
    Serial.println("Contadores: {A: " + String((!(actSettings.selected)) ? 99 : actuator->getCounterA()) + ", R: " + String(!(actSettings.selected) ? 99 : actuator->getCounterR()) + "}");
    Serial.println("Tipo de gatilho de acionamento: " + String((!(actSettings.selected)) ? 99 : actuator->getActuatorTrigType()));
    Serial.println("Estado das solenoids: {S1: " + String((!(actSettings.selected)) ? 99 : (actuator->getY1())) + ", S2: " + String(!(actSettings.selected) ? 99 : actuator->getY2()) + "}");
    Serial.println("Tipo de gatilho do timer: " + String((!(actSettings.selected)) ? 99 : actuator->getTimerTrigType()));
    Serial.println("Valores do timer: {TON: " + String((!(actSettings.selected)) ? 99 : actuator->getTON()) + ", TOFF: " + String(!(actSettings.selected) ? 99 : actuator->getTOFF ()) + "}");

    bfore = millis();
  }
  if (actSettings.selected) {
    //for (int i = 0; i < actLen; i++) {
    //if (actuators[i]->getActuatorTrigType() != NOTRIGGER){
    //Serial.println("qua");
    actuator->actuate();
    //}
    //}
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //Write from Serial Monitor to HC06
  //  if (Serial.available()) {
  //   hc06.write(Serial.read());
  //  }
}
void initSettings() {
  actSettings.setting = false;
  actSettings.advance = false;
  actSettings.retreat = false;
  actSettings.timer = false;
  actSettings.value = false;
  actSettings.actuator = false;
  actSettings.trigger = false;
  actSettings.trigtype = false;
  actSettings.selected = false;
  actSettings.rspring = false;
  actSettings.rsolenoid = false;
  actSettings.readingN = false;
  actSettings.shifting = false;
  actSettings.create = false;
}//
void readData(void* vParameters) {
  //  while (1) {
  if (hc06.available() > 0) {
    char temp = hc06.read();
    if (temp != ' ' ) {
      l = temp;
      Serial.println("canal Bluetooth com data");
    }
  } else if (Serial.available() > 0) {
    char temp = Serial.read();
    if (temp != ' ' ) {
      l = temp;
      Serial.println("canal Bluetooth com data");
    }
  }
  //  }
}
void createActuators() {
  //while(1){
  char type;
  int num;
  if (((actSettings.rsolenoid || l == RSOLENOID) || (actSettings.rspring || l == RSPRING)) && !(actSettings.create)) {
    switch (l) {
      case RSPRING: type = TYPESPRING; actSettings.rspring = true; break;
      case RSOLENOID: type = TYPESOLENOID; actSettings.rsolenoid = true; break;
      default: break;
    }
    //actSettings.rsolenoid = true;
    actSettings.selected = false;
    //Serial.println("adcionando novo(s) atuadore(s) do tipo dupla solenoid...");
    readINTEn();
    int num = getNum();
    Serial.println("adcionando " + String(num) + " novo(s) atuador(s) do tipo " + (type == 'v' ? ("solenoid") : ("retorno por mola")) + "...");
    for (int i = 0; i < num; i++) {
      dummies[dumLen + i] = type;
    }
    dumLen += num;
    switch (type) {
      case TYPESPRING: actSettings.rspring = false; Serial.println("MOff..."); break;
      case TYPESOLENOID: actSettings.rsolenoid = false; Serial.println("VOff..."); break;
      default: break;
    }
  }
  //while(l!=DONE&&l!=RESET)
  if (l == DONE) {
    actSettings.create = true;
    actSettings.setting = false;
  }
  if (l == RESET) {
    Serial.println("Here we go again!");
    Serial.println("Número de atuadores: " + String(actLen));
    for (int i = 0; i < dumLen; i++) {
      dummies[i] = '0';
    }
    dumLen = 0;
    actSettings.selected = true;
  }
  //}
}
void selectActuator(void* vParameter) {
  //while(1){
  int getNum();
  int id = -1;
  if (l == SELECT || actSettings.shifting) {
    actSettings.shifting = true;
    actSettings.selected = false;
    readINTEn();
    id = getNum();
    if (id >= 0) {
      Actuator* p = getActuatorByID(actuators, actLen, id);
      actuator = (p != NULL) ? (p) : (actuator);
      Serial.println("Atuador " + String(actuator->getID()) + " selecionado.");
      actSettings.selected = true;
      actSettings.shifting = false;
    }
  }
  //}
}
void selectActuator(Actuator** actuators, int& actLen, Actuator* tact, int& sel) {
  tact = NULL;
  while (tact == NULL) {
    readINTEn();
    sel = getNum();//seleciona o atuador que servirá como gatilho
    tact = getActuatorByID(actuators, actLen, sel);
    if (tact == NULL) {
      hc06.write(ALERT);
    }
  }
}
void selectTimerEventTrigger(Actuator* act, void** trigList, int trigLen) {
  int sel2;
  if (trigLen > 1) {
    do {
      readINTEn();
      sel2 = getNum();  //selciona o gatilho desejado
      act->setTimerTrigger(trigList[sel2 - 1]);
    } while (sel2 > trigLen || sel2 <= 0);
  } else {//caso trigLen==1, não é necessário selecionar o gatilho
    act->setTimerTrigger(trigList[0]);
  }
  trigList = NULL;
}
void selectActuatorEventTrigger(Actuator* act, void** trigList, int trigLen) {
  int sel2;
  if (trigLen > 1) {
    do {
      readINTEn();
      sel2 = getNum();  //selciona o gatilho desejado
      act->setActuatorTrigger(trigList[sel2 - 1]);
    } while (sel2 > trigLen || sel2 <= 0);
  } else {
    act->setActuatorTrigger(trigList[0]);
  }
  trigList = NULL;
}
void xactSettings(void* vParameter = NULL) {
  //int getNum();

  //while (1) {

  if (l == SETUP || actSettings.setting) {
    actSettings.setting = true;
    if (actSettings.selected && !(actSettings.actuator)) {
      actSettings.selected = false;
    }
    createActuators();
    if (l == ACTUATOR || actSettings.actuator) {

      actSettings.actuator = true;
      if (!actSettings.selected) {
        //hc06.write(ALERT);

        readINTEn();
        sid = getNum();
        Serial.println("Configurando atuador " + String(sid));
        actuator = getActuatorByID(actuators, actLen, sid);
        actSettings.selected = true;
      }
      if (l == TIMER || actSettings.timer) {
        actSettings.timer = true;
        if (l == TRIGTYPE || actSettings.trigtype) {
          actSettings.trigtype = true;
          int tt;
          //if (((l - 48) >= 0) &&  ((l - 48) <= 9)) {
          do {
            readINTEn();
            tt = getNum();
            actuator->setTimerTriggerType(tt);
            if (tt < NOTRIGGER || tt > TEN) {
              hc06.write(ALERT);
            }
          } while (tt < NOTRIGGER || tt > TEN);

          actSettings.trigtype = false;
          //}
        } else if (l == TRIGGER || actSettings.trigger) {
          actSettings.trigger = true;
          int sel, sel2;
          Actuator* tact = NULL;
          void* trigList[] = {};
          switch (actuator->getTimerTrigType()) {
            case NOTRIGGER:
              actSettings.trigger = false;
              break;
            case TCN:
              actuator->setTimerTrigger(&l);
              actSettings.trigger = false;
              break;
            case TYN:
              selectActuator(actuators, actLen, tact, sel);
              for (int i = 0; i < 2; i++) {
                trigList[i] = i == 0 ? (void*)tact->giveY1() : (void*)tact->giveY2();
              }
              selectTimerEventTrigger(actuator, trigList, 2);
              actSettings.trigger = false;
              break;
            case TSN:

              selectActuator(actuators, actLen, tact, sel);
              for (int i = 0; i < 2; i++) {
                trigList[i] = i == 0 ? (void*)tact->giveS1() : (void*)tact->giveS2();
              }
              selectTimerEventTrigger(actuator, trigList, 2);
              actSettings.trigger = false;
              break;
            case TAN:
            case TRN:
              selectActuator(actuators, actLen, tact, sel);
              trigList[0] = (actuator->getTimerTrigType() == TAN ? (void*)tact->giveCounterA() : (void*)tact->giveCounterR());
              selectTimerEventTrigger(actuator, trigList, 1);
              actSettings.trigger = false;
              break;
            case TEN:
              actuator->setTimerTrigger(TEN);
              actSettings.trigger = false;
              break;
          }
        } else if (l == VALUE || actSettings.value) {
          int sel, sel2;
          actSettings.value = true;
          readINTEn();
          sel = getNum();
          readINTEn();
          sel2 = getNum();
          actuator->setTimer(sel, sel2);
          actSettings.value = false;
        }
      }
      if (l == TRIGTYPE || actSettings.trigtype) {//configura o tipo de gatilho do atuador
        actSettings.trigtype = true;
        int att;
        do {
          readINTEn();
          att = getNum();
          actuator->setActuatorTriggerType(att);
          if (att < NOTRIGGER || att > TEN) {
            hc06.write(ALERT);
          }
        } while (att < NOTRIGGER || att > TEN);

        actSettings.trigtype = false;
      } else if (l == TRIGGER || actSettings.trigger) {
        actSettings.trigger = true;
        int sel, sel2, aval;
        Actuator* sact = NULL;
        void* trigList[] = {};
        switch (actuator->getActuatorTrigType()) {
          case NOTRIGGER: break;
          case TCN: actuator->setActuatorTrigger(&l);
            actSettings.trigger = false;
            break;
          case TSN:
            selectActuator(actuators, actLen, sact, sel);
            for (int i = 0; i < 2; i++) {
              trigList[i] = i == 0 ? (void*)sact->giveS1() : (void*)sact->giveS2();
            }
            selectActuatorEventTrigger(actuator, trigList, 2);
            break;
          case TRN:
          case TAN:
            selectActuator(actuators, actLen, sact, sel);
            trigList[0] = (actuator->getTimerTrigType() == TAN ? (void*)sact->giveCounterA() : (void*)sact->giveCounterR());
            selectActuatorEventTrigger(actuator, trigList, 1);
            break;
            //          case TEN: actuator->setActuatorTrigger(TEN);
            //            break;
        }
      }


    }
    if (l == DONE) {
      //actSettings.selected = true;
      actSettings.setting = false;
      actSettings.actuator = false;
      //Serial.println("Número de atuadores: " + String(actLen));
    }
  }
  //}
}
void readINTEn() {
  actSettings.readingN = true;
}
void readINTUn() {
  actSettings.readingN = false;
}
int getNum () {
  char a;
  int num = 0;
  uint8_t isNegative = 0;
  Serial.println ("Entre um numero algarismo por algarismo:\n");
  while (actSettings.readingN)
  {
    l = WAITING;

    //      printf ("Estado %c", l);
    //    if (l == WAITING) {//**
    readData(0);  //***teste
    a = l;
    //    }                  //**
    if (num == 0 && a == '-') {
      isNegative = 1;
    }
    if (((a - 48) >= 0) &&  ((a - 48) <= 9)) {
      num *= 10;
      num += (a - 48);
      Serial.println(" num == " + String(num));
    } else if (a == BREAK) {
      readINTUn();
      Serial.println ("Saindo...");
    }
    else {
      Serial.println ("Isso nao eh um numero!");
      delay(5000);
    }

  }
  num *= (isNegative > 0 ? (-1) : (1));
  return num;
}
int getNum (char p) {
  int num = (p - 48 >= 0 && p - 48 <= 9) ? (p - 48) : (-1);
  return num;
}
