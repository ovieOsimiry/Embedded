/**
 * This is the main file of the ESPLaboratory Demo project.
 * It implements simple sample functions for the usage of UART,
 * writing to the display and processing user inputs.
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 *
 */

#include "Demo.h"
#include "includes.h"
#include "Draw.h"
#include "Shape.h"
#include "SystemState.h"
#include "time.h"


// start and stop bytes for the UART protocol
#define ESPL_StartByte 0xAA
#define ESPL_StopByte 0x55



QueueHandle_t ESPL_RxQueue; // Already defined in ESPL_Functions.h
SemaphoreHandle_t ESPL_DisplayReady;


typedef struct _joyStickCorrd
{
	coord_t x_joy;
	coord_t y_joy;
}JoyStickCord_t;

JoyStickCord_t JoyStickVal = {0,0};
// Stores lines to be drawn
QueueHandle_t DrawQueue;

/*--------------Please put all function prototypes used only within this file here----*/
void ResetGamePlay();
void CreateNewPiece();
void VApplicationIdleHook();
void sendValue(uint32_t  anIntegerValue);
int calculateScore(int level, int lines);
static void checkJoystick();
static void drawTask();
static void GamePlay();
static void SystemState();
static void ReceiveValue();
/*-----------------Please put all shared global variables here------------------------*/


int mNextShape, mNextRotation;	// Kind and rotation of the next piece
int debounce = 0; //Restricts the execution of the IRQ_handler if the debouncing time hasn't expired

int gTotalNumberOfLinesCompleted = 0;
int gGameScore = 0;
int gDifficultyLevel;
int gReceiving = 0;
int gHighestScore = 0;
int g2playerGameNoOfRoundsWon = 0;
int g2playerGameNoOfRounds = 0;
int gWhoWon = 0;
uint8_t gSelectionArrowPosition = 0;
uint8_t gPlayer1NumOfLinesCompleted = 0;
uint8_t gPlayer2NumOfLinesCompleted = 0;
shape_t gCurrentShape;
shape_t gNextShape;
joystickselection_t gjoyStickSelection = JoyStickNoSelection;
joystickselection_t gjoyStickLastSelection = JoyStickNoSelection;
boolean_t gSelectButtonPressed = false;
boolean_t gShapeDownMovementSpeedGaurd = false;
boolean_t gSend2PlayerRequestFlag = true;

playermode_t gPlayerMode = onePlayerMode;
/*---------------------Game Play global variables used in GamePlay task-------------*/
#define INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE  15;
const uint8_t TWO_PLAYER_REQUEST_VALUE = 0xeb;
#define TWO_PLAYER_MODE_GAME_OVER  0xab
#define TWO_PLAYER_MODE_NO_OF_LINES_TO_COMPLETE 30
#define HORIZONTAL_CONTROL_MOVE_SPEED 4
#define VERTICAL_CONTROL_MOVE_SPEED 2
uint32_t gDifficultyCheckPoint;

/*------------------------------------------------------------------------------------*/


/*--------------------------RTOS related definitions----------------------------------*/

/*------------------------------Semaphores/Mutexs-------------------------------------*/

static TaskHandle_t gTaskSynchroNotification = NULL;

/*---------------------------------------Timers---------------------------------------*/
TimerHandle_t xTimers; //Define the de-bouncing timer
/*------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------*/

/**
 * Entry function to the example program.
 * Initializes hardware and software.
 * Starts scheduler.
 */
int main() {
	// Initialize Board functions
	ESPL_SystemInit();
	//Initialize debounce button
	timerInit();

	//Initialise the state of the game to main menu
	startState();

	// Initializes Draw Queue with 100 lines buffer
	DrawQueue = xQueueCreate(100, 4 * sizeof(char));

	// Initialise all Tasks with their respective priority
//	xTaskCreate(drawTask, "drawTask", 1000, NULL, 4, NULL);
//	xTaskCreate(checkJoystick, "checkJoystick", 1000, NULL, 3, NULL);
//	xTaskCreate(ReceiveValue, "ReceiveValue", 1000, NULL, 2, NULL);
//	xTaskCreate(GamePlay, "GamePlay", 1000, NULL, 3, &gTaskSynchroNotification);
//	xTaskCreate(SystemState, "SystemState", 1000, NULL, 3, NULL);

	xTaskCreate(drawTask, "drawTask", 1000, NULL, 2, NULL);
	xTaskCreate(checkJoystick, "checkJoystick", 1000, NULL, 4, NULL);
	xTaskCreate(ReceiveValue, "ReceiveValue", 1000, NULL, 4, NULL);
	xTaskCreate(GamePlay, "GamePlay", 1000, NULL, 3, &gTaskSynchroNotification);
	xTaskCreate(SystemState, "SystemState", 1000, NULL, 3, NULL);
	// Start FreeRTOS Scheduler
	vTaskStartScheduler();
}


