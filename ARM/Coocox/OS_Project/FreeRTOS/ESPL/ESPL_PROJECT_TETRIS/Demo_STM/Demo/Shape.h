#ifndef _SHAPE_
#define _SHAPE_

#include "includes.h"
#include <math.h>
#include "arm_math.h"
#include "Draw.h"


typedef enum {true=1, false=0} boolean_t;



extern char ShapeSquare [4][5][5];
extern char ShapeI [4][5][5];
extern char ShapeL [4][5][5];
extern char ShapeLmirrored [4][5][5];
extern char ShapeN [4][5][5];
extern char ShapeNmirroed [4][5][5];
extern char ShapeT [4][5][5];


char *ShapeContainer[7];

typedef struct _shape_t
{
coord_t  x;
coord_t  y;
char * shapeType;
char * shapeOrientation;
//boolean_t leftMovePossible;
//boolean_t rightMovePossible;
//boolean_t downMovePossible;
//boolean_t rotationPossible;
} shape_t;


//	int PGetBlockType		(int pShape, int pRotation, int pX, int pY);
	int PGetXInitialPosition (int pShape, int pRotation);
	int PGetYInitialPosition (int pShape, int pRotation);
	//void bStoreShape (int pX, int pY, int pShape, int pRotation);

#endif
