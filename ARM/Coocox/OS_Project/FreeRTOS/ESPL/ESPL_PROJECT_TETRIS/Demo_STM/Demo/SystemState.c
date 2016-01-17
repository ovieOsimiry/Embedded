/*
 * This file holds function definitions that set and retrieve the possible states
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 */


#include "SystemState.h"

static systemState_t currentState;

/* -------------------------------------------
 *@desc: Starts the state of the game in the Main Menu.
 *
 *@param:	- void parameters
 *
 *@return:	- void
 * ------------------------------------------- */
void startState(){
	currentState = stateMainMenu;
}

/* -------------------------------------------
 *@desc: Gets the state of the game.
 *
 *@param:				- void parameters
 *
 *@return: systemState	- Current state of the game
 * ------------------------------------------- */
systemState_t getState(){
	return currentState;
}

/* -------------------------------------------
 *@desc: Sets the state of the game.
 *
 *@param: newState	- systemState new state of the game
 *
 *@return:			- void parameters
 * ------------------------------------------- */
void setState(systemState_t newState){
	currentState = newState;
}