/* -------------------------------------------
 *@desc: Assign the next shape to the current one, creates a new shape and stores it in gNextShape.
 *
 *@param:	- void parameters
 *
 *@return:	- void
 * ------------------------------------------- */
void CreateNewShape()
{
	// The new shape
	gCurrentShape = gNextShape;

	int randNum;
	TM_RNG_Init();
	randNum = TM_RNG_Get() & 0x1f;
	while(randNum>27)
	{
		randNum = TM_RNG_Get() & 0x1f;
	}
	gNextShape = GetShape(randNum);//compute next shape.
}

static void SystemState()
{
	TickType_t _debounceDelay = 300;
	/*------------------------Initialise all variables used within this task----------------*/
	gPlayerMode = onePlayerMode;
    gjoyStickLastSelection = JoyStickUp;
    gSelectButtonPressed = 0;
    gReceiving = 0;
    gSend2PlayerRequestFlag = true;
    uint8_t maxMenuCount = 0;
    int lastState;
    /*---------------------------------------------------------------------------------------*/
	while(1)
	{

			switch(getState())
			{
				case stateMainMenu:
					maxMenuCount = 1;

					if(gjoyStickSelection == JoyStickUp)
						{
						   gPlayerMode = onePlayerMode;
						   gReceiving = 0;

						   if(gjoyStickLastSelection==JoyStickNoSelection) //check if the joy stick has been released. If has been released then enter.
							{
								gjoyStickLastSelection = JoyStickUp;//update the joy stick to up selection to prevent the selectionArrowPosition from changing continuously.
								if(gSelectionArrowPosition!=0)
								{
									--gSelectionArrowPosition;
								}
								gjoyStickLastSelection = JoyStickUp;
							}
						}
					else if(gjoyStickSelection == JoyStickDown)
						{
							gSend2PlayerRequestFlag = true;// enable flag so that a response can be sent back.
							sendValue(TWO_PLAYER_REQUEST_VALUE);
							if(gReceiving == TWO_PLAYER_REQUEST_VALUE)
							{
								gPlayerMode = twoPlayerMode;
								gjoyStickLastSelection = JoyStickDown;
								gReceiving = 0;//reset received value

								{
									if(gSelectionArrowPosition!=maxMenuCount)
									{
										++gSelectionArrowPosition;
									}
								}
							}
						}
					else
						{
						gjoyStickLastSelection=JoyStickNoSelection;
							if(gReceiving==TWO_PLAYER_REQUEST_VALUE)
							 {
								gReceiving = 0;
								sendValue(TWO_PLAYER_REQUEST_VALUE);
								gSend2PlayerRequestFlag = false;
							 }
						}
						if (gSelectButtonPressed && gSelectionArrowPosition==0)
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gReceiving = 0;
							setState(stateGame1Player);//1 player mode
							gSelectionArrowPosition = 0;
						}

						if (gSelectButtonPressed && gSelectionArrowPosition==1)
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gReceiving = 0;
							setState(stateGame2Player);//2 player mode
							gSelectionArrowPosition = 0;
						}
				break;

				case stateGame1Player:
					gSelectButtonPressed = 0;
					lastState = stateGame1Player;
					//We wake up the Gameplay task if we are in state 2 (gaming screen in two player mode)
					xTaskNotifyGive(gTaskSynchroNotification);

				break;

				case stateGame2Player:
					gSelectButtonPressed = 0;
					lastState = stateGame2Player;
					//We wake up the Gameplay task if we are in  stateGame1Player or stateGame2Player (gaming screen in one player mode)
					xTaskNotifyGive(gTaskSynchroNotification);

				break;
				case stateGamePaused:
					maxMenuCount = 2;
					if(gjoyStickSelection == JoyStickUp)
						{
							if(gjoyStickLastSelection==JoyStickNoSelection) //check if the joy stick has been released. If has been released then enter.
							{
								gjoyStickLastSelection = JoyStickUp;//update the joy stick to up selection to prevent the selectionArrowPosition from changing continuously.
								if(gSelectionArrowPosition!=0)
								{
									--gSelectionArrowPosition;
								}
								gjoyStickLastSelection = JoyStickUp;
							}
						}
					else if(gjoyStickSelection == JoyStickDown)
						{
							if(gjoyStickLastSelection==JoyStickNoSelection) //check if the joy stick has been released. If has been released then enter.
							{
								gjoyStickLastSelection = JoyStickDown;//update the joy stick to up selection to prevent the selectionArrowPosition from changing continuously.
								if(gSelectionArrowPosition!=maxMenuCount)
								{
									++gSelectionArrowPosition;
								}
							}
						}
					else
						{
							gjoyStickLastSelection = JoyStickNoSelection;
						}
						if (gSelectButtonPressed && gSelectionArrowPosition == 0)
						{
							vTaskDelay(_debounceDelay);//wait for the button to settle
							gSelectButtonPressed = 0;
							gSelectionArrowPosition = 0; //reset the menu selection to start from the first menu item
							if(lastState==stateGame1Player)
								setState(stateGame1Player);
							else if(lastState==stateGame2Player)
								setState(stateGame2Player);
						}

						if (gSelectButtonPressed && gSelectionArrowPosition == 1)
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gSelectionArrowPosition = 0; //reset the menu selection to start from the first menu item
							if(lastState==stateGame1Player)
								setState(stateGame1Player);
							else if(lastState==stateGame2Player)
								setState(stateGame2Player);
							ResetGamePlay(); //restart the game
						}

						if (gSelectButtonPressed && gSelectionArrowPosition == 2)
						{
							vTaskDelay(_debounceDelay);
							gSelectionArrowPosition = 0; //reset the menu selection to start from the first menu item
							ResetGamePlay();
							gSelectButtonPressed = 0;
							setState(stateMainMenu);
						}
				break;
				case stateGameOver:
					maxMenuCount = 1;
					if(gjoyStickSelection == JoyStickUp)
						{
							if(gjoyStickLastSelection==JoyStickNoSelection) //check if the joy stick has been released. If has been released then enter.
							{
								gjoyStickLastSelection = JoyStickUp;//update the joy stick to up selection to prevent the selectionArrowPosition from changing continuously.
								if(gSelectionArrowPosition!=0)
								{
									--gSelectionArrowPosition;
								}
								gjoyStickLastSelection = JoyStickUp;
							}
						}
					else if(gjoyStickSelection == JoyStickDown)
						{
							if(gjoyStickLastSelection==JoyStickNoSelection) //check if the joy stick has been released. If has been released then enter.
							{
								gjoyStickLastSelection = JoyStickDown;//update the joy stick to up selection to prevent the selectionArrowPosition from changing continuously.
								if(gSelectionArrowPosition!=maxMenuCount)
								{
									++gSelectionArrowPosition;
								}
							}
						}
					else
						{
							gjoyStickLastSelection = JoyStickNoSelection;
						}

						if(gSelectButtonPressed && gSelectionArrowPosition==0)//gjoyStickLastSelection==JoyStickUp)//restart game
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;

							if(lastState==stateGame1Player)
								setState(stateGame1Player);
							else if(lastState==stateGame2Player)
								setState(stateGame2Player);
						}
						if(gSelectButtonPressed && gSelectionArrowPosition == 1)//gjoyStickLastSelection==JoyStickDown)//go to main menu
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gjoyStickLastSelection=JoyStickUp;
							setState(stateMainMenu);
						}
					break;
			}

			vTaskDelay(10);
	}
}

