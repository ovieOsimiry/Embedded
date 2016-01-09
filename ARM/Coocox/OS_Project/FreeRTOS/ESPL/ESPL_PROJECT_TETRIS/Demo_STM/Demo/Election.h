#ifndef ELECTION_H
#define ELECTION_H


typedef enum
{
	stateMainMenu,
	stateGamePaused,
	state2ndPlayerPausedGame,
	stateYouWonARound,
	stateYouLostARound,
	stateGameOver,
	stateGame1Player,
	stateGame2Player,
	stateYouWon2playerGame,
	stateYouLost2PlayerGame
} systemState_t;

void startState();
systemState_t getState();
void setState(systemState_t state);//(int state);

#endif
