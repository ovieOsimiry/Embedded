#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H


typedef enum
{
	stateMainMenu,
	stateGamePaused,
	stateGameOver,
	stateGame1Player,
	stateGame2Player
} systemState_t;

void startState();
systemState_t getState();
void setState(systemState_t state);//(int state);

#endif
