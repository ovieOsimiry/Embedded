#include "Election.h"

/* 4 possible numbers.
1 before choosing 1 PLayer
2 before choosing 2 Players
3 inside 1 Player mode
4 inside 2 Player mode*/

static systemState_t currentState;

void startState(){
	currentState = stateMainMenu;//1;
}

systemState_t getState(){
	return currentState;
}
void setState(systemState_t newState){
	currentState = newState;
}
