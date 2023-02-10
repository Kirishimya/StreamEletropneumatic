#ifndef CUSTOMDIGITALWIRING_H
#define CUSTOMDIGITALWIRING_H
#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
  #else
  #include <WProgram.h>
#endif
    #define INPUTR  0
    #define OUTPUTR 1
    #define LOWR    0
    #define HIGHR   1
    
    #define pinModeR(P, mode)  		   { 	if (mode <= INPUTR)\
										    {\
	        									switch(P){  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: \
														DDRD &= ~(1<<(P%8));\
														case 8: case 9: case 10:case 11:case 12:case 13:\
														DDRB &= ~(1<<(P%8));\
														case 14:case 15:case 16:case 17:case 18:case 19:\
														DDRC &= ~(1<<(P%8));\
													}\
										    }    		\
												else	\
											    {		\
													switch(P){  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: \
														DDRD |= (1<<(P%8));\
														case 8: case 9: case 10:case 11:case 12:case 13:\
														DDRB |= (1<<(P%8));\
														case 14:case 15:case 16:case 17:case 18:case 19:\
														DDRC |= (1<<(P%8));\
													}\
												}		\
										}
	#define digitalWriteR(P, level)    {    if (level <= LOW)															\
										    {			\
										        switch(P){		\
														case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: \
														PORTD &= ~(1<<(P%8));\
														case 8: case 9: case 10:case 11:case 12:case 13:\
														PORTB &= ~(1<<(P%8));\
														case 14:case 15:case 16:case 17:case 18:case 19:\
														PORTC &= ~(1<<(P%8));\
													}  \
											}			\
												else	\
												{     		   \
													switch(P){ \
														case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: \
														PORTD |= (1<<(P%8));\
														case 8:case 9:case 10:case 11:case 12:case 13:\
														PORTB |= (1<<(P%8));\
														case 14:case 15:case 16:case 17:case 18:case 19:\
														PORTC |= (1<<(P%8));\
													}  \
											    }		\
										}
    int  digitalReadR (int p);
#endif
