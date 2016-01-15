#include "SystemState.h"

static systemState_t currentState;

/* -------------------------------------------
 *@desc Starts the state of the game in the Main Menu
 *@param void parameters
 *@return void parameters
 * ------------------------------------------- */
void startState(){
	currentState = stateMainMenu;//1;
}

/* -------------------------------------------
 *@desc Gets the state of the game
 *@param void parameters
 *@return systemState - Current state of the game
 * ------------------------------------------- */
systemState_t getState(){
	return currentState;
}

/* -------------------------------------------
 *@desc Sets the state of the game
 *@param systemState new state of the game
 *@return void parameters
 * ------------------------------------------- */
void setState(systemState_t newState){
	currentState = newState;
}
