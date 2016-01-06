#ifndef SHAPE_H
#define SHAPE_H

#include "includes.h"
#include <math.h>
#include "arm_math.h"
//#include "Draw.h"


typedef enum {true=1, false=0} boolean_t;


typedef struct //shape_t_
{
	const char * pArr;//pointer to shape array
	short x;
	short y;
	char shapeOrientation;
	char(*GetAPeiceFromShape)(const char*, char, short,short);
	//char(*GetAPeiceFromShape)(void *,short,short);
}
shape_t;
shape_t GetShape(char randomNumber);

#endif