static void GamePlay()
{
		shape_t * ptrShape = &gCurrentShape;
		shape_t shapeTemp;// used for temporary calculations
		const int MAX_DIFFICULTY_LEVEL = 9;
		const char MAX_TICK = 100;
		int tick = 0;
		boolean_t downMovePossible = true;
		int tempNoOfLines = 0;
		char verticalSpeed = 0;
//@-------------------------------------------
		int randNo;
		TM_RNG_Init();
		randNo = TM_RNG_Get() & 0x1f;
		while(randNo>27)
		{
			randNo = TM_RNG_Get() & 0x1f;
		}
	    gNextShape = GetShape(randNo);
		CreateNewShape();
//@-------------------------------------------


/*--------Initialise global variables used within this task----------------*/

		gDifficultyLevel = 0;
		gDifficultyCheckPoint = (uint32_t) INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE;

/*-------------------------------------------------------------------------*/

	while(1){
				//Make the loop wait until the task notification from the SystemState task is released
				ulTaskNotifyTake( pdTRUE, portMAX_DELAY);

				vTaskDelay(10);
				if(gTotalNumberOfLinesCompleted >= gDifficultyCheckPoint)
				{
					gDifficultyCheckPoint = gDifficultyCheckPoint + (10/(gDifficultyLevel+1));
					if(gDifficultyLevel!=MAX_DIFFICULTY_LEVEL)
					 ++gDifficultyLevel;
				}

				if(getState() == stateGame2Player) //if we are in 2 player mode, when lines are sent add them.
				{

					switch(gReceiving)
					{
						case TWO_PLAYER_MODE_GAME_OVER: // Takes care of when 2nd player loses by full screen.
							++g2playerGameNoOfRoundsWon;
							++g2playerGameNoOfRounds;
							if(g2playerGameNoOfRoundsWon==4){
								gWhoWon = 1;
								setState(stateGameOver);
								g2playerGameNoOfRoundsWon = 0; //reset variable for next time
								g2playerGameNoOfRounds = 0;
								gPlayer1NumOfLinesCompleted = 0;
								gPlayer2NumOfLinesCompleted = 0;
								gReceiving = 0;
							}else
							{
								ResetGamePlay();
								tempNoOfLines = 0;//reset temporary local variables within the task.
								gPlayer1NumOfLinesCompleted = 0;
								gPlayer2NumOfLinesCompleted = 0;
								gReceiving = 0;//reset variable for next time
							}
						break;

						default ://Handles number of lines we receive or we generate.

							if(gReceiving!=0)
								gPlayer2NumOfLinesCompleted+=gReceiving;//keep track of second player number of lines

							switch(gReceiving)
							{
								case 2:
									AddLine(1,ptrShape);
									gReceiving = 0;
								break;
								case 3:
									AddLine(2,ptrShape);
									gReceiving = 0;
								break;
								case 4:
									AddLine(4,ptrShape);
									gReceiving = 0;
								break;
							}

							gReceiving=0;


							//We check our line count first because it got updated in the last cycle so we need to check it if we already reached 30 lines
							//if our line count has reached 30 lines then it means we have won a round p.
							if(gPlayer1NumOfLinesCompleted >=TWO_PLAYER_MODE_NO_OF_LINES_TO_COMPLETE)
							{
								if(gPlayer2NumOfLinesCompleted >=TWO_PLAYER_MODE_NO_OF_LINES_TO_COMPLETE)//we check if player 2 has also reached 30 lines and if so a tie has occurred.
								{
									ResetGamePlay();
									tempNoOfLines = 0;//reset temporary local variables within the task.
									gPlayer1NumOfLinesCompleted = 0;
									gPlayer2NumOfLinesCompleted = 0;
								}
								else//if no tie happened then we won the round or game.
								{
									++g2playerGameNoOfRoundsWon;
									++g2playerGameNoOfRounds;
									if(g2playerGameNoOfRoundsWon == 4){
										gWhoWon = 1;
										setState(stateGameOver);
										g2playerGameNoOfRoundsWon = 0; //reset variable for next time
										g2playerGameNoOfRounds = 0;
										gPlayer1NumOfLinesCompleted = 0;
										gPlayer2NumOfLinesCompleted = 0;
									}
									else
									{
										ResetGamePlay();
										gPlayer1NumOfLinesCompleted = 0;
										gPlayer2NumOfLinesCompleted = 0;
										tempNoOfLines = 0;//reset temporary local variables within the task.
									}
								}
							}
							else if(gPlayer2NumOfLinesCompleted >=TWO_PLAYER_MODE_NO_OF_LINES_TO_COMPLETE)
							{
								++g2playerGameNoOfRounds;
								if((g2playerGameNoOfRounds - g2playerGameNoOfRoundsWon) == 4)
								{
									gWhoWon = 2;
									setState(stateGameOver);
									g2playerGameNoOfRoundsWon = 0; //reset variable for next time
									g2playerGameNoOfRounds = 0;
									gPlayer1NumOfLinesCompleted = 0;
									gPlayer2NumOfLinesCompleted = 0;
								}
								else
								{
									ResetGamePlay();
									tempNoOfLines = 0;//reset temporary local variables within the task.
									gPlayer1NumOfLinesCompleted = 0;
									gPlayer2NumOfLinesCompleted = 0;
								}

							}


						break;
					}
				}

				if(tick >=(MAX_TICK/(gDifficultyLevel+1)))
					tick = 0;
				else
					tick++;

			if( (tick==(MAX_TICK/(gDifficultyLevel+1))) || ((gjoyStickSelection == JoyStickDown) && (gShapeDownMovementSpeedGaurd == true) ))
				{
					shapeTemp =  *ptrShape;
					shapeTemp.y = shapeTemp.y + 1;
					downMovePossible = IsMoveMentPossible(&shapeTemp);
					if(downMovePossible==true)
					{
						if (ptrShape->y==20)
							ptrShape->y = 0;
						else
						{
							if(verticalSpeed == VERTICAL_CONTROL_MOVE_SPEED)
							{
								verticalSpeed = 0;
								ptrShape->y+=1;
							}else
							{
								++verticalSpeed;
							}
						}
					}
					else
					{
						StoreShape(ptrShape);
						tempNoOfLines = DeletePossibleLines();
						gTotalNumberOfLinesCompleted += tempNoOfLines;
						gGameScore+=calculateScore(gDifficultyLevel,tempNoOfLines);
						if(gGameScore>gHighestScore)
						{
							gHighestScore = gGameScore;
						}
						gShapeDownMovementSpeedGaurd=false;
						gPlayer1NumOfLinesCompleted+=tempNoOfLines;//Number of lines completed

						if(tempNoOfLines>=1 && getState()==stateGame2Player)
							{
								sendValue(tempNoOfLines);
							}

						if(!isGameOver())
						 {
							CreateNewShape();//if the game is not over then create a new shape.
						 }
						else
						 {

							if(getState()==stateGame2Player){
								sendValue(TWO_PLAYER_MODE_GAME_OVER);//Notify the other player you lost. takes care of when you lost a round due to full screen in 2 player mode.
								ResetGamePlay();
								tempNoOfLines = 0;//reset temporary local variables within the task.
								++g2playerGameNoOfRounds;
								if((g2playerGameNoOfRounds - g2playerGameNoOfRoundsWon)  == 4)
								{
									gWhoWon = 2;
									setState(stateGameOver);
									g2playerGameNoOfRoundsWon = 0; //reset variable for next time
									g2playerGameNoOfRounds = 0;
									gPlayer1NumOfLinesCompleted = 0;
									gPlayer2NumOfLinesCompleted = 0;
								}
								else
								{
									ResetGamePlay();
									tempNoOfLines = 0;//reset temporary local variables within the task.
									gPlayer1NumOfLinesCompleted = 0;
									gPlayer2NumOfLinesCompleted = 0;
								}
							}
							else
							{
								gWhoWon = 0;
								setState(stateGameOver);
							}
						 }
					}
				}
			else if(getState()==stateGameOver)
			{
				ResetGamePlay();
				tempNoOfLines = 0;//reset temporary local variables within the task.
			}
		}
}

