/**
 * This is the main file of the ESPLaboratory Demo project.
 * It implements simple sample functions for the usage of UART,
 * writing to the display and processing user inputs.
 *
 * @author: Jonathan Müller-Boruttau, Nadja Peters nadja.peters@tum.de (RCS, TUM)
 *
 */

#include "Demo.h"
#include "includes.h"
#include "Draw.h"
#include "Shape.h"
#include "Election.h"
#include "time.h"


// start and stop bytes for the UART protocol
#define ESPL_StartByte 0xAA
#define ESPL_StopByte 0x55

#define HORIZONTAL_CONTROL_MOVE_SPEED 4

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
void sendLine(struct coord coord_1, struct coord coord_2);
void ResetGamePlay();
void CreateNewPiece();
void VApplicationIdleHook();
void sendValue(uint8_t * aByteValue);
int calculateScore(int level, int lines);
//static void uartReceive();
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
int gSending = 0;
shape_t gCurrentShape;
shape_t gNextShape;
joystickselection_t gjoyStickSelection = JoyStickNoSelection;
joystickselection_t gjoyStickLastSelection = JoyStickNoSelection;
boolean_t gSelectButtonPressed = false;
boolean_t gShapeDownMovementSpeedGaurd = false;
boolean_t gSend2PlayerRequestFlag = true;

playermode_t gPlayerMode = onePlayerMode;
/*---------------------Game Play global variables used in GamePlay task-------------*/
#define INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE  5;
const uint8_t TWO_PLAYER_REQUEST_VALUE = 0xeb;
//const int SUBSEQUENT_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE = 10;
uint32_t gDifficultyCheckPoint;

/*------------------------------------------------------------------------------------*/


/*--------------------------RTOS related definitions----------------------------------*/

/*------------------------------Semaphores/Mutexs-------------------------------------*/

