/**
 * Function definitions for the main project file.
 *
 * @author: Jonathan Müller-Boruttau, Nadja Peters nadja.peters@tum.de (RCS, TUM)
 */

#ifndef Demo_INCLUDED
#define Demo_INCLUDED

#include "stm32f4xx.h"

typedef enum {JoyStickUp , JoyStickDown, JoyStickNoSelection} joystickselection_t;
typedef enum {onePlayerMode, twoPlayerMode} playermode_t;

struct line
	{
		char x_1;
		char y_1;
		char x_2;
		char y_2;
	};
	struct coord
	{
		char x;
		char y;
	};





static void uartReceive();
void sendLine(struct coord coord_1, struct coord coord_2);
static void checkJoystick();
static void drawTask();
static void GamePlay();
static void SystemState();
void ResetGamePlay();
void CreateNewPiece();
void UpdateShape();
void VApplicationIdleHook();
void sendValue(uint8_t * aByteValue);
static void ReceiveValue();

#endif