/* -------------------------------------------
 *@desc: Draws the screen depending on the position of the shapes, state of the game and use of the buttons and joystick;
 *	 	 making use of the functions in the Draw.c file.
 *
 *@param:	- void parameters
 *
 *@return:	- void
 * ------------------------------------------- */
static void drawTask() {


	struct line line; // Init buffer for line
	InitializeBoardMatrix();
	gdispClear(White);



	// Start endless loop
	while (TRUE) {
		// wait for buffer swap
		while (xQueueReceive(DrawQueue, &line, 0) == pdTRUE) {
		 }
		// draw to display
		gdispClear(Black);

		if(getState()==stateMainMenu)
		{
			DrawMainMenu(&gSelectionArrowPosition, &gPlayerMode, &gHighestScore);
		}
		else if(getState()==stateGame1Player || getState()==stateGame2Player) {
			DrawGameFrame(&gNextShape,gTotalNumberOfLinesCompleted,gGameScore,gDifficultyLevel,g2playerGameNoOfRoundsWon,
					g2playerGameNoOfRounds, gPlayer2NumOfLinesCompleted,getState());
			DrawShapeWithHandle(&gCurrentShape);
		}
		else if(getState()== stateGamePaused)
		{
			DrawPauseMenu(&gSelectionArrowPosition);
		}
		else if(getState()==stateGameOver)
		{
			DrawGameOver(&gSelectionArrowPosition,gWhoWon);
		}

		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, 0);
		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, portMAX_DELAY);
		// swap buffers
		ESPL_DrawLayer();
	}
}

