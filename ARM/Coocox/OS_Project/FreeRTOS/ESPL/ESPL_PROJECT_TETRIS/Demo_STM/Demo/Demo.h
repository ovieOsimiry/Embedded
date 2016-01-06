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

#endif
