#ifndef _SHAPE_
#define _SHAPE_
#include "Shape.h"


/**
 *
 * Here we define each shape as an array of characters
 * Each array contains the possible position of the shape when it is rotated.
 * We gained very good inspiration on how to completely implement the shapes
 * from: http://javilop.com/gamedev/tetris-tutorial-in-c-platform-independent-focused-in-game-logic-for-beginners/
 *
 */

char ShapeSquare [4][5][5]; //Structure array that defines the shape of a square

char ShapeI [4][5][5];		//Structure array that defines the I shape.

char ShapeL [4][5][5];		//Structure array that defines the L shape.

char ShapeLmirrored [4][5][5];	//Structure array that defines the L reversed shape

char ShapeN [4][5][5];			//Structure array that defines the N shape

char ShapeNmirroed [4][5][5];	//Structure array that defines the N reversed shape

char ShapeT [4][5][5];			//Structure array that defines the T shape

/*------------------------------------Define the container that holds the shapes------------------------*/

char *ShapeContainer[] = {&ShapeSquare,&ShapeI,&ShapeL,&ShapeLmirrored,&ShapeN,&ShapeNmirroed, &ShapeT};

/*------------------------------------Create A container that holds the shapes------------------------*/


/*-----------------------------------------------------------------------------------------------------
 * int pShape => The shape (0 to 6)
 * int pRotation => The orientation Matrix of the Shape
 * int pX => The Shape matrix
 * int pY => The Shape element
 *----------------------------------------------------------------------------------------------------*/
int GetAPeiceFromTheShape (int pShape, int pRotation, int pX, int pY)
{
	/*This function pulls a single piece from the shape requested
	 *The Array of pointers holds all the shapes. The shapes are multidimentional arrays
	 *and they are normally stored in a contigous manner in memory.
	 *This function works by returning the start address of a particular shape.
	 *
	 *Then the constant 25 is multiplied with the pRotation and it creates the offset to select the correct rotation array.
	 *The constant 5 is multiplied with pX to create the correct offset for the shape matrix.
	 *The variable pY simple represent the Piece of the shape that we are interested in.
	 *This is simple the element of the last array.
	 * */

	return *(ShapeContainer[pShape]+(pRotation*25)+(pX*5)+pY);
}
/*------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------Character Array for Shape Shape N----------------------------------------------*/

char ShapeN [4][5][5] =

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
char ShapeNmirroed [4][5][5] =

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
char ShapeT [4][5][5] =
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
char ShapeSquare [4][5][5] =
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
char ShapeI [4][5][5] =

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
char ShapeL [4][5][5] =

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
char ShapeLmirrored [4][5][5] =
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



// Displacement of the piece to the position where it is first drawn in the board when it is created
int mShapesInitialPosition  [7 /*kind */ ][4 /* rotation */ ][2 /* position */] =
{
/* Square */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -3}
   },
/* I */
  {
	{-2, -2},
    {-2, -3},
    {-2, -2},
    {-2, -3}
   },
/* L */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* L mirrored */
  {
	{-2, -3},
    {-2, -2},
    {-2, -3},
    {-2, -3}
   },
/* N */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* N mirrored */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
/* T */
  {
	{-2, -3},
    {-2, -3},
    {-2, -3},
    {-2, -2}
   },
};



int pGetXInitialPosition (int pShape, int pRotation)
{
	return mShapesInitialPosition [pShape][pRotation][0];
}


int pGetYInitialPosition (int pShape, int pRotation)
{
	return mShapesInitialPosition [pShape][pRotation][1];
}


#endif
