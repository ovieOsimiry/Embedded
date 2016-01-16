#ifndef SHAPE_H
#define SHAPE_H

#include "includes.h"
#include <math.h>
#include "arm_math.h"


//Enum definition for boolean type
typedef enum {true=1, false=0} boolean_t;

/*--------------------------------------------------------------------------------------
* @Desc: Structure definition for representing the shapes for TETRIS
* @Params:
* 	*pArr 			:	Pointer to a 3 Dimensional array [4][3][3] that holds the pattern of the shape
* 	 x 				:	x coordinate of shape
* 	 y 				:	y coordinate of shape
* 	shapeOrientation:	Orientation of shape
* 	(*GetAPeiceFromShape) : Function pointer. Used as handle to a function that can retrieve
* 							a peice from the shape matrix that *pArr points to*
*-------------------------------------------------------------------------------------*/
typedef struct
{
	const char * pArr;
	short x;
	short y;
	char shapeOrientation;
	char(*GetAPeiceFromShape)(const char*, char, short,short);
}
shape_t;

//Function declaration for GetShape. used for initialising a shape structure with a shape.
shape_t GetShape(char randomNumber);

#endif
