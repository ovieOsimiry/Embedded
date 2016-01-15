#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H


typedef enum
{
	stateMainMenu,
	stateGamePaused,
	state2ndPlayerPausedGame,
	stateYouWonARound2PlayerGame,
	stateYouLostARound2PlayerGame,
	stateGameOver,
	stateGame1Player,
	stateGame2Player,
	stateYouWon2playerGame,
	stateYouLost2PlayerGame,
	stateATieOccurred2PlayerGame
} systemState_t;

void startState();
systemState_t getState();
void setState(systemState_t state);//(int state);

#endif
