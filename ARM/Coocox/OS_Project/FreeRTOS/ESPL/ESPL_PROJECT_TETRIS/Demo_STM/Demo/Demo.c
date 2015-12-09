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


// start and stop bytes for the UART protocol
#define ESPL_StartByte 0xAA
#define ESPL_StopByte 0x55

//#define dispMovSpeed	5
#define HORIZONTAL_CONTROL_MOVE_SPEED 4

int mPosX, mPosY;				// Position of the piece that is falling down
int mShape, mRotation;
int mNextPosX, mNextPosY;		// Position of the next piece
int mNextShape, mNextRotation;	// Kind and rotation of the next piece

int debounce = 0; //Restricts the execution of the IRQ_handler if the debouncing time hasn't expired
TimerHandle_t xTimers; //Define the debouncing timer

static void delay(unsigned int nCount);


QueueHandle_t ESPL_RxQueue; // Already defined in ESPL_Functions.h
SemaphoreHandle_t ESPL_DisplayReady;


static int btnACount = 0;
static int btnBCount = 0;
static int btnCCount = 0;
static int btnDCount = 0;
static int btnECount = 0;
static volatile signed int gbLeftRight = 0;

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

/**
 * Entry function to the example program.
 * Initializes hardware and software.
 * Starts scheduler.
 */

shape_t Shape;
int Score_NumberOfLinesCompleted = 0;


int main() {
	// Initialize Board functions
	ESPL_SystemInit();

	//Debouncing timer initialization
	timerInit();

	CreateNewShape();
	UpdateShape();


	// Initializes Draw Queue with 100 lines buffer
	DrawQueue = xQueueCreate(100, 4 * sizeof(char));

	// Initializes Tasks with their respective priority
	xTaskCreate(drawTask, "drawTask", 1000, NULL, 4, NULL);
	xTaskCreate(checkJoystick, "checkJoystick", 1000, NULL, 3, NULL);
	xTaskCreate(uartReceive, "queueReceive", 1000, NULL, 2, NULL);
	xTaskCreate(GamePlay, "GamePlay", 500, NULL, 3, NULL);
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

	// Random next shape
	mNextShape 		=  (rand() % (7)); // (rand() % (max+1-min))+min;
	mNextRotation 	=  (rand() % (4)); // (rand() % (max+1-min))+min;
}

void UpdateShape(){
	Shape.shapeOrientation = mRotation;
	Shape.shapeType = mShape;
	Shape.x = (BOARD_WIDTH_IN_BLOCKS / 2) + pGetXInitialPosition(mShape, mRotation);
	Shape.y = pGetYInitialPosition (mShape, mRotation);
}


static void GamePlay()
{
	shape_t * _shape = &Shape;
	coord_t lastY = 0;
	boolean_t downMovePossible = true;
	int tick = 0;
	while(1){

		vTaskDelay(10);

		if(tick==100)
			tick = 0;
		else
			tick++;

		if(tick==100){
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
				StoreShape (_shape->x, _shape->y, _shape->shapeType, _shape->shapeOrientation);
				Score_NumberOfLinesCompleted = DeletePossibleLines();
				CreateNewShape();
				UpdateShape();
			}

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
	font1 = gdispOpenFont("DejaVuSans24*");
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


		DrawShapeWithHandle(&Shape);
		DrawBoardMatrix();

		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, 0);
		// Wait for display to stop writing
		xSemaphoreTake(ESPL_DisplayReady, portMAX_DELAY);
		// swap buffers
		ESPL_DrawLayer();
	}
}



/*External interrupt handler for Button E*/

void EXTI0_IRQHandler(void)//Button E interrupt handler
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    /* Button E will Reset all counters */
	  delay(6);//debounce
	  if(GPIO_ReadInputDataBit(ESPL_Register_Button_E, ESPL_Pin_Button_E)==0)
		  btnACount = 0; btnBCount = 0; btnCCount = 0; btnDCount = 0;
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

/*External interrupt handler for Button D*/
void EXTI2_IRQHandler(void)//Button D interrupt handler
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	  {
	    /* Increase count for button D */
		 delay(6);//debounce
		if (GPIO_ReadInputDataBit(ESPL_Register_Button_D, ESPL_Pin_Button_D)==0)
		{
			btnDCount++;
			gbLeftRight = gbLeftRight - 10;
		}
	    /* Clear the EXTI line 0 pending bit */
	    EXTI_ClearITPendingBit(EXTI_Line2);
	  }
}

/*External interrupt handler for Button B*/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	  {
		/* Increase count for button D */
		 delay(6);//debounce
		if (GPIO_ReadInputDataBit(ESPL_Register_Button_B, ESPL_Pin_Button_B)==0)
		{
			btnBCount++;
			gbLeftRight = gbLeftRight + 10;
		}
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line4);
	  }
}

int ButtonADelay = 0;

/*External interrupt handler for Button A and Button C*/
void EXTI9_5_IRQHandler(void)
{
	timerStart(); //Reset the debouncing timer

	if(debounce == 0){
		if(EXTI_GetITStatus(EXTI_Line5) != RESET)
		{
			if (GPIO_ReadInputDataBit(ESPL_Register_Button_C, ESPL_Pin_Button_C)==0)
			btnCCount++;
			debounce = 1;
			EXTI_ClearITPendingBit(EXTI_Line5);
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
}

static void delay(unsigned int nCount)
{
  unsigned int index = 0;
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
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
		boolean_t movePossible = true;
		int leftMove = 0;
		int rightMove = 0;
		lastX = Shape.x;


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

			if(joystick_now.y > 135)
			{

			}
			else if((joystick_now.y < 110))
			{

			}

			// Send over UART
			sendLine(joystick_last, joystick_now);
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
	xTimers = xTimerCreate("timer", 30 / portTICK_PERIOD_MS, pdTRUE, (void *)1, vTimerCallback);  //It overflows in 10ms and then it calls the vTimerCallback

	xTimerStart(xTimers, 0);
	xTimerStop(xTimers, 0);

}

void timerStart(){
	//Reset the timer
	xTimerReset(xTimers, 0);
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

/**
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
