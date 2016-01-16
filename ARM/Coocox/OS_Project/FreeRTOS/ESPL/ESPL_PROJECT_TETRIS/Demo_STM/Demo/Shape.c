/*
 *
 * This file defines matrices for all shapes, structures for holding and representing all shapes as well
 * as a function for retrieving pieces of elements for a given shape.
 *
 * We gained very good inspiration from:
 * http://javilop.com/gamedev/tetris-tutorial-in-c-platform-independent-focused-in-game-logic-for-beginners/
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 */

#include "Shape.h"


/*-------------------------------Array declaration for all 7 TETRIS shapes---------------------------------------*/
/* Each shape pattern can have 4 possible orientation and each orientation is represented with a 5 by 5 matrix.
 * This leads to an array with a 3 dimensions [4][3][3]
 * [4] - dimension
 * [5] - y coordinate
 * [5] - x coordinate
 *----------------------------------------------------------------------------------------------------------------*/

static const char Array_Square [4][5][5];		//Structure array that defines the shape of a square

static const char Array_I [4][5][5];			//Structure array that defines the I shape.

static const char Array_L [4][5][5];			//Structure array that defines the L shape.

static const char Array_L_Mirrored [4][5][5];	//Structure array that defines the L reversed shape

static const char Array_N [4][5][5];			//Structure array that defines the N shape

static const char Array_N_Mirrored [4][5][5];	//Structure array that defines the N reversed shape

static const char Array_T [4][5][5];			//Structure array that defines the T shape

/*----------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------
* 	@desc:	This function is used by the shape struct function pointer (*GetAPeiceFromShape)
* 			For retrieving a piece from the shape matrix that *arr points to*
*
* 	@param: *arr 				- Pointer to a 3 Dimensional array [4][3][3] that holds the pattern of the shape
* 	@param: x					- X coordinate of shape
*	@param:	y					- Y coordinate of shape
* 	@param:	shapeOrientation	- Orientation of shape
*
* 	@return:					- Returns an element of type 'char' from the shape array
*--------------------------------------------------------------------------------------------------------------*/

char ReturnAnElementFrom3DArray(const char *arr, char shapeOrientation, short Y, short X) {
	return *(arr + (shapeOrientation*25) + (Y*5) + X);
}


/*------------Declaration and initialisation of Shape Structures for-------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------
 * @Desc:	Each shape can have 4 possible orientations, and each orientation of a shape is unique due
 * 			to the different initialisation data such as shape matrix etc. There are 7 shapes in total so this give a
 * 			total of 28 (7 * 4) structures initialised with 7 shapes and their 4 different orientations.
----------------------------------------------------------------------------------------------------------------------*/

