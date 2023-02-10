#ifndef ACTSETTINGSRDV_H
#define ACTSETTINGSRDV_H
	#include <TriggersAndCMs.h>
	using namespace trs_and_cms;
	/*Essa é apenas uma biblioteca de definições de bandeiras indicadoras.
	  Cada valor desse indica um bit da variável de configuração (um inteiro).
	  Esses bits serão usado para indicar qual ramo de configuração está e deve estar aberto ou fechado.
	*/
	#define SETTING   1
	#define STIMER	  2
	#define SVALUE	  4
	#define SACTUATOR 8
	#define STRIGGER  16
	#define STRIGTYPE 32
	#define SELECTED  64
	#define TSPRING   128
	#define TSOLENOID 256
	#define READINGN  512
	#define SHIFTING  1024
	#define CREATE	  2048
	#define CBUTTON	  4096
	#define TIMERON	  8192
	#define TIMEROFF  (2*8192)
	//assim que timer ON finalizar a contagem
	//void Actuator::update() é executado uma vez
	#define AFTERON   (((unsigned long)4)*((unsigned long)8192))
	
	//PINOS PARA SOLENÓIDE
	const int solpins[ACTPINSLEN] = {8, 7};
	const int endcoursepins[ACTPINSLEN*2] = {10, 9, 12, 11};
	//extern int actSettings;
	
	//bool getSetting    (int s);
	bool getSetting	   (unsigned int& ss, int s);
	//void setSetting    (int s);
	void setSetting	   (unsigned int& ss, int s);
	//void unsetSetting  (int s);
	void unsetSetting  (unsigned int& ss, int s);
	
	void unsetAll (unsigned int& ss);
	
#endif
