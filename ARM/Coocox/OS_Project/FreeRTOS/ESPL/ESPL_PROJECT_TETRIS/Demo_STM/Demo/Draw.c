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


int BoardMatrix [BOARD_WIDTH_IN_BLOCKS][BOARD_HEIGHT_IN_BLOCKS];

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
				case 4: mColor = Orange; break;	// For the pivot
			}

			if (GetAPeiceFromTheShape (shape->shapeType, shape->shapeOrientation, j, i) != 0)
			gdispFillArea(((mPixelsX+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,((mPixelsY+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,mColor);

		}
	}
}

//Draws next block to be playable into the <next> box
void DrawNextShape(int type, int orientation)
{
	color_t mColor;				// Color of the block
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (GetAPeiceFromTheShape (type, orientation, j, i))
			{
				case 1: mColor = Red; break;	// For each block of the piece except the pivot
				case 2: mColor = Yellow; break;	// For the pivot
				case 3: mColor = Green; break;	// For the pivot
				case 4: mColor = Orange; break;	// For the pivot
			}

			if (GetAPeiceFromTheShape (type, orientation, j, i) != 0)
			gdispFillArea(i*BLOCK_SIZE+170+60,j*BLOCK_SIZE+0+175,BLOCK_SIZE,BLOCK_SIZE,mColor);
		}
	}
}
void DrawBoardMatrix(int nextShape,int nextRotation,int lines, int score, int level, int receiving, int sending)
{
	char str[100]; // Init buffer for message
	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");
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

    gdispFillArea(170+30-5, 0+20-10 , 100+10,30+10, Blue);
    gdispFillArea(170+30-5, 0+70-10 , 100+10,30+10, Blue);
    gdispFillArea(170+30-5, 0+120-10 , 100+10,30+10, Blue);
    gdispFillArea(170+30-5, 0+170-10 , 100+10,60+10, Blue);

    gdispFillArea(170+30, 0+20-5 , 100,30, Black);
    gdispFillArea(170+30, 0+70-5 , 100,30, Black);
    gdispFillArea(170+30, 0+120-5 , 100,30, Black);
    gdispFillArea(170+30, 0+170-5 , 100,60, Black);

    sprintf(str, "Score");
    // Print string
    gdispDrawString(170+30, 0+20-5, str, font1, White);

    sprintf(str, "%d",score);
    gdispDrawString(170+80, 0+30, str, font1, White);

    sprintf(str, "Level");
    // Print string
    gdispDrawString(170+30, 0+70-5, str, font1, White);

	sprintf(str, "%d",level);
    gdispDrawString(170+80, 0+80, str, font1, White);

    sprintf(str, "Lines");
    // Print string
    gdispDrawString(170+30, 0+120-5, str, font1, White);

    sprintf(str, "%d",lines);
    gdispDrawString(170+80, 0+130, str, font1, White);

    sprintf(str, "Next");
    // Print string
    gdispDrawString(170+30, 0+170-5, str, font1, White);

    DrawNextShape(nextShape,nextRotation);

    sprintf(str, "Receiving");
    gdispDrawString(140, 0+20-5, str, font1, White);
    sprintf(str, "%d",receiving);
    gdispDrawString(140, 0+30, str, font1, White);

    sprintf(str, "Sending");
    gdispDrawString(140, 0+70-5, str, font1, White);
	sprintf(str, "%d",sending);
    gdispDrawString(140, 0+80, str, font1, White);


}

void DrawMainMenu()
{
	char str[100]; // Init buffer for message
	gdispClear(Black);

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");

	sprintf(str, "TETRIS");
	gdispDrawString(30, 5, str, font1, White);


	if(getState()==1)
	{
		sprintf(str, "->");
		gdispDrawString(20, 30, str, font1, White);
	}
	if(getState()==2)
	{
		sprintf(str, "->");
		gdispDrawString(20, 60, str, font1, White);
	}

	sprintf(str, "Player 1");
	gdispDrawString(30, 30, str, font1, White);

	sprintf(str, "Player 2");
	gdispDrawString(30, 60, str, font1, White);

	sprintf(str, "Select an option and then press button B");
	gdispDrawString(30, 120, str, font1, White);

}


void DrawGameOver(const joystickselection_t * joystickselection  )
{
	char str[100]; // Init buffer for message
	gdispClear(Black);

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");

	sprintf(str, "GAME OVER");
	gdispDrawString(140, 5, str, font1, White);


	if(*joystickselection==JoyStickUp)
	{
		sprintf(str, "->");
		gdispDrawString(20, 30, str, font1, White);
	}
	if(*joystickselection==JoyStickDown)
	{
		sprintf(str, "->");
		gdispDrawString(20, 60, str, font1, White);
	}

	sprintf(str, "Restart");
	gdispDrawString(30, 30, str, font1, White);

	sprintf(str, "Main menu");
	gdispDrawString(30, 60, str, font1, White);

	sprintf(str, "Select an option and then press button B");
	gdispDrawString(30, 120, str, font1, White);

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
		return Orange;

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
	// Checks collision with shapes already stored in the board or the board limits
	// This is just to check the 5x5 blocks of a shape with the appropriate area in the board

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

			// Check if the shape touching a shape already stored in the board
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
			if (blockType == 0) break;
				i++;
		}

		if (i == BOARD_WIDTH_IN_BLOCKS){
			DeleteLine(j);
			++numOfLinesFilled;
		}
	}
	return numOfLinesFilled;
}

bool_t isGameOver()
{
	char blockType = 0;
	int i = 0;
	if(BoardMatrix[4][0]!=0 || BoardMatrix[5][0]!=0 || BoardMatrix[6][0]!=0)
		return true;
	return false;
}