//SemaphoreHandle_t xNewShapeCreationMutex;

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
	//Debouncing timer initialization
	timerInit();

	//Initialites states of the game
	startState();


	// Initializes Draw Queue with 100 lines buffer
	DrawQueue = xQueueCreate(100, 4 * sizeof(char));

	// Initializes Tasks with their respective priority
	xTaskCreate(drawTask, "drawTask", 1000, NULL, 4, NULL);
	xTaskCreate(checkJoystick, "checkJoystick", 1000, NULL, 3, NULL);
	//xTaskCreate(uartReceive, "queueReceive", 1000, NULL, 2, NULL);
	xTaskCreate(ReceiveValue, "ReceiveValue", 1000, NULL, 2, NULL);
	xTaskCreate(GamePlay, "GamePlay", 1000, NULL, 3, NULL);
	xTaskCreate(SystemState, "SystemState", 1000, NULL, 3, NULL);

	// Start FreeRTOS Scheduler
	vTaskStartScheduler();
}



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
    int lastState;
    /*---------------------------------------------------------------------------------------*/
	while(1)
	{

			switch(getState())
			{
				case 1:

					if(gjoyStickSelection == JoyStickUp)
						{
						   gPlayerMode = onePlayerMode;
						   gReceiving = 0;
						   gjoyStickLastSelection = JoyStickUp;
						}
					else if(gjoyStickSelection == JoyStickDown)
						{
							uint8_t request = TWO_PLAYER_REQUEST_VALUE;
						    gSend2PlayerRequestFlag = true;// enable flag so that a response can be sent back.
							sendValue(&request);
							//vTaskDelay(5);// wait for response to come.
							if(gReceiving == TWO_PLAYER_REQUEST_VALUE)
							{
								gPlayerMode = twoPlayerMode;
								gjoyStickLastSelection = JoyStickDown;
								gReceiving = 0;//reset received value
							}
						}
					else//(gjoyStickSelection==JoyStickNoSelection)
						{
							if(gReceiving==TWO_PLAYER_REQUEST_VALUE)// && gSend2PlayerRequestFlag == true)
							 {
								uint8_t response = TWO_PLAYER_REQUEST_VALUE;
								gReceiving = 0;
								sendValue(&response);
								gSend2PlayerRequestFlag = false;
							 }
						}
						//if(gjoyStickLastSelection==JoyStickUp){setState(1);}// gjoyStickSelection = JoyStickNoSelection;}
						if (gSelectButtonPressed && gjoyStickLastSelection == JoyStickDown)
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gReceiving = 0;
							setState(2);
						} // gjoyStickLastSelection=JoyStickNoSelection;}

						if (gSelectButtonPressed && gjoyStickLastSelection == JoyStickUp)
						{
							vTaskDelay(_debounceDelay);
							gSelectButtonPressed = 0;
							gReceiving = 0;
							setState(3);
						}
				break;

				case 2:
					gSelectButtonPressed = 0;
					lastState = 2;
				break;

				case 3:
					gSelectButtonPressed = 0;
					lastState = 3;
				break;

				case 4:
					if(gjoyStickSelection==JoyStickUp) {setState(4); gjoyStickSelection = JoyStickNoSelection;}
					else if(gjoyStickSelection==JoyStickDown) {setState(5); gjoyStickSelection = JoyStickNoSelection;}
					if(gSelectButtonPressed)
					{
						if(lastState==3)
							setState(3);
						else if(lastState==2)
							setState(2);
						gSelectButtonPressed = 0;
					};
				break;

				case 5:
					if(gjoyStickSelection==JoyStickUp) {setState(4); gjoyStickSelection = JoyStickNoSelection;}
					else if(gjoyStickSelection==JoyStickDown) {setState(6); gjoyStickSelection = JoyStickNoSelection;}
					if(gSelectButtonPressed) {
						vTaskDelay(_debounceDelay);
						ResetGamePlay();
						if(lastState==3)
							setState(3);
						else if(lastState==2)
							setState(2);
						gSelectButtonPressed = 0;
					};
				break;

				case 6:
					if(gjoyStickSelection==JoyStickUp) {setState(5); gjoyStickSelection = JoyStickNoSelection;}
					else if(gjoyStickSelection==JoyStickDown) {setState(6); gjoyStickSelection = JoyStickNoSelection;}
					if(gSelectButtonPressed)
					{
						vTaskDelay(_debounceDelay);
						ResetGamePlay();
						gSelectButtonPressed = 0;
						setState(1);
					};
				break;


				case 7:
					if(gjoyStickSelection==JoyStickUp)
						gjoyStickLastSelection =JoyStickUp ;
					if(gjoyStickSelection==JoyStickDown)
						gjoyStickLastSelection = JoyStickDown;
					if(gSelectButtonPressed && gjoyStickLastSelection==JoyStickUp)//restart game
					{
						vTaskDelay(_debounceDelay);
						gSelectButtonPressed = 0;

						if(lastState==3)
							setState(3);
						else if(lastState==2)
							setState(2);
					}
					if(gSelectButtonPressed && gjoyStickLastSelection==JoyStickDown)//go to main menu
					{
						vTaskDelay(_debounceDelay);
						gSelectButtonPressed = 0;
						gjoyStickLastSelection=JoyStickUp;
						setState(1);
					}

			}

			vTaskDelay(100);
	}
}