shape_t shapeN0 = {&Array_N[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeN1 = {&Array_N[0][0][0],3,-3,1,ReturnAnElementFrom3DArray};
shape_t shapeN2 = {&Array_N[0][0][0],3,-3,2,ReturnAnElementFrom3DArray};
shape_t shapeN3 = {&Array_N[0][0][0],3,-2,3,ReturnAnElementFrom3DArray};


shape_t shapeNMirrored0 = {&Array_N_Mirrored[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeNMirrored1 = {&Array_N_Mirrored[0][0][0],3,-3,1,ReturnAnElementFrom3DArray};
shape_t shapeNMirrored2 = {&Array_N_Mirrored[0][0][0],3,-3,2,ReturnAnElementFrom3DArray};
shape_t shapeNMirrored3 = {&Array_N_Mirrored[0][0][0],3,-2,3,ReturnAnElementFrom3DArray};

shape_t shapeSquare0 = {&Array_Square[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeSquare1 = {&Array_Square[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeSquare2 = {&Array_Square[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeSquare3 = {&Array_Square[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};

shape_t shapeI0 = {&Array_I[0][0][0],3,-2,0,ReturnAnElementFrom3DArray};
shape_t shapeI1 = {&Array_I[0][0][0],3,-3,1,ReturnAnElementFrom3DArray};
shape_t shapeI2 = {&Array_I[0][0][0],3,-2,2,ReturnAnElementFrom3DArray};
shape_t shapeI3 = {&Array_I[0][0][0],3,-3,3,ReturnAnElementFrom3DArray};

shape_t shapeL0 = {&Array_L[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeL1 = {&Array_L[0][0][0],3,-3,1,ReturnAnElementFrom3DArray};
shape_t shapeL2 = {&Array_L[0][0][0],3,-3,2,ReturnAnElementFrom3DArray};
shape_t shapeL3 = {&Array_L[0][0][0],3,-2,3,ReturnAnElementFrom3DArray};

shape_t shapeLMirrored0 = {&Array_L_Mirrored[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeLMirrored1 = {&Array_L_Mirrored[0][0][0],3,-2,1,ReturnAnElementFrom3DArray};
shape_t shapeLMirrored2 = {&Array_L_Mirrored[0][0][0],3,-3,2,ReturnAnElementFrom3DArray};
shape_t shapeLMirrored3 = {&Array_L_Mirrored[0][0][0],3,-3,3,ReturnAnElementFrom3DArray};

shape_t shapeT0 = {&Array_T[0][0][0],3,-3,0,ReturnAnElementFrom3DArray};
shape_t shapeT1 = {&Array_T[0][0][0],3,-3,1,ReturnAnElementFrom3DArray};
shape_t shapeT2 = {&Array_T[0][0][0],3,-3,2,ReturnAnElementFrom3DArray};
shape_t shapeT3 = {&Array_T[0][0][0],3,-2,3,ReturnAnElementFrom3DArray};

/*----------------------------------------------------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------------------------------------------------------------------
* 	@Desc: This function returns an initialised shape struct from 28 (4(orientation) * 7(shapes)) possible initialisation.
*
* 	@Params: randomNumber 	-	A random number between 0 and 27.
* 	@return:				- 	Returns an initialised shape_t struct from 28 possible configurations (7(shapes)*4(orientation)).
*------------------------------------------------------------------------------------------------------------------------------------------------*/
shape_t GetShape(char randomNumber)
{
	shape_t * selectedShape;

	switch(randomNumber)
	{
	case 0:
	selectedShape =  &shapeT0;
	break;
		case 1:
		selectedShape = &shapeT1;
		break;
	case 2:
	selectedShape = &shapeT2;
	break;
		case 3:
		selectedShape = &shapeT3;
		break;

	case 4:
	selectedShape = &shapeN0;
	break;
		case 5:
		selectedShape = &shapeN1;
		break;
	case 6:
	selectedShape = &shapeN2;
	break;
		case 7:
		selectedShape = &shapeN3;
		break;

	case 8:
	selectedShape =  &shapeSquare0;
	break;
		case 9:
		selectedShape = &shapeSquare1;
		break;
	case 10:
	selectedShape = &shapeSquare2;
	break;
		case 11:
		selectedShape = &shapeSquare3;
		break;

	case 12:
	selectedShape = &shapeI0;
	break;
		case 13:
		selectedShape = &shapeI1;
		break;
	case 14:
	selectedShape = &shapeI2;
	break;
		case 15:
		selectedShape = &shapeI3;
		break;

	case 16:
	selectedShape =  &shapeL0;
	break;
		case 17:
		selectedShape = &shapeL1;
		break;
	case 18:
	selectedShape = &shapeL2;
	break;
		case 19:
		selectedShape = &shapeL3;
		break;

	case 20:
	selectedShape = &shapeNMirrored0;
	break;
		case 21:
		selectedShape = &shapeNMirrored1;
		break;
	case 22:
	selectedShape = &shapeNMirrored2;
	break;
		case 23:
		selectedShape = &shapeNMirrored3;
		break;

	case 24:
	selectedShape = &shapeLMirrored0;
	break;
		case 25:
		selectedShape = &shapeLMirrored1;
		break;
	case 26:
	selectedShape = &shapeLMirrored2;
	break;
		case 27:
		selectedShape = &shapeLMirrored3;
		break;
	}
	return *selectedShape;
}




/*-----------------------------------------------------------------------------------------------------------------
 * 									Initialisation of Shape matrices
 * --------------------------------------------------------------------------------------------------------------*/
static const char Array_N [4][5][5] =

  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 4, 0},
    {0, 0, 2, 3, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 1, 2, 0, 0},
					{0, 0, 3, 4, 0},
					{0, 0, 0, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 3, 2, 0, 0},
    {0, 4, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 4, 3, 0, 0},
					{0, 0, 2, 1, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
					}
};
/*------------------------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------Character Array for Shape N mirrored-------------------------------------------*/
static const char Array_N_Mirrored [4][5][5] =

  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 3, 0},
    {0, 0, 0, 4, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 2, 1, 0},
					{0, 4, 3, 0, 0},
					{0, 0, 0, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 4, 0, 0, 0},
    {0, 3, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 3, 4, 0},
					{0, 1, 2, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
					}
   };
/*------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------Character Array for Shape T----------------------------------------------*/
static const char Array_T [4][5][5] =
{
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 4, 0},
    {0, 0, 3, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 3, 2, 1, 0},
					{0, 0, 4, 0, 0},
					{0, 0, 0, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 0, 3, 0, 0},
    {0, 4, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 4, 0, 0},
					{0, 1, 2, 3, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
					}
};
/*-------------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------Character Array for Shape Square--------------------------*/
static const char Array_Square [4][5][5] =
{
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 2, 1, 0},
    {0, 0, 3, 4, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 2, 1, 0},
					{0, 0, 3, 4, 0},
					{0, 0, 0, 0, 0}
					},
   {
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 2, 1, 0},
	{0, 0, 3, 4, 0},
	{0, 0, 0, 0, 0}
	},
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 2, 1, 0},
					{0, 0, 3, 4, 0},
					{0, 0, 0, 0, 0}
					}
};
/*------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------Character Array for Shape I-----------------------------------------*/
static const char Array_I [4][5][5] =

{
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 2, 3, 4},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 1, 0, 0},
					{0, 0, 2, 0, 0},
					{0, 0, 3, 0, 0},
					{0, 0, 4, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {4, 3, 2, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 4, 0, 0},
					{0, 0, 3, 0, 0},
					{0, 0, 2, 0, 0},
					{0, 0, 1, 0, 0},
					{0, 0, 0, 0, 0}
					}
};
/*-----------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------Character Array for Shape L-----------------------------------------------*/
static const char Array_L [4][5][5] =

  {
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 3, 4, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 3, 2, 1, 0},
					{0, 4, 0, 0, 0},
					{0, 0, 0, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 4, 3, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 4, 0},
					{0, 1, 2, 3, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
					}
};
/*---------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------Character Array for Shape L mirrored---------------------------------------------*/
static const char Array_L_Mirrored [4][5][5] =
{
   {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 2, 0, 0},
    {0, 4, 3, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 4, 0, 0, 0},
					{0, 3, 2, 1, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0}
					},
   {
    {0, 0, 0, 0, 0},
    {0, 0, 3, 4, 0},
    {0, 0, 2, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
    },
				   {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 1, 2, 3, 0},
					{0, 0, 0, 4, 0},
					{0, 0, 0, 0, 0}
					}
};
/*-------------------------------------------------------------------------------------------------------------------------------*/
