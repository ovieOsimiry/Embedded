/**
 * This file contains routines for drawing the shapes for the Tetris game
 * Some inspiration was gained from the opensource project in the following link
 * http://javilop.com/gamedev/tetris-tutorial-in-c-platform-independent-focused-in-game-logic-for-beginners/
 * writing to the display and processing user inputs.
 *
 * @author: Natalia, Edu and Ovie.
 *
 */

#include "Draw.h"
#include "Shape.h"


int BoardMatrix [BOARD_WIDTH_IN_BLOCKS][BOARD_HEIGHT_IN_BLOCKS];

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

/*-------------------------------------------------------------------------------------------------------
 * This function clears the board Matrix by initializing all elements to 0
 *
 * -----------------------------------------------------------------------------------------------------*/
void InitializeBoardMatrix()
{
	for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
		for (int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
			BoardMatrix[i][j] = 0;
}
/*-------------------------------------------------------------------------------------------------------*/


void DrawShapeWithHandle(shape_t * shape)
{
	color_t mColor;				// Color of the block

	// Obtain the position in pixel in the screen of the block we want to draw
	int mPixelsX = shape->x;
	int mPixelsY = shape->y;

	// Travel the matrix of blocks of the piece and draw the blocks that are filled
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (GetAPeiceFromTheShape (shape->shapeType, shape->shapeOrientation, j, i))
			{
				case 1: mColor = Red; break;	// For each block of the piece except the pivot
				case 2: mColor = Yellow; break;	// For the pivot
				case 3: mColor = Green; break;	// For the pivot
				case 4: mColor = Blue; break;	// For the pivot
			}

			if (GetAPeiceFromTheShape (shape->shapeType, shape->shapeOrientation, j, i) != 0)
			gdispFillArea(((mPixelsX+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,((mPixelsY+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,mColor);

		}
	}
}


void DrawBoardMatrix()
{

	char blockType = 0;
	int mX1 = 0;
	int mX2 = BOARD_WIDTH;
	int mY = 0;

	gdispFillArea(BOARD_OFFSET_X_AXIS,mY,BOARDER_THICKNESS,SCREEN_HEIGHT-BOARDER_THICKNESS,Blue);//left boarder
	gdispFillArea(BOARD_OFFSET_X_AXIS + BOARD_WIDTH + BOARDER_THICKNESS,mY,BOARDER_THICKNESS,SCREEN_HEIGHT-BOARDER_THICKNESS,Blue);//right boarder
	gdispFillArea(BOARD_OFFSET_X_AXIS,230,(BOARDER_THICKNESS+BOARD_WIDTH+BOARDER_THICKNESS),BOARDER_THICKNESS,Blue);//bottom boarder

	// Drawing the blocks that are already stored in the board
	mX1 += 1;

	for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
		{
			// Check if the block is filled, if so, draw it
			blockType = GetPeiceType(i, j);
			if (blockType!=0)
				gdispFillArea(((mX1+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS,((mY+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,GetColor(blockType));

		}
	}
}


color_t GetColor(char color)
{
	switch(color)
	{
		case 0:
		return White;

		case 1:
		return Red;

		case 2:
		return Yellow;

		case 3:
		return Green;

		case 4:
		return Blue;

		default:
			return White;
	}
}


char GetPeiceType(int pX, int pY)
{
	return BoardMatrix [pX][pY];
}

boolean_t IsMoveMentPossible (int pX, int pY, int pShape, int pRotation)
{
	// Checks collision with pieces already stored in the board or the board limits
	// This is just to check the 5x5 blocks of a piece with the appropiate area in the board

	boolean_t status = true;
	int blocktype = 0;
	for (int i1 = pX, i2 = 0; i1 < pX + SHAPE_MATRIX_DIMENSION; i1++, i2++)
	{
		for (int j1 = pY, j2 = 0; j1 < pY + SHAPE_MATRIX_DIMENSION; j1++, j2++)
		{
			// Check if the shape is outside the limits of the board
			if(	i1 < 0 	||	i1 > BOARD_WIDTH_IN_BLOCKS  - 1	|| 	j1 > BOARD_HEIGHT_IN_BLOCKS - 1)
			{
				if (GetAPeiceFromTheShape (pShape, pRotation, j2, i2) != 0)
					return 0;
			}

			// Check if the piece have collisioned with a block already stored in the map
			if (j1 >= 0)
			{
				status = GetPeiceType(i1, j1);
				blocktype = GetAPeiceFromTheShape (pShape, pRotation, j2, i2);
				if ((blocktype != 0) && (status!=0))
					return false;
			}
		}
	}

	// No collision
	return true;
}


void StoreShape (int pX, int pY, int pShape, int pRotation)
{

	char blockType = 0;
	for (int i1 = pX, i2 = 0; i1 < pX + SHAPE_MATRIX_DIMENSION; i1++, i2++)
	{
		for (int j1 = pY, j2 = 0; j1 < pY + SHAPE_MATRIX_DIMENSION; j1++, j2++)
		{
			blockType = GetAPeiceFromTheShape (pShape, pRotation, j2, i2);
			if (blockType!= 0)
				BoardMatrix[i1][j1] = blockType;
		}
	}
}


void DeleteLine (int pY)
{
	// Moves all the upper lines one row down
	for (int j = pY; j > 0; j--)
	{
		for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
		{
			BoardMatrix[i][j] = BoardMatrix[i][j-1];
		}
	}
}


int DeletePossibleLines ()
{
	char blockType = 0;
	int numOfLinesFilled = 0;
	for (int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
	{
		int i = 0;
		while (i < BOARD_WIDTH_IN_BLOCKS)
		{
			blockType = BoardMatrix[i][j];
		if (blockType != 0) break;
			i++;
		}

		if (i == BOARD_WIDTH_IN_BLOCKS){
			DeleteLine (j);
			++numOfLinesFilled;
		}
	}
	return numOfLinesFilled;
}
