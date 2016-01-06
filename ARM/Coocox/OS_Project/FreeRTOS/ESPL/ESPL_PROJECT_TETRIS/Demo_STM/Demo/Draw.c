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
#include "Election.h"


void DrawNextShape(shape_t * ptrShape);
int GetYCoordForButtomOfShape(shape_t * ptrShape);
void GetHeighestPointOnBoard(coord_t * xRightCoordinate, coord_t * xLeftCoordinate, coord_t * yCordinate);
void GetXCoordsForBothSidesOfShape(shape_t * ptrShape, coord_t * xRight, coord_t * xLeft);
boolean_t CheckForCollision(coord_t shapeXCoordRight, coord_t shapeXCoordLeft, coord_t rightXCoordOfHeighestPointOnTheBoard, coord_t leftXCoordOfHeighestPointOnTheBoard);

void DrawCustomBlock(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);

int CheckForUnRemovableLine();
char GetPeiceType (int pX, int pY);
void DeleteLine (int pY);
color_t GetColor(char color);

static int BoardMatrix [BOARD_WIDTH_IN_BLOCKS][BOARD_HEIGHT_IN_BLOCKS];

//int BoardMatrix [BOARD_HEIGHT_IN_BLOCKS][BOARD_WIDTH_IN_BLOCKS];

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


void DrawShapeWithHandle(shape_t * ptrShape)
{
	color_t mColor;				// Color of the block

	// Travel the matrix of blocks of the piece and draw the blocks that are filled
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and select the right color
			//switch(ptrShape->GetAPeiceFromShape(ptrShape,j,i))
			switch(ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,j,i))
			{
				case 1: mColor = Red; break;	//
				case 2: mColor = Yellow; break;	//
				case 3: mColor = Green; break;	//
				case 4: mColor = Orange; break;	//
			}
			//if(ptrShape->GetAPeiceFromShape(ptrShape,j,i))
			 if(ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j, i) != 0)
			//gdispFillArea(((mPixelsX+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,((mPixelsY+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,mColor);
			DrawCustomBlock(((ptrShape->x+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS + BOARDER_THICKNESS,((ptrShape->y+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,mColor);
		}
	}
}

void DrawCustomBlock(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color)
{
	gdispFillArea(x,y,cx,cy,Black);//border of block
	gdispFillArea(x+2,y+2,cx-2,cy-2,color);//block
}

//Draws next block to be playable into the <next> box
void DrawNextShape(shape_t * ptrShape)//(int type, int orientation)
{
	color_t mColor;				// Color of the block
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and draw it with the correct color
			//switch (ptrShape->GetAPeiceFromShape(ptrShape,j,i))
			switch (ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j,i ))//(GetAPeiceFromTheShape (type, orientation, j, i))
			{
				case 1: mColor = Red; break;	// For each block of the piece except the pivot
				case 2: mColor = Yellow; break;	//
				case 3: mColor = Green; break;	//
				case 4: mColor = Orange; break;	//
			}

			//if (ptrShape->GetAPeiceFromShape(ptrShape,j,i)!= 0)
			if (ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,j,i)!= 0)
			//gdispFillArea(i*BLOCK_SIZE+170+60,j*BLOCK_SIZE+0+168,BLOCK_SIZE,BLOCK_SIZE,mColor);
			DrawCustomBlock(i*BLOCK_SIZE+170+60,j*BLOCK_SIZE+0+168,BLOCK_SIZE,BLOCK_SIZE,mColor);
		}
	}
}

