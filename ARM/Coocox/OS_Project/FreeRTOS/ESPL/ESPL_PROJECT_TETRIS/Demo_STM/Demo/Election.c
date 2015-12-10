#ifndef _ELECTION_
#define _ELECTION_
#include "Election.h"

/* 4 possible numbers.
1 before choosing 1 PLayer
2 before choosing 2 Players
3 inside 1 Player mode
4 inside 2 Player mode*/

int state;

void startState(){
	state = 1;
}

int getState(){
	return state;
}

void setState(int state1){
	state = state1;
}

#endif
