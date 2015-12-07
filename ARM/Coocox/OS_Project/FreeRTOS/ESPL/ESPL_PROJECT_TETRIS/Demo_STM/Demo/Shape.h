#ifndef _SHAPE_
#define _SHAPE_

#include "includes.h"
#include <math.h>
#include "arm_math.h"
#include "Draw.h"


typedef enum {true=1, false=0} boolean_t;


char *ShapeContainer[7];

typedef struct _shape_t
{
coord_t  x;
coord_t  y;
char * shapeType;
char * shapeOrientation;
} shape_t;


int PGetXInitialPosition (int pShape, int pRotation);
int PGetYInitialPosition (int pShape, int pRotation);
int GetAPeiceFromTheShape(int pShape, int pRotation, int pX, int pY);

#endif
