#include "CustomDigitalWiring.h"

int digitalReadR(int p){
    switch(p){		
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            return  (PIND & (1<<(p%8)));
		case 8: case 9: case 10:case 11:case 12:case 13:
		    return  (PINB & (1<<(p%8)));
		case 14:case 15:case 16:case 17:case 18:case 19:
		    return  (PINC & (1<<(p%8)));
	}
    return false;  
}
