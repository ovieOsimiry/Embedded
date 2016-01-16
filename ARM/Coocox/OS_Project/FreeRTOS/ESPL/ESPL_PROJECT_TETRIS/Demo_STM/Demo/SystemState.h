/*
 * This file defines all the possible states the system can have,
 * in here functions for accessing and setting the state of the system are declared as well.
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 */

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