/* -------------------------------------------
 *@desc: Resets the global variables of the game to zero and initializes the functions for a new game.
 *
 *@param:	- void parameters
 *
 *@return:	- void
 * ------------------------------------------- */
void ResetGamePlay()
{
	gGameScore = 0;
	gDifficultyLevel = 0;
	gDifficultyCheckPoint = INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE; //Initialize the difficulty level.
	gTotalNumberOfLinesCompleted = 0;
	InitializeBoardMatrix();
	CreateNewShape();
}

/* -------------------------------------------
 *@desc: Calculates the score of the game according to the lines made and the level of the game.
 *
 *@param: level - current level of the game
 *@param: lines - lines made
 *
 *@return: int 	- Score reached with those lines and level.
 * ------------------------------------------- */
int calculateScore(int level, int lines)
{

	switch(lines){
		case 0:
			return 0;
		case 1:
			return 40*(level+1);
		case 2:
			return 100*(level+1);
		case 3:
			return 300*(level+1);
		case 4:
			return 1200*(level+1);
		default:
			return 0;

	}
}

/* -------------------------------------------
 *@desc: External interrupt handler for Button E. Pauses the game.
 *
 *@param:	- void
 *
 *@return:	- void
 * ------------------------------------------- */
void EXTI0_IRQHandler(void)//Button E interrupt handler
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
	  timerStart();
	  if(debounce == 0)
	  {
		  if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			  /* Button E pauses the game */
			  if(GPIO_ReadInputDataBit(ESPL_Register_Button_E, ESPL_Pin_Button_E)==0){
				  if(getState()!=stateMainMenu || getState()!=stateGameOver) //Only pauses the game if we are in a gameplay state.
				  {
					  setState(stateGamePaused);
				  	  debounce = 1; //The function is executed once so it will not be executed until the debouncing timer expires.
				  }
			  }
		  }
	  }
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}


