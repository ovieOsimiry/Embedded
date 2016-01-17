/**
 * Function definitions for the main project file.
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 */

#ifndef Demo_INCLUDED
#define Demo_INCLUDED

#include "stm32f4xx.h"

//Enum type for representing joy stick selection
typedef enum {JoyStickUp , JoyStickDown, JoyStickNoSelection} joystickselection_t;

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