static void GamePlay()
{

		shape_t * ptrShape = &gCurrentShape;
		shape_t shapeTemp;// used for temporary calculations
		//const int MAX_DIFFICULTY_TIMER_VALUE = 1000;
		const int MAX_DIFFICULTY_LEVEL = 9;
		const char MAX_TICK = 100;
		int tick = 0;
		boolean_t downMovePossible = true;
		int tempNoOfLines = 0;
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

/*----------------------------------------------Remove after Debugging----------------------------------------------------*/
		int testingAddLines = 0;
/*--------------------------------------------------------------------------------------------------------------*/


	while(1){
				vTaskDelay(10);
				if(gTotalNumberOfLinesCompleted >= gDifficultyCheckPoint)
				{
					gDifficultyCheckPoint = gDifficultyCheckPoint + (10/(gDifficultyLevel+1));
					if(gDifficultyLevel!=MAX_DIFFICULTY_LEVEL)
					 ++gDifficultyLevel;
				}

				if(getState() == 2) //if we are in 2 player mode, when lines are sent add them.
				{
					switch(gReceiving)
					{
						case 1:
							AddLine(1,ptrShape);
							gReceiving = 0;
						break;
						case 2:
							AddLine(2,ptrShape);
							gReceiving = 0;
						break;
						case 4:
							AddLine(4,ptrShape);
							gReceiving = 0;
						break;
					}
				}

				if(tick >=(MAX_TICK/(gDifficultyLevel+1)))
					tick = 0;
				else
					tick++;

			if( (tick==(MAX_TICK/(gDifficultyLevel+1)) && (getState()==3 || getState()==2) ) ||
				((gjoyStickSelection == JoyStickDown) && (getState()==3  || getState()==2) && (gShapeDownMovementSpeedGaurd == true) ))
				{
					shapeTemp =  *ptrShape;//currentShape;
					shapeTemp.y = shapeTemp.y + 1;
					downMovePossible = IsMoveMentPossible(&shapeTemp);//downMovePossible = IsMoveMentPossible(_shape,x, lastY+1);
					if(downMovePossible==true)
					{
						if (ptrShape->y==20)
							ptrShape->y = 0;
						else
							ptrShape->y+=1;
					}
					else
					{
						StoreShape(ptrShape);
						tempNoOfLines = DeletePossibleLines();
						gTotalNumberOfLinesCompleted += tempNoOfLines;
						gGameScore+=calculateScore(gDifficultyLevel,tempNoOfLines);
						gShapeDownMovementSpeedGaurd=false;
						if(tempNoOfLines>=1 && getState()==2)
							{
								uint8_t temp;
								switch(tempNoOfLines)
								{
								case 2:
									temp = 1;
									gSending = temp;
									sendValue(&temp);
									break;
								case 3:
									temp = 2;
									gSending = temp;
									sendValue(&temp);
									break;
								case 4:
									temp = 4;
									gSending = temp;
									sendValue(&temp);
									break;
								}
							}

						if(!isGameOver())
						 {
							CreateNewShape();//if the game is not over then create a new shape.
						 }
						else
						 {
							setState(7);
						 }
					}
				}
			else if(getState()==7)
			{
				ResetGamePlay();
				tempNoOfLines = 0;//reset temporary local variables within the task.

				/*-------Remove after debugging-----------*/
				testingAddLines = 0;
				gSending = 0;
			}


//			if(testingAddLines == 2000)
//			{
//				testingAddLines = 0;
//				AddLine(1,_shape);
//			}
//			else if(getState()==3)
//			{
//				++testingAddLines;
//			}
		}
}


/**
 * Example task which draws to the display.
 */
static void drawTask() {

//	char str[100]; // Init buffer for message
	struct line line; // Init buffer for line

//	font_t font1; // Load font for ugfx
//	font1 = gdispOpenFont("DejaVuSans32*");
	InitializeBoardMatrix();
	gdispClear(White);



	// Start endless loop
	while (TRUE) {
		// wait for buffer swap
		while (xQueueReceive(DrawQueue, &line, 0) == pdTRUE) {
		 }
		// draw to display
		gdispClear(Black);

		//DrawShape (BOARD_WIDTH/2*BLOCK_SIZE, verticalMove*BLOCK_SIZE, 5, rotation);

		if(getState()==1)
		{
			DrawMainMenu(&gjoyStickLastSelection, &gPlayerMode);
		}
		else if(getState()==3 || getState()==2) {
			//DrawShapeWithHandle(&gCurrentShape);
			//DrawGameFrame(mNextShape,mNextRotation,gTotalNumberOfLinesCompleted,gGameScore,gDifficultyLevel,gReceiving,gSending);
			DrawGameFrame(&gNextShape,gTotalNumberOfLinesCompleted,gGameScore,gDifficultyLevel,gReceiving,gSending);
			DrawShapeWithHandle(&gCurrentShape);
		}
		else if(getState()== 4 || getState() == 5 || getState() == 6){
			DrawPauseMenu();
		}
		else if(getState()==7)
		{
			DrawGameOver(&gjoyStickLastSelection);
		}

		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, 0);
		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, portMAX_DELAY);
		// swap buffers
		ESPL_DrawLayer();
	}
}

void ResetGamePlay()
{
	gGameScore = 0;
	gDifficultyLevel = 0;
	gDifficultyCheckPoint = INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE;//initialise the difficulty level.
	gTotalNumberOfLinesCompleted = 0;
	InitializeBoardMatrix();
	CreateNewShape();
}

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
		default: //more than 4 lines
			return 1200*(level+1)+(lines-4)*40*(level+1);

	}
}
/*External interrupt handler for Button E*/

void EXTI0_IRQHandler(void)//Button E interrupt handler
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
	  timerStart();
	  if(debounce == 0)
	  {
		  if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			  /* Button E will Reset all counters */
			  if(GPIO_ReadInputDataBit(ESPL_Register_Button_E, ESPL_Pin_Button_E)==0){
				  if(getState()!=1)// && getState() != 2)
				  {
					  setState(4);
				  	  debounce = 1;
				  }
			  }
		  }
	  }
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