/* -------------------------------------------
 *@desc: External interrupt handler for Button B. Selects the option on which the pointer is.
 *
 *@param:	- void
 *
 *@return:	- void
 * ------------------------------------------- */
void EXTI4_IRQHandler(void)//Button B interrupt handler
{

	timerStart();
	if(debounce == 0)
	 {
		if(EXTI_GetITStatus(EXTI_Line4) != RESET)
		{
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_B, ESPL_Pin_Button_B)==0)
			{
				gSelectButtonPressed = 1;
				debounce = 1; //The function is executed once so it will not be executed until the debouncing timer expires.
			}
		}
	}
	/* Clear the EXTI line 4 pending bit */
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/* -------------------------------------------
 *@desc: External interrupt handler for Button A and C. Button C does nothing. Button A changes the orientation of the shape
 *		 while playing.
 *
 *@param:	- void
 *
 *@return:	- void
 * ------------------------------------------- */
void EXTI9_5_IRQHandler(void)//Buttons C and A interrupt handler
{
	timerStart(); //Reset the debouncing timer

	if(debounce == 0){
		if(EXTI_GetITStatus(EXTI_Line5) != RESET) //Button C pressed
		{
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_C, ESPL_Pin_Button_C)==0)
			debounce = 1;
		}
		else if(EXTI_GetITStatus(EXTI_Line6) != RESET) //Button A pressed
		{
			shape_t shapeTemp = gCurrentShape;
			boolean_t rotatePossible = true;
			if(shapeTemp.shapeOrientation==3){
				shapeTemp.shapeOrientation = 0;  //If we have reached the max value roll it back to zero.
			}
			else {
				shapeTemp.shapeOrientation = shapeTemp.shapeOrientation + 1;	//Increase the rotation.
			}
			rotatePossible = IsMoveMentPossible (&shapeTemp); //Checks if the rotation is possible, it may collide with the limits
															  //or with other blocks.
			if(rotatePossible==true)  //If rotation is possible then assign it to the new rotation.
			{
				gCurrentShape.shapeOrientation = shapeTemp.shapeOrientation;
			}
			debounce = 1; //The function is executed once so it will not be executed until the debouncing timer expires.
		}
	}
	/* Clear the EXTI lines 5 and 6 pending bit */
	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line5);
}


