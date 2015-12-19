/**
 * This is the main file of the ESPLaboratory Demo project.
 * It implements simple sample functions for the usage of UART,
 * writing to the display and processing user inputs.
 *
 * @author: Jonathan Müller-Boruttau, Nadja Peters nadja.peters@tum.de (RCS, TUM)
 *
 */

#include "includes.h"
#include "Draw.h"
#include "Shape.h"
#include "Election.h"
#include "time.h"


// start and stop bytes for the UART protocol
#define ESPL_StartByte 0xAA
#define ESPL_StopByte 0x55

#define HORIZONTAL_CONTROL_MOVE_SPEED 4

int mPosX, mPosY;				// Position of the piece that is falling down
int mShape, mRotation;
int mNextPosX, mNextPosY;		// Position of the next piece
int mNextShape, mNextRotation;	// Kind and rotation of the next piece

int debounce = 0; //Restricts the execution of the IRQ_handler if the debouncing time hasn't expired


QueueHandle_t ESPL_RxQueue; // Already defined in ESPL_Functions.h
SemaphoreHandle_t ESPL_DisplayReady;


extern char verticalMove;
extern char rotation;

typedef struct _joyStickCorrd
{
	coord_t x_joy;
	coord_t y_joy;
}JoyStickCord_t;

JoyStickCord_t JoyStickVal = {0,0};
// Stores lines to be drawn
QueueHandle_t DrawQueue;


/*-----------------Please put all shared global variables here------------------------*/

shape_t Shape;
int gTotalNumberOfLinesCompleted = 0;
int gGameScore = 0;
int gDifficultyLevel;
int gReceiving = 0;
int gSending = 0;
joystickselection_t gjoyStickSelection = JoyStickNoSelection;
bool_t gSelectButtonPressed = false;
/*---------------------Game Play gloabal variables used in GamePlay task-------------*/
#define INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE  5;
//const int SUBSEQUENT_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE = 10;
uint32_t gDifficultyCheckPoint;

/*------------------------------------------------------------------------------------*/


/*--------------------------RTOS related definitions----------------------------------*/

/*------------------------------Semaphores/Mutexs-------------------------------------*/

SemaphoreHandle_t xNewShapeCreationMutex;

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
	xNewShapeCreationMutex = xSemaphoreCreateMutex();
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
	xTaskCreate(GamePlay, "GamePlay", 500, NULL, 3, NULL);
	xTaskCreate(SystemState, "SystemState", 500, NULL, 3, NULL);

	// Start FreeRTOS Scheduler
	vTaskStartScheduler();
}



void CreateNewShape()
{
	// The new shape
	mShape		= mNextShape;
	mRotation	= mNextRotation;
	mPosX 		= (BOARD_WIDTH_IN_BLOCKS / 2) + pGetXInitialPosition (mShape, mRotation);
	mPosY 		= pGetYInitialPosition (mShape, mRotation);
	int randNum;
	TM_RNG_Init();
	randNum = TM_RNG_Get() & 0x7;
	while(randNum>6)
	{
		randNum = TM_RNG_Get() & 0x7;
	}
	mNextShape 		=  randNum; // (rand() % (max+1-min))+min;
	randNum = TM_RNG_Get() & 0x7;
	while(randNum>3)
	{
		randNum = TM_RNG_Get() & 0x7;
	}
	mNextRotation 	=  randNum; // (rand() % (max+1-min))+min;
}

void UpdateShape(){
	Shape.shapeOrientation = mRotation;
	Shape.shapeType = mShape;
	Shape.x = (BOARD_WIDTH_IN_BLOCKS / 2) + pGetXInitialPosition(mShape, mRotation);
	Shape.y = pGetYInitialPosition (mShape, mRotation);
}


static void SystemState()
{
	while(1)
	{

			switch(getState())
			{
				case 1:
					if(gjoyStickSelection==JoyStickUp) setState(1);
					else if(gjoyStickSelection==JoyStickDown) setState(2);
					if(gSelectButtonPressed) {setState(3); gSelectButtonPressed = 0;};
				break;

				case 2:
					if(gjoyStickSelection==JoyStickDown) setState(2);
					else if(gjoyStickSelection==JoyStickUp) setState(1);
					if(gSelectButtonPressed) {setState(3); gSelectButtonPressed = 0;};
				break;

				case 3:
					gSelectButtonPressed = 0;
				break;

				case 7:
					if(gSelectButtonPressed && gjoyStickSelection==JoyStickUp) {setState(3); gSelectButtonPressed = 0;}
					if(gSelectButtonPressed && gjoyStickSelection==JoyStickDown) {setState(1); gSelectButtonPressed = 0;}

			}

			vTaskDelay(100);
	}
}