/*External interrupt handler for Button D*/
void EXTI2_IRQHandler(void)//Button D interrupt handler
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	  {
		if (GPIO_ReadInputDataBit(ESPL_Register_Button_D, ESPL_Pin_Button_D)==0)
		{

		}
	    /* Clear the EXTI line 0 pending bit */
	    EXTI_ClearITPendingBit(EXTI_Line2);
	  }
}

/*External interrupt handler for Button B*/
void EXTI4_IRQHandler(void)
{

	timerStart();
	if(debounce == 0)
	 {
		if(EXTI_GetITStatus(EXTI_Line4) != RESET)
		{
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_B, ESPL_Pin_Button_B)==0)
			{
				gSelectButtonPressed = 1;
				debounce = 1;
			}
			/* Clear the EXTI line 0 pending bit */
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}


/*External interrupt handler for Button A and Button C*/
void EXTI9_5_IRQHandler(void)
{
	timerStart(); //Reset the debouncing timer

	if(debounce == 0){
		if(EXTI_GetITStatus(EXTI_Line5) != RESET)
		{
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_C, ESPL_Pin_Button_C)==0)
			debounce = 1;
			//EXTI_ClearITPendingBit(EXTI_Line5);
		}
		else if(EXTI_GetITStatus(EXTI_Line6) != RESET)
		{
			//if (GPIO_ReadInputDataBit(ESPL_Register_Button_A, ESPL_Pin_Button_A)==0)

			shape_t shapeTemp = gCurrentShape;
			boolean_t rotatePossible = true;
			if(shapeTemp.shapeOrientation==3){
				shapeTemp.shapeOrientation = 0;// if we have reached the max value roll it back to zero.
			}
			else {
				shapeTemp.shapeOrientation = shapeTemp.shapeOrientation + 1;	//increase the rotation.
			}
			rotatePossible = IsMoveMentPossible (&shapeTemp);

			if(rotatePossible==true)//if rotation is possible then assign it to the new rotation
			{
				gCurrentShape.shapeOrientation = shapeTemp.shapeOrientation;
			}
			debounce = 1; //The function is executed once so it will not be executed until the debouncing timer expires
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line5);
}


/**
 * This task polls the joystick value every 20 ticks
 */
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

			shapeTemp = *ptrShape;//make a temporary copy of the shape
			if(joystick_now.x > 150)
			{
				if(rightMove==HORIZONTAL_CONTROL_MOVE_SPEED)
				{
					rightMove = 0;
					shapeTemp.x = shapeTemp.x + 1;//if we have not reached the max value the increase.
				movePossible = IsMoveMentPossible (&shapeTemp);
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
				shapeTemp.x = shapeTemp.x - 1;//lastX = lastX - 1;//if we have not reached the min value then decrease.
				movePossible = IsMoveMentPossible (&shapeTemp);
				if(movePossible==true)
					ptrShape->x = shapeTemp.x;
				}else
				{
					++leftMove;
				}
			}

			if(joystick_now.y > 135) //Joy stick is moved downwards.
			{
				gjoyStickSelection = JoyStickDown;
				//else vTaskDelay(200); //If we are not in the state 3(playing the game), we delay the polling so it doesn't change between options too quick

			}
			else if((joystick_now.y < 110))//joystick moved upwards
			{
				gjoyStickSelection = JoyStickUp;
				if(getState!=3) vTaskDelay(200); //If we are not in the state 3(playing the game), we delay the polling so it doesn't change between options too quick
			}
			else
			{
				gShapeDownMovementSpeedGaurd=true;
				gjoyStickSelection = JoyStickNoSelection;
			}
			vTaskDelayUntil(&xLastWakeTime, 20);
		}
	}

/*************************BUTTON DEBOUNCING TIMER FUNCTIONS**************************************/
//Function executed when the debounce timer expires
void vTimerCallback(TimerHandle_t pxTimer){
	//Allow the next button interruption
	debounce = 0;

	//Stop the timer
	xTimerStop(pxTimer, 0);
}

//Debounce timer setup
void timerInit(){
	//Software timer to debounce the button
	xTimers = xTimerCreate("timer", 200 / portTICK_PERIOD_MS, pdTRUE, (void *)1, vTimerCallback);  //It overflows in 10ms and then it calls the vTimerCallback

	xTimerStart(xTimers, 0);
	xTimerStop(xTimers, 0);

}

void timerStart(){
	//Reset the timer
	xTimerReset(xTimers, 0);
}
/************************************************************************************************/