/*-------------------------------------------------------------------------------------------------------
 *@desc: This task polls the joystick value every 20 ticks. For the horizontal shifting we first have to reach the value of
 *		 HORIZONTAL_CONTROL_MOVE_SPEED to increase the x position of the shape 1 time. This allows us to control the horizontal
 *		 shifting speed of the shape. For the vertical shifting we just refresh the variable gjoyStickSelection which will be used
 *		 in other tasks, like GamePlay and SystemState, to accelerate the vertical falling of the shape and to choose between options
 *		 respectively.
 *
 *@param:	- void
 *
 *@return:	- void
 * -----------------------------------------------------------------------------------------------------*/
static void checkJoystick() {
		TickType_t xLastWakeTime;
		xLastWakeTime = xTaskGetTickCount();
		struct coord joystick_now = { 0, 0 };
		boolean_t movePossible = true;
		shape_t * ptrShape = &gCurrentShape;
		shape_t shapeTemp;
		int leftMove = 0;
		int rightMove = 0;

		while (TRUE) {

			joystick_now.x = (uint8_t) (ADC_GetConversionValue(ESPL_ADC_Joystick_2) >> 4);
			joystick_now.y = (uint8_t) 255 - (ADC_GetConversionValue(ESPL_ADC_Joystick_1) >> 4);

			shapeTemp = *ptrShape;	//Make a temporary copy of the shape.
			if(joystick_now.x > 150)
			{
				if(rightMove==HORIZONTAL_CONTROL_MOVE_SPEED)
				{
					rightMove = 0;
					shapeTemp.x = shapeTemp.x + 1;				//If we have not reached the max value then increase.
				movePossible = IsMoveMentPossible (&shapeTemp);	//Check if the shape doesn't go further the lateral limits.
				if(movePossible==true)
					ptrShape->x = shapeTemp.x;
				}
				else
					++rightMove;
			}

			else if((joystick_now.x < 120))
			{
				if(leftMove==HORIZONTAL_CONTROL_MOVE_SPEED)
				{
				leftMove = 0;
				shapeTemp.x = shapeTemp.x - 1;					//If we have not reached the min value then decrease.
				movePossible = IsMoveMentPossible (&shapeTemp);	//Check if the shape doesn't go further the lateral limits.
				if(movePossible==true)
					ptrShape->x = shapeTemp.x;
				}else
				{
					++leftMove;
				}
			}

			if(joystick_now.y > 135) 				//Joystick is moved downwards.
			{
				gjoyStickSelection = JoyStickDown;
			}
			else if((joystick_now.y < 110))			//Joystick moved upwards
			{
				gjoyStickSelection = JoyStickUp;
			}
			else
			{
				gShapeDownMovementSpeedGaurd=true;
				gjoyStickSelection = JoyStickNoSelection;	//If the joystick hasn't moved.
			}
			vTaskDelayUntil(&xLastWakeTime, 20); 			//Polls the joystick every 20 ticks.
		}
	}

/*************************BUTTON DEBOUNCING TIMER FUNCTIONS**************************************/
/*-------------------------------------------
 *@desc: Function executed when the debouncing timer expires. Resets the debounce variable and stops the timer.
 *
 *@param: pxTimer	- Timer handler
 *
 *@return:			-void
 *-------------------------------------------*/
void vTimerCallback(TimerHandle_t pxTimer){
	//Allow the next button interruption
	debounce = 0;

	//Stop the timer
	xTimerStop(pxTimer, 0);
}