static void GamePlay()
{

		shape_t * _shape = &Shape;
		coord_t lastY = 0;
		const int MAX_DIFFICULTY_TIMER_VALUE = 1000;
		const int MAX_DIFFICULTY_LEVEL = 9;
		const char MAX_TICK = 100;
		int tick = 0;
		boolean_t downMovePossible = true;
		int tempNoOfLines = 0;

		CreateNewShape();
		UpdateShape();

/*--------Initialise global variables used within this task----------------*/

		gDifficultyLevel = 0;
		gDifficultyCheckPoint = (uint32_t) INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE;

/*-------------------------------------------------------------------------*/
		int testSending = 0;
	while(1){
				vTaskDelay(10);
				if(gTotalNumberOfLinesCompleted >= gDifficultyCheckPoint)
				{
					gDifficultyCheckPoint = gDifficultyCheckPoint + (10/(gDifficultyLevel+1));
					if(gDifficultyLevel!=MAX_DIFFICULTY_LEVEL)
					 ++gDifficultyLevel;
				}

				if(tick >=(MAX_TICK/(gDifficultyLevel+1)))
					tick = 0;
				else
					tick++;

			if(tick==(MAX_TICK/(gDifficultyLevel+1)) && getState()==3)
				{
					lastY = _shape->y;
					downMovePossible = IsMoveMentPossible (_shape->x, lastY+1, _shape->shapeType, _shape->shapeOrientation);
					if(downMovePossible==true)
					{
						if (_shape->y==20)
							_shape->y = 0;
						else
							_shape->y+=1;
					}
					else
					{
						StoreShape(_shape->x, _shape->y, _shape->shapeType, _shape->shapeOrientation);
						tempNoOfLines = DeletePossibleLines();
						gTotalNumberOfLinesCompleted += tempNoOfLines;
						gGameScore+=calculateScore(gDifficultyLevel,tempNoOfLines);

						if(tempNoOfLines>=1)
							{
								gSending = ++testSending;//(uint8_t)(rand()%4);
								sendValue((uint8_t)gSending);
							}

						if(!isGameOver())
							{
								if( xNewShapeCreationMutex != NULL )
									{
									//Since the shape can be updated from another task, we try to take mutex before updating shape
									//if we wait for mutex to become available for at most 1 tick, and it is not available then we don't
									//bother to update shape because the other task must have already done so.
									if( xSemaphoreTake( xNewShapeCreationMutex, ( TickType_t )1) == pdTRUE )
										{
											CreateNewShape();
											UpdateShape();
											xSemaphoreGive( xNewShapeCreationMutex );
										}
									}
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
			}
		}
}


/**
 * Example task which draws to the display.
 */
static void drawTask() {

	char str[100]; // Init buffer for message
	struct line line; // Init buffer for line

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");
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

		if(getState()==1||getState()==2)
		{
			DrawMainMenu();
		}
		else if(getState()==3) {
			DrawShapeWithHandle(&Shape);
			DrawBoardMatrix(mNextShape,mNextRotation,gTotalNumberOfLinesCompleted,gGameScore,gDifficultyLevel,gReceiving,gSending);
		}
		else if(getState()==7)
		{
			DrawGameOver(&gjoyStickSelection);
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
	InitializeBoardMatrix();
	gGameScore = 0;
	gDifficultyLevel = 0;
	gDifficultyCheckPoint = INITIAL_NO_OF_LINES_DETERMINING_DIFFICULTY_CHANGE;//initialise the difficulty level.
	gTotalNumberOfLinesCompleted = 0;
//	gSelectButtonPressed = false;
//	gjoyStickSelection = JoyStickUp;
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
    /* Button E will Reset all counters */
	  if(GPIO_ReadInputDataBit(ESPL_Register_Button_E, ESPL_Pin_Button_E)==0)
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
			/* Increase count for button D */
			debounce == 1;
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_B, ESPL_Pin_Button_B)==0)
			{
	//			if(getState()==1||getState()==2)
	//			{
	//				if(getState()==1)
	//				{
	//					setState(3);
	//				}
	//				if(getState()==2)
	//				{
	//					setState(4);
	//				}
	//			}
	//			else {
	//			}
				gSelectButtonPressed = 1;

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

			coord_t lastOrientation = 0;
			boolean_t rotatePossible = true;
			lastOrientation = Shape.shapeOrientation;
			if(lastOrientation==3){
				lastOrientation = 0;// if we have reached the max value roll it back to zero.
			}
			else {
				lastOrientation = lastOrientation + 1;	//increase the rotation.
			}
			rotatePossible = IsMoveMentPossible (Shape.x, Shape.y, Shape.shapeType, lastOrientation);

			if(rotatePossible==true)//if rotation is possible then assign it to the new rotation
			{
				Shape.shapeOrientation = lastOrientation;
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
		struct coord joystick_now = { 0, 0 }, joystick_last = { 0, 0 };

		unsigned int X_sensitivity = 0;
		unsigned int Y_sensitivity = 0;


		coord_t lastX = 0;
		coord_t lastY = 0;
		boolean_t movePossible = true;
		shape_t * _shape = &Shape;
		boolean_t shapeDownMovementSpeedGaurd = false;
		int leftMove = 0;
		int rightMove = 0;
		lastX = Shape.x;
		boolean_t downMovePossible = true;
		int temp = 0;

		while (TRUE) {
			// Remember last joystick values
			joystick_last = joystick_now;
			joystick_now.x = (uint8_t) (ADC_GetConversionValue(ESPL_ADC_Joystick_2) >> 4);
			joystick_now.y = (uint8_t) 255 - (ADC_GetConversionValue(ESPL_ADC_Joystick_1) >> 4);
			//##
			lastX = Shape.x;
			if(joystick_now.x > 150)
			{
				if(rightMove==HORIZONTAL_CONTROL_MOVE_SPEED)
				{
					rightMove = 0;
					lastX = lastX + 1;//if we have not reached the max value the increase.
				movePossible = IsMoveMentPossible (lastX, Shape.y, Shape.shapeType, Shape.shapeOrientation);
				if(movePossible==true)
					Shape.x = lastX;
				}
				else
					++rightMove;
			}
			else if((joystick_now.x < 120))
			{

				if(leftMove==HORIZONTAL_CONTROL_MOVE_SPEED)
				{
				leftMove = 0;
				lastX = lastX - 1;//if we have not reached the min value then decrease.
				movePossible = IsMoveMentPossible (lastX, Shape.y, Shape.shapeType, Shape.shapeOrientation);
				if(movePossible==true)
					Shape.x = lastX;
				}else
				{
					++leftMove;
				}
			}

			if(joystick_now.y > 135) //Joy stick is moved downwards.
			{
				gjoyStickSelection = JoyStickDown;
				if(getState()==3) {
					if(shapeDownMovementSpeedGaurd==true)//check guard to see if the shape has already touched the base of the board
					{
						lastY = _shape->y;
						downMovePossible = IsMoveMentPossible (_shape->x, lastY+1, _shape->shapeType, _shape->shapeOrientation);
						if(downMovePossible==true)
						{
							if (_shape->y==20)
							_shape->y = 0;
							else
							_shape->y+=1;
						}
						else
						{
							shapeDownMovementSpeedGaurd=false;//the shape has reached the bottom so we set the guard to false to prevent the next shape from droping fast until the joystick is released
							StoreShape (_shape->x, _shape->y, _shape->shapeType, _shape->shapeOrientation);
							temp = DeletePossibleLines();
							gTotalNumberOfLinesCompleted += temp;
							gGameScore+=calculateScore(gDifficultyLevel,temp);

							if(!isGameOver())//check if the game is over.
							{
								if( xNewShapeCreationMutex != NULL )
									{
									//Since the shape can be updated from another task, we try to take mutex before updating shape
									//if we wait for mutex to become available for at most 1 tick, and it is not available then we don't
									//bother to update shape because the other task must have already done so.
									if( xSemaphoreTake( xNewShapeCreationMutex, ( TickType_t )1) == pdTRUE )
										{
											CreateNewShape();
											UpdateShape();
											xSemaphoreGive( xNewShapeCreationMutex );
										}
									}
							}
						}
					}
				}

			}
			else if((joystick_now.y < 110))//joystick moved upwards
			{
				gjoyStickSelection = JoyStickUp;
			}
			else
			{
				shapeDownMovementSpeedGaurd=true;
			}

			// Send over UART
			//sendLine(joystick_last, joystick_now);
			// Execute every 20 Ticks
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

void sendValue(uint8_t numberOfLines)
{
	static uint8_t xorChar = 0xeb;
	char checksum;
	// Generate simple error detection checksum
	checksum = xorChar^numberOfLines;
	// Structure of one packet:
	//  Start byte
	//	4 * line byte
	//	checksum (all xor)
	//	End byte
	UART_SendData((uint8_t) ESPL_StartByte);
	UART_SendData((uint8_t) numberOfLines);
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
	struct line line;
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
						//xQueueSend(DrawQueue, &line, 100);
//						pos = 0;
//					} else {
//						// Reset buffer
//						pos = 0;
//					}
//				} else {
//					pos = 0;
//				}
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
static void uartReceive() {
	char input;
	uint8_t pos = 0;
	char checksum;
	char buffer[7]; // Start byte,4* line byte, checksum (all xor), End byte
	struct line line;
	while (TRUE) {
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
			case 2:
			case 3:
			case 4:
			// Check sum
			case 5:
				buffer[pos] = input;
				pos++;
				break;
			// Last byte should be stop byte
			case 6:
				if (input == ESPL_StopByte) {
					// Check if checksum is accurate
					checksum = buffer[1] ^ buffer[2] ^ buffer[3] ^ buffer[4];
					// Decode packet to line struct
					if (buffer[5] == checksum) {
						line.x_1 = buffer[1] / 2;
						line.y_1 = buffer[2] / 2;
						line.x_2 = buffer[3] / 2;
						line.y_2 = buffer[4] / 2;
						// Send line to be drawn
						xQueueSend(DrawQueue, &line, 100);
						pos = 0;
					} else {
						// Reset buffer
						pos = 0;
					}
				} else {
					pos = 0;
				}
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