void DrawGameFrame(shape_t * ptrShape, int lines, int score, int level, int receiving, int sending)//(int nextShape,int nextRotation,int lines, int score, int level, int receiving, int sending)
{
	char str[100]; // Init buffer for message
	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");
	char blockType = 0;
	int mX1 = 0;
	int mY = 0;

//	gdispFillArea(BOARD_OFFSET_X_AXIS,mY,BOARDER_THICKNESS,SCREEN_HEIGHT-BOARDER_THICKNESS,Blue);//left boarder
//	gdispFillArea(BOARD_OFFSET_X_AXIS + BOARD_WIDTH + BOARDER_THICKNESS,mY,BOARDER_THICKNESS,SCREEN_HEIGHT-BOARDER_THICKNESS,Blue);//right boarder
//	gdispFillArea(BOARD_OFFSET_X_AXIS,230,(BOARDER_THICKNESS+BOARD_WIDTH+BOARDER_THICKNESS),BOARDER_THICKNESS,Blue);//bottom boarder

	gdispFillArea(BOARD_OFFSET_X_AXIS,mY,BOARD_WIDTH+(2*BOARDER_THICKNESS),SCREEN_HEIGHT,Blue);//left boarder
	gdispFillArea(BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,mY,BOARD_WIDTH,SCREEN_HEIGHT-BOARDER_THICKNESS,Black);//left boarder

	// Drawing the blocks that are already stored in the board
	mX1 += 1;

	for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
		{
			// Check if the block is filled, if so, draw it
			blockType = GetPeiceType(i, j);
			if (blockType!=0)
				//gdispFillArea(((mX1+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS,((mY+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,GetColor(blockType));
				DrawCustomBlock(((i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,(j * BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,GetColor(blockType));

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

    //DrawNextShape(nextShape,nextRotation);
    DrawNextShape(ptrShape);

    sprintf(str, "Receiving");
    gdispDrawString(140, 0+20-5, str, font1, White);
    sprintf(str, "%d",receiving);
    gdispDrawString(140, 0+30, str, font1, White);

    sprintf(str, "Sending");
    gdispDrawString(140, 0+70-5, str, font1, White);
	sprintf(str, "%d",sending);
    gdispDrawString(140, 0+80, str, font1, White);
}

void DrawMainMenu(const joystickselection_t * joystickselection, const playermode_t * playerMode )
{
	char str[100]; // Init buffer for message
	gdispClear(Black);

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");

	sprintf(str, "TETRIS");
	gdispDrawString(30, 5, str, font1, White);


	if(*joystickselection==JoyStickUp)//if(getState()==1)
	{
		sprintf(str, "->");
		gdispDrawString(20, 30, str, font1, White);
	}
	if(*joystickselection==JoyStickDown && *playerMode==twoPlayerMode)//if(getState()==2)
	{
		sprintf(str, "->");
		gdispDrawString(20, 60, str, font1, White);
	}

	sprintf(str, "Player 1");
	gdispDrawString(30, 30, str, font1, White);

	if(*playerMode==twoPlayerMode)
	{
		sprintf(str, "Player 2");
		gdispDrawString(30, 60, str, font1, White);
	}

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

void DrawPauseMenu(){
	char str[100]; // Init buffer for message
	gdispClear(Black);

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans32*");

	sprintf(str, "PAUSE");
	gdispDrawString(140, 5, str, font1, White);

	if(getState() == 4){
		sprintf(str, "->");
		gdispDrawString(20, 30, str, font1, White);
	}
	else if(getState() == 5){
			sprintf(str, "->");
			gdispDrawString(20, 60, str, font1, White);
	}
	else if(getState() == 6){
			sprintf(str, "->");
			gdispDrawString(20, 90, str, font1, White);
	}

	sprintf(str, "Resume");
	gdispDrawString(30, 30, str, font1, White);

	sprintf(str, "Restart game");
	gdispDrawString(30, 60, str, font1, White);

	sprintf(str, "Exit game");
	gdispDrawString(30, 90, str, font1, White);

	sprintf(str, "Select an option and then press button B");
	gdispDrawString(30, 150, str, font1, White);
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

		case 5:
		return Gray;

		default:
			return White;
	}
}


char GetPeiceType(int pX, int pY)
{
	return BoardMatrix [pX][pY];
}

boolean_t IsMoveMentPossible (shape_t * ptrShape)
{
	// Checks collision with shapes already stored in the board or the board limits
	// This is just to check the 5x5 blocks of a shape with the appropriate area in the board

	boolean_t status = true;
	int blocktype = 0;
	for (int i1 = ptrShape->x, i2 = 0; i1 < ptrShape->x + SHAPE_MATRIX_DIMENSION; i1++, i2++)
	{
		for (int j1 = ptrShape->y, j2 = 0; j1 <ptrShape->y + SHAPE_MATRIX_DIMENSION; j1++, j2++)
		{
			// Check if the shape is outside the limits of the board
			if(	i1 < 0 	||	i1 > BOARD_WIDTH_IN_BLOCKS  - 1	|| 	j1 > BOARD_HEIGHT_IN_BLOCKS - 1)
			{
				//if(ptrShape->GetAPeiceFromShape(ptrShape,j2,i2)!= 0)//
				if(ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j2, i2) != 0)
					return false;
			}

			// Check if the shape touching a shape already stored in the board
			if (j1 >= 0)
			{
				status = GetPeiceType(i1, j1);
				//blockType = ptrShape->GetAPeiceFromShape(ptrShape,j2,i2);//
				blocktype = ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j2, i2);
				if ((blocktype != 0) && (status!=0))
					return false;
			}
		}
	}

	// No collision
	return true;
}


void StoreShape (shape_t * ptrShape)
{

	char blockType = 0;
	for (int i1 = ptrShape->x, i2 = 0; i1 < ptrShape->x + SHAPE_MATRIX_DIMENSION; i1++, i2++)
	{
		for (int j1 = ptrShape->y, j2 = 0; j1 < ptrShape->y + SHAPE_MATRIX_DIMENSION; j1++, j2++)
		{
			//blockType = ptrShape->GetAPeiceFromShape(ptrShape,j2,i2);
			blockType = ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,j2,i2);
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

int CheckForUnRemovableLine()
{
	for (int j = 0; j <= BOARD_HEIGHT_IN_BLOCKS; j++)
		{
			if(BoardMatrix[0][j]==5)
				//if we find an already existing un-removable line then we return the row number of the next matrix line above
				return j-1; //check later, to avoid negative value.
		}
	return (BOARD_HEIGHT_IN_BLOCKS-1);
}


void GetHeighestPointOnBoard(coord_t * xRightCoordinate, coord_t * xLeftCoordinate, coord_t * yCordinate)
{
	*yCordinate = 19;
	*xLeftCoordinate = 0;
	*xRightCoordinate = 0;
	for(int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
		{
			for(int i = 0; i<BOARD_WIDTH_IN_BLOCKS; i++)
			{
				if(BoardMatrix[i][j]!=0 && BoardMatrix[i][j]!=5)//if(BoardMatrix[i][j]!=0)
				{
				//if we find an already existing shape on the board we take the x and y coordinate of the shape.
				//we will use these values to compare with the x and y coordinate of the falling shape
					*xLeftCoordinate = i;// we have found the left side of the x coordinate for the shape
					*yCordinate = j-1; //check later, to avoid negative value.
					for(i = BOARD_WIDTH_IN_BLOCKS-1; i>=0; i--)
					{
						if(BoardMatrix[i][j]!=0)
							{
							 *xRightCoordinate = i;// we have found the right side of the x coordinate for the shape
							 return;
							}
					}
				}
			}
		}
}


int GetYCoordForButtomOfShape(shape_t * ptrShape)
{
	int aPeiceOfShape = 0;
	for(int x = 4; x>=0; x--)
	{
		for(int y = 0; y<=4; y++)
		{
			//aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape,x,y);
			aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,x,y);
			if(aPeiceOfShape!=0)
				return x;
		}

	}
	return 0;
}


void GetXCoordsForBothSidesOfShape(shape_t * ptrShape, coord_t * xRight, coord_t * xLeft)
{
	int aPeiceOfShape = 0;

	for(int x = 0; x<5; x++)
	{
		for(int y = 0; y<=4; y++)
		{
			//aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape,x,y);//
			aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,x,y);
			if(aPeiceOfShape!=0)
			{
				*xLeft = y;
				 break;
			}
		}

	}

	for(int x = 0; x<5; x++)
		{
			for(int y = 4; y>=0; y--)
			{
				//aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape,x,y);
				aPeiceOfShape = ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,x,y);
				if(aPeiceOfShape!=0)
				{
					*xRight = y;
					 break;
				}
			}

		}
return;
}

boolean_t CheckForCollision(coord_t shapeXCoordRight, coord_t shapeXCoordLeft, coord_t rightXCoordOfHeighestPointOnTheBoard, coord_t leftXCoordOfHeighestPointOnTheBoard)
	{
		for(int j=shapeXCoordLeft;j<=shapeXCoordRight;j++)
		{
			for(int i=leftXCoordOfHeighestPointOnTheBoard;i<=rightXCoordOfHeighestPointOnTheBoard;i++)
			{
				if(i==j)
					return true;
			}
		}
		return false;
	}

void AddLine(int NumOfLines, shape_t * ptrShape)
{
	int StartPos;
	int DistanceBtwShapeAndNewBaseOfBoard;//distance between the bottom of the shape and the new refernece base of the board
	int DistBtwShapeAndHeighestPointOnBoard;
	int yCoordForBtmOfShape;
	int baseOfShape;
	coord_t rightXCoordOfHeighestPointOnTheBoard;
	coord_t leftXCoordOfHeighestPointOnTheBoard;
	coord_t yCoordOfHeighestPointOnTheBoard;
	coord_t shapeXCoordRight;
	coord_t shapeXCoordLeft;

	GetHeighestPointOnBoard(&rightXCoordOfHeighestPointOnTheBoard, &leftXCoordOfHeighestPointOnTheBoard, &yCoordOfHeighestPointOnTheBoard);
	GetXCoordsForBothSidesOfShape(ptrShape,&shapeXCoordRight,&shapeXCoordLeft);

	shapeXCoordLeft = shapeXCoordLeft + ptrShape->x;
	shapeXCoordRight = (ptrShape->x + 4) - shapeXCoordRight;

	baseOfShape = GetYCoordForButtomOfShape(ptrShape);
	yCoordForBtmOfShape = ptrShape->y + baseOfShape;
	StartPos = CheckForUnRemovableLine();

	DistanceBtwShapeAndNewBaseOfBoard = StartPos - yCoordForBtmOfShape;
	DistBtwShapeAndHeighestPointOnBoard = yCoordOfHeighestPointOnTheBoard - yCoordForBtmOfShape;


	if(CheckForCollision(shapeXCoordRight,shapeXCoordLeft, rightXCoordOfHeighestPointOnTheBoard, leftXCoordOfHeighestPointOnTheBoard ))
	{
		if(DistBtwShapeAndHeighestPointOnBoard < NumOfLines)
		{
			ptrShape->y = ptrShape->y - (NumOfLines - DistBtwShapeAndHeighestPointOnBoard);
		}
	}

	else if((DistanceBtwShapeAndNewBaseOfBoard) < NumOfLines)
		{
			(ptrShape->y) = (ptrShape->y) - (NumOfLines - DistanceBtwShapeAndNewBaseOfBoard);
		}

	/*------Beginning from the bottom, move each line of block up by a number equal to the number of lines that will be added----*/

		for (int j = 0; j <= (StartPos-NumOfLines); j++)
			{
				for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
				{
					BoardMatrix[i][j] = BoardMatrix[i][j+NumOfLines];//#check here for negative value when ([j-NumOfLines])
				}
			}


	/*--------------------------------------------------------------------------------*/

	/*--------------------Add the number of Unremovable blocks specified in NumOfLines-----------*/
		for (int j = StartPos; j > (StartPos-NumOfLines); j--)
			{
					for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
					{
						BoardMatrix[i][j] = 5; // assign space as an un-removable peice.
					}
			}
	/*--------------------------------------------------------------------------------------------*/
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
			if (blockType == 0 || blockType == 5) break;//#
				i++;
		}

		if (i == BOARD_WIDTH_IN_BLOCKS){
			DeleteLine(j);
			++numOfLinesFilled;
		}
	}
	return numOfLinesFilled;
}

boolean_t isGameOver()
{
	if(BoardMatrix[4][0]!=0 || BoardMatrix[5][0]!=0 || BoardMatrix[6][0]!=0)
		return true;
	return false;
}