/*-------------------------------------------
 *@desc: Sets up the debouncing timer
 *
 *@param:	-void
 *
 *@return:	-void
 *-------------------------------------------*/
void timerInit(){
	//Software timer to debounce the button
	//It overflows in 200ms and then it calls the vTimerCallback
	xTimers = xTimerCreate("timer", 200 / portTICK_PERIOD_MS, pdTRUE, (void *)1, vTimerCallback);

	xTimerStart(xTimers, 0);
	xTimerStop(xTimers, 0);

}

/*-------------------------------------------
 *@desc: Resets the debouncing timer
 *
 *@param:	-void
 *
 *@return:	-void
 *-------------------------------------------*/
void timerStart(){
	//Reset the timer
	xTimerReset(xTimers, 0);
}
/************************************************************************************************/

/*-------------------------------------------
 *@desc: Sends a 32 bit integer through the UART connection. It also makes a simple checksum of the bytes that will be sent for
 *		 simple error detection.
 *
 *@param:	-void
 *
 *@return:	-void
 *-------------------------------------------*/
void sendValue(uint32_t  anIntegerValue)
{
	uint8_t bytes[4];
	char checksum;
	//Because we can only send byte by byte through the UART we divide the 32 bit integer in 4 bytes.
	bytes[3] = (anIntegerValue >> 24) & 0xFF;
	bytes[2] = (anIntegerValue >> 16) & 0xFF;
	bytes[1] = (anIntegerValue >> 8) & 0xFF;
	bytes[0] = anIntegerValue & 0xFF;
	// Generate simple error detection checksum
	checksum = bytes[3] ^ bytes[2] ^ bytes[1] ^ bytes[0];

	// Structure of one packet:
	//  Start byte
	//	4 * line byte
	//	checksum (all xor)
	//	End byte

	UART_SendData((uint8_t) ESPL_StartByte);
	UART_SendData((uint8_t) (bytes[0]));
	UART_SendData((uint8_t) (bytes[1]));
	UART_SendData((uint8_t) (bytes[2]));
	UART_SendData((uint8_t) (bytes[3]));
	UART_SendData((uint8_t) checksum);
	UART_SendData((uint8_t) ESPL_StopByte);
}

/*-------------------------------------------
 *@desc: Taks which receives data via UART and decodes it.
 *
 *@param:	-void
 *
 *@return:	-void
 *-------------------------------------------*/
static void ReceiveValue()
{
	char input;
	uint8_t pos = 0;
	char checksum;
	char buffer[7];
	while (TRUE)
	 {
		// wait for data in queue
		xQueueReceive(ESPL_RxQueue, &input, portMAX_DELAY);
		// decode package by buffer position
		switch (pos) {
			// start byte
			case 0:
				if (input == ESPL_StartByte) {
					buffer[0] = input;	//Start byte of the data received
					pos = 1;
				}
				break;
			// line bytes
			case 1:
				buffer[1] = input;	//first data byte sent from other device.
				pos = 2; 			//set it to check for next byte
				break;
			case 2:
				buffer[2] = input;	//second data byte sent from other device.
				pos = 3;			//set it to check for next byte
				break;
			case 3:
				buffer[3] = input;	//third data byte sent from other device.
				pos = 4;			//set it to check for next byte
				break;
			case 4:
				buffer[4] = input;	//fourth data byte sent from other device.
				pos = 5; 			//set it to check for next byte
				break;
			case 5:
				buffer[5] = input;	//checksum value.
				pos = 6;
				break;
			// stop byte
			case 6:
				if (input == ESPL_StopByte){	//Limiting byte of the data received
					checksum = buffer[4] ^ buffer[3] ^ buffer[2] ^ buffer[1];
					if(checksum==buffer[5])		//Check for errors
					{
						gReceiving = (buffer[4]<<24) | (buffer[3]<<16) | (buffer[2]<<8) | (buffer[1]);
						pos = 0;
					}
					else
					{
						pos = 0;
					}
				}
				else
				{
					pos = 0;
				}
				break;
		}
	}
}

/**
 * Idle hook, definition is needed for FreeRTOS to function.
 */
void VApplicationIdleHook() {
	while (TRUE) {
	};
}