void sendValue(uint8_t * aByteValue)
{
	static uint8_t xorChar = 0xeb;
	char checksum;
	// Generate simple error detection checksum
	checksum = xorChar^(*aByteValue);
	// Structure of one packet:
	//  Start byte
	//	4 * line byte
	//	checksum (all xor)
	//	End byte
	UART_SendData((uint8_t) ESPL_StartByte);
	UART_SendData((uint8_t) (*aByteValue));
	UART_SendData((uint8_t) checksum);
	UART_SendData((uint8_t) ESPL_StopByte);
}

/**
 * Task which receives data via UART and decodes it.
 */
static void ReceiveValue()
{
	char input;
	static uint8_t xorChar = 0xeb;
	uint8_t pos = 0;
	char checksum;
	char buffer[3]; // Start byte,4* line byte, checksum (all xor), End byte
	//struct line line;
	while (TRUE)
	 {
		// wait for data in queue
		xQueueReceive(ESPL_RxQueue, &input, portMAX_DELAY);
		// decode package by buffer position
		switch (pos) {
			// start byte
			case 0:
				if (input == ESPL_StartByte) {
					buffer[0] = input;
					pos = 1;
				}
				break;
			// line bytes
			case 1:
				buffer[1] = input; //value sent from other device.
				pos = 2;
				break;
			case 2:
				buffer[2] = input; //checksum value.
				pos = 3;
				break;
			case 3:
				if (input == ESPL_StopByte){
					checksum = xorChar ^ buffer[1];
					if(checksum==buffer[2])
					{
						gReceiving = buffer[1];//sendData(buffer[1])
//						if(gReceiving==TWO_PLAYER_REQUEST_VALUE)
//						{
//							uint8_t response = TWO_PLAYER_REQUEST_VALUE;
//							if(gSend2PlayerRequestFlag!=false)
//								{
//									sendValue(&response);
//									gSend2PlayerRequestFlag = false;
//								}
//						}
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
 * Example function to send data over UART
 */
void sendLine(struct coord coord_1, struct coord coord_2) {
	char checksum;
	// Generate simple error detection checksum
	checksum = coord_1.x ^ coord_1.y ^ coord_2.x ^ coord_2.y;
	// Structure of one packet:
	//  Start byte
	//	4 * line byte
	//	checksum (all xor)
	//	End byte
	UART_SendData((uint8_t) ESPL_StartByte);
	UART_SendData((uint8_t) coord_1.x);
	UART_SendData((uint8_t) coord_1.y);
	UART_SendData((uint8_t) coord_2.x);
	UART_SendData((uint8_t) coord_2.y);
	UART_SendData((uint8_t) checksum);
	UART_SendData((uint8_t) ESPL_StopByte);
}

/*
 * Task which receives data via UART and decodes it.
 */
//static void uartReceive() {
//	char input;
//	uint8_t pos = 0;
//	char checksum;
//	char buffer[7]; // Start byte,4* line byte, checksum (all xor), End byte
//	struct line line;
//	while (TRUE) {
//		// wait for data in queue
//		xQueueReceive(ESPL_RxQueue, &input, portMAX_DELAY);
//		// decode package by buffer position
//		switch (pos) {
//			// start byte
//			case 0:
//				if (input == ESPL_StartByte) {
//					buffer[0] = input;
//					pos = 1;
//				}
//				break;
//			// line bytes
//			case 1:
//			case 2:
//			case 3:
//			case 4:
//			// Check sum
//			case 5:
//				buffer[pos] = input;
//				pos++;
//				break;
//			// Last byte should be stop byte
//			case 6:
//				if (input == ESPL_StopByte) {
//					// Check if checksum is accurate
//					checksum = buffer[1] ^ buffer[2] ^ buffer[3] ^ buffer[4];
//					// Decode packet to line struct
//					if (buffer[5] == checksum) {
//						line.x_1 = buffer[1] / 2;
//						line.y_1 = buffer[2] / 2;
//						line.x_2 = buffer[3] / 2;
//						line.y_2 = buffer[4] / 2;
//						// Send line to be drawn
//						xQueueSend(DrawQueue, &line, 100);
//						pos = 0;
//					} else {
//						// Reset buffer
//						pos = 0;
//					}
//				} else {
//					pos = 0;
//				}
//		}
//	}
//}

/**
 * Idle hook, definition is needed for FreeRTOS to function.
 */
void VApplicationIdleHook() {
	while (TRUE) {
	};
}
