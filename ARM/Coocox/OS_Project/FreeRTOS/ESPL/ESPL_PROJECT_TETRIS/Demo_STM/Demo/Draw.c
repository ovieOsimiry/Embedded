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
#include "SystemState.h"


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

/*-------------------------------------------------------------------------------------------------------
 * @desc This function clears the board Matrix by initializing all elements to 0
 * -----------------------------------------------------------------------------------------------------*/
void InitializeBoardMatrix()
{
	for (int i = 0; i < BOARD_WIDTH_IN_BLOCKS; i++)
		for (int j = 0; j < BOARD_HEIGHT_IN_BLOCKS; j++)
			BoardMatrix[i][j] = 0;
}



void DrawShapeWithHandle(shape_t * ptrShape)
{
	color_t mColor;// Color of the block

	// Traverse the shape matrix and draw the relevant blocks
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and select the right color
			switch(ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,j,i))
			{
				case 1: mColor = Red; break;
				case 2: mColor = Yellow; break;
				case 3: mColor = Green; break;
				case 4: mColor = Orange; break;
			}
			if(ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j, i) != 0)
			DrawCustomBlock(((ptrShape->x+i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS + BOARDER_THICKNESS,((ptrShape->y+j)*BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,mColor);
		}
	}
}

/* -------------------------------------------
 *@desc Draws a custom block on the screen according to the
 *@desc parameters given
 *@param x,y - coordinates of the block
 *@param cx, cy - size of the block
 *@param color - color of the block
 *@return void function
 * ------------------------------------------- */
void DrawCustomBlock(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color)
{
	gdispFillArea(x,y,cx,cy,Black);//border of block
	gdispFillArea(x+2,y+2,cx-2,cy-2,color);//block
}

/* -------------------------------------------
 *@desc Draws the shape to be playable into the next box
 *@param ptrShape - next shape
 *@return void function
 * ------------------------------------------- */
void DrawNextShape(shape_t * ptrShape)
{
	color_t mColor;				// Color of the block
	for (int i = 0; i < SHAPE_MATRIX_DIMENSION; i++)
	{
		for (int j = 0; j < SHAPE_MATRIX_DIMENSION; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (ptrShape->GetAPeiceFromShape(ptrShape->pArr, ptrShape->shapeOrientation, j,i ))
			{
				case 1: mColor = Red; break;
				case 2: mColor = Yellow; break;
				case 3: mColor = Green; break;
				case 4: mColor = Orange; break;
			}

			if (ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,j,i)!= 0)
			DrawCustomBlock(i*BLOCK_SIZE+170+60,j*BLOCK_SIZE+0+168,BLOCK_SIZE,BLOCK_SIZE,mColor);
		}
	}
}

///NOT FINISHED
/* -------------------------------------------
 *@desc Draws the game screen depending on the state of the game (1 or 2 player mode)
 *@param ptrShape - next Shape
 *@param lines - lines made
 *@param score - actual score
 *@param level - current level
 *@param sth
 *@param sth
 *@param state - state of the game ( 1 or 2 player mode)
 *@return void function
 * ------------------------------------------- */
void DrawGameFrame(shape_t * ptrShape, int lines, int score, int level, int g2playerGameNoOfRoundsWon,
		int g2playerGameNoOfRounds, int gPlayer2NumOfLinesCompleted, systemState_t state)//(int nextShape,int nextRotation,int lines, int score, int level, int receiving, int sending)
{
	char str[100]; // Init buffer for message
	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("UI1");
	char blockType = 0;
	int mX1 = 0;
	int mY = 0;

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
				DrawCustomBlock(((i)*BLOCK_SIZE)+BOARD_OFFSET_X_AXIS+BOARDER_THICKNESS,(j * BLOCK_SIZE)+BOARD_OFFSET_Y_AXIS,BLOCK_SIZE,BLOCK_SIZE,GetColor(blockType));
		}
	}

	int adjustment = 0;
	if(state == stateGame1Player)
	{
	    gdispFillArea(170+30-5+adjustment, 0+20-10 , 100+10,30+10, Blue);
	    gdispFillArea(170+30-5+adjustment, 0+70-10 , 100+10,30+10, Blue);
	    gdispFillArea(170+30-5+adjustment, 0+120-10 , 100+10,30+10, Blue);
	    gdispFillArea(170+30-5+adjustment, 0+170-10 , 100+10,60+10, Blue);

	    gdispFillArea(170+30+adjustment, 0+20-5 , 100,30, Black);
	    gdispFillArea(170+30+adjustment, 0+70-5 , 100,30, Black);
	    gdispFillArea(170+30+adjustment, 0+120-5 , 100,30, Black);
	    gdispFillArea(170+30+adjustment, 0+170-5 , 100,60, Black);

	    sprintf(str, "Score");
	    // Print string
	    gdispDrawString(170+30+adjustment, 0+20-5, str, font1, White);

	    sprintf(str, "%d",score);
	    gdispDrawString(170+80+adjustment, 0+30, str, font1, White);

	    sprintf(str, "Level");
	    // Print string
	    gdispDrawString(170+30+adjustment, 0+70-5, str, font1, White);

		sprintf(str, "%d",level);
	    gdispDrawString(170+80+adjustment, 0+80, str, font1, White);

	    sprintf(str, "Lines");
	    // Print string
	    gdispDrawString(170+30+adjustment, 0+120-5, str, font1, White);

	    sprintf(str, "%d",lines);
	    gdispDrawString(170+80+adjustment, 0+130, str, font1, White);

	    sprintf(str, "Next");
	    // Print string
	    gdispDrawString(170+30+adjustment, 0+170-5, str, font1, White);

	    DrawNextShape(ptrShape);
	}
	else { // 2 player mode
		adjustment = 10;
		gdispFillArea(170+30-5+adjustment, 0+20-10 , 100+10,30+10, Blue);
		gdispFillArea(170+30-5+adjustment, 0+70-10 , 100+10,30+10, Blue);
		gdispFillArea(170+30-5+adjustment, 0+120-10 , 100+10,30+10, Blue);
		gdispFillArea(170+30-5+adjustment, 0+170-10 , 100+10,60+10, Blue);

		gdispFillArea(170+30+adjustment, 0+20-5 , 100,30, Black);
		gdispFillArea(170+30+adjustment, 0+70-5 , 100,30, Black);
		gdispFillArea(170+30+adjustment, 0+120-5 , 100,30, Black);
		gdispFillArea(170+30+adjustment, 0+170-5 , 100,60, Black);

		sprintf(str, "Score");
		// Print string
		gdispDrawString(170+30+adjustment, 0+20-5, str, font1, White);

		sprintf(str, "%d",score);
		gdispDrawString(170+80+adjustment, 0+30, str, font1, White);

		sprintf(str, "Level");
		// Print string
		gdispDrawString(170+30+adjustment, 0+70-5, str, font1, White);

		sprintf(str, "%d",level);
		gdispDrawString(170+80+adjustment, 0+80, str, font1, White);

		sprintf(str, "Lines");
		// Print string
		gdispDrawString(170+30+adjustment, 0+120-5, str, font1, White);

		sprintf(str, "%d",lines);
		gdispDrawString(170+80+adjustment, 0+130, str, font1, White);

		sprintf(str, "Next");
		// Print string
		gdispDrawString(170+30+adjustment, 0+170-5, str, font1, White);

		DrawNextShape(ptrShape);

		sprintf(str, "Rounds won");
		gdispDrawString(133, 0+20-5, str, font1, White);
		sprintf(str, "%d",g2playerGameNoOfRoundsWon);
		gdispDrawString(133, 0+30, str, font1, White);

		sprintf(str, "Total");
		gdispDrawString(133, 0+70-5, str, font1, White);
		sprintf(str, "rounds");
		gdispDrawString(133, 0+85-5, str, font1, White);
		sprintf(str, "%d",g2playerGameNoOfRounds);
		gdispDrawString(133, 0+95, str, font1, White);

		sprintf(str, "Opponent");
		gdispDrawString(133, 0+135-5, str, font1, White);
		sprintf(str, "lines");
		gdispDrawString(133, 0+150-5, str, font1, White);
		sprintf(str, "%d",gPlayer2NumOfLinesCompleted);
		gdispDrawString(133, 0+160, str, font1, White);
	}
}

/* -------------------------------------------
 *@desc Draws the main menu screen
 *@param gSelectionArrowPosition - Possible selection according to the joystick
 *@param playerMode - 1 or 2 player mode
 *@param gHighestScore - highest score of the game
 *@return void function
 * ------------------------------------------- */
void DrawMainMenu(const uint8_t * gSelectionArrowPosition, const playermode_t * playerMode, const int * gHighestScore )//(const joystickselection_t * joystickselection, const playermode_t * playerMode )
{
	char str[100]; // Init buffer for message
	gdispClear(Black);

	font_t font1; // Load font for ugfx
	font1 = gdispOpenFont("DejaVuSans16");

	int j = 1;
	for(int i = 0; i <= 240-10; i = i+10)
	{
		color_t mColor;
		switch (j)
		{
			case 1: mColor = Red; break;
			case 2: mColor = Yellow; break;
			case 3: mColor = Green; break;
			case 4: mColor = Orange; break;
			case 5: mColor = Blue; break;
		}

		gdispFillArea(0, 0+i, 10,10, Black);
		gdispFillArea(2, 2+i, 8,8, mColor);
		j++;
		if(j>5)
			j = 1;
	}
	j=2;
	for(int i = 10; i <= 320-10; i = i+10)
	{
		color_t mColor;
		switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
		{
			case 1: mColor = Red; break;	// For each block of the piece except the pivot
			case 2: mColor = Yellow; break;	//
			case 3: mColor = Green; break;	//
			case 4: mColor = Orange; break;	//
			case 5: mColor = Blue; break;
		}
		gdispFillArea(0+i, 0, 10,10, Black);
		gdispFillArea(2+i, 2, 8,8, mColor);
		j++;
		if(j>5)
			j = 1;
	}
	j=3;
	for(int i = 10; i <= 240-10; i = i+10)
	{
			color_t mColor;
			switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
			{
				case 1: mColor = Red; break;	// For each block of the piece except the pivot
				case 2: mColor = Yellow; break;	//
				case 3: mColor = Green; break;	//
				case 4: mColor = Orange; break;	//
				case 5: mColor = Blue; break;
			}
			gdispFillArea(319-10, 0+i, 10,10, Black);
			gdispFillArea(319-10+2, 2+i, 8,8, mColor);
			j++;
			if(j>5)
				j = 1;
	}
	j=5;
	for(int i = 10; i <= 320-10; i = i+10)
	{
		color_t mColor;
		switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
		{
			case 1: mColor = Red; break;	// For each block of the piece except the pivot
			case 2: mColor = Yellow; break;	//
			case 3: mColor = Green; break;	//
			case 4: mColor = Orange; break;	//
			case 5: mColor = Blue; break;
		}
		gdispFillArea(0+i, 239-10, 10,10, Black);
		gdispFillArea(2+i, 239-10+2, 8,8, mColor);
		j++;
		if(j>5)
			j = 1;
	}

	// Draw Title

	color_t titleColor = Lime;
	//T
	gdispFillArea(160-5-90, 20, 10,10, Black);
	gdispFillArea(160-5+2-90, 20+2, 8,8, titleColor);
	gdispFillArea(160-5-90, 20+10, 10,10, Black);
	gdispFillArea(160-5+2-90, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5-90, 20+20, 10,10, Black);
	gdispFillArea(160-5+2-90, 20+22, 8,8, titleColor);
	gdispFillArea(160-5-90, 20+30, 10,10, Black);
	gdispFillArea(160-5+2-90, 20+32, 8,8, titleColor);

	gdispFillArea(160-5-10-90, 20, 10,10, Black);
	gdispFillArea(160-5+2-10-90, 20+2, 8,8, titleColor);

	gdispFillArea(160-5+10-90, 20, 10,10, Black);
	gdispFillArea(160-5+2+10-90, 20+2, 8,8, titleColor);

	//E
	gdispFillArea(160-5-60, 20, 10,10, Black);
	gdispFillArea(160-5+2-60, 20+2, 8,8, titleColor);
	gdispFillArea(160-5-60, 20+10, 10,10, Black);
	gdispFillArea(160-5+2-60, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5-60, 20+20, 10,10, Black);
	gdispFillArea(160-5+2-60, 20+22, 8,8, titleColor);
	gdispFillArea(160-5-60, 20+30, 10,10, Black);
	gdispFillArea(160-5+2-60, 20+32, 8,8, titleColor);

	gdispFillArea(160-5-50, 20, 10,10, Black);
	gdispFillArea(160-5+2-50, 20+2, 8,8, titleColor);
	gdispFillArea(160-5-50+10, 20, 10,10, Black);
	gdispFillArea(160-5+2-50+10, 20+2, 8,8, titleColor);

	gdispFillArea(160-5-50, 20+15, 10,10, Black);
	gdispFillArea(160-5+2-50, 20+17, 8,8, titleColor);

	gdispFillArea(160-5-50, 20+30, 10,10, Black);
	gdispFillArea(160-5+2-50, 20+32, 8,8, titleColor);
	gdispFillArea(160-5-50+10, 20+30, 10,10, Black);
	gdispFillArea(160-5+2-50+10, 20+32, 8,8, titleColor);

	//T
	gdispFillArea(160-5-10, 20, 10,10, Black);
	gdispFillArea(160-5+2-10, 20+2, 8,8, titleColor);
	gdispFillArea(160-5-10, 20+10, 10,10, Black);
	gdispFillArea(160-5+2-10, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5-10, 20+20, 10,10, Black);
	gdispFillArea(160-5+2-10, 20+22, 8,8, titleColor);
	gdispFillArea(160-5-10, 20+30, 10,10, Black);
	gdispFillArea(160-5+2-10, 20+32, 8,8, titleColor);

	gdispFillArea(160-5-20, 20, 10,10, Black);
	gdispFillArea(160-5+2-20, 20+2, 8,8, titleColor);

	gdispFillArea(160-5, 20, 10,10, Black);
	gdispFillArea(160-5+2, 20+2, 8,8, titleColor);

	//R
	gdispFillArea(160-5+20, 20, 10,10, Black);
	gdispFillArea(160-5+2+20, 20+2, 8,8, titleColor);
	gdispFillArea(160-5+20, 20+10, 10,10, Black);
	gdispFillArea(160-5+2+20, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5+20, 20+20, 10,10, Black);
	gdispFillArea(160-5+2+20, 20+22, 8,8, titleColor);
	gdispFillArea(160-5+20, 20+30, 10,10, Black);
	gdispFillArea(160-5+2+20, 20+32, 8,8, titleColor);

	gdispFillArea(160-5+30, 20, 10,10, Black);
	gdispFillArea(160-5+2+30, 20+2, 8,8, titleColor);
	gdispFillArea(160-5+30, 20+15, 10,10, Black);
	gdispFillArea(160-5+2+30, 20+17, 8,8, titleColor);

	gdispFillArea(160-5+30+10, 20+7, 10,10, Black);
	gdispFillArea(160-5+2+30+10, 20+2+7, 8,8, titleColor);

	gdispFillArea(160-5+30+10, 20+15+10, 10,10, Black);
	gdispFillArea(160-5+2+30+10, 20+17+10, 8,8, titleColor);

	// I
	gdispFillArea(160-5+60, 20, 10,10, Black);
	gdispFillArea(160-5+2+60, 20+2, 8,8, titleColor);
	gdispFillArea(160-5+60, 20+10, 10,10, Black);
	gdispFillArea(160-5+2+60, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5+60, 20+20, 10,10, Black);
	gdispFillArea(160-5+2+60, 20+22, 8,8, titleColor);
	gdispFillArea(160-5+60, 20+30, 10,10, Black);
	gdispFillArea(160-5+2+60, 20+32, 8,8, titleColor);

	//S
	gdispFillArea(160-5+90, 20, 10,10, Black);
	gdispFillArea(160-5+2+90, 20+2, 8,8, titleColor);
	gdispFillArea(160-5+90, 20+10, 10,10, Black);
	gdispFillArea(160-5+2+90, 20+2+10, 8,8, titleColor);
	gdispFillArea(160-5+90, 20+20, 10,10, Black);
	gdispFillArea(160-5+2+90, 20+22, 8,8, titleColor);
	gdispFillArea(160-5+90, 20+30, 10,10, Black);
	gdispFillArea(160-5+2+90, 20+32, 8,8, titleColor);

	gdispFillArea(160-5+100, 20, 10,10, Black);
	gdispFillArea(160-5+2+100, 20+2, 8,8, titleColor);
	gdispFillArea(160-5+80, 20+30, 10,10, Black);
	gdispFillArea(160-5+2+80, 20+32, 8,8, titleColor);


	if(*gSelectionArrowPosition==0)////if(*joystickselection==JoyStickUp)
	{
		gdispFillArea(115, 103, 10,10, Black);
		gdispFillArea(115+2, 103, 8,8, titleColor);
	}
	if(*gSelectionArrowPosition==1 && *playerMode==twoPlayerMode)//(*joystickselection==JoyStickDown && *playerMode==twoPlayerMode)//if(getState()==2)
	{
		gdispFillArea(110, 143, 10,10, Black);
		gdispFillArea(110+2, 143, 8,8, titleColor);
	}

	sprintf(str, "1 Player");
	gdispDrawString(130, 100, str, font1, White);

	sprintf(str, "2 Players");
	gdispDrawString(125, 140, str, font1, White);

	if(*playerMode==twoPlayerMode)
	{
		sprintf(str, "2 Players");
		gdispDrawString(125, 140, str, font1, White);
	}

	gdispCloseFont(font1);
	font1 = gdispOpenFont("UI1");

	sprintf(str, "Press B to select an option");
	gdispDrawString(30, 200, str, font1, White);

	sprintf(str, "Highest score:");
	gdispDrawString(30, 215, str, font1, White);

    sprintf(str, "%d", *gHighestScore);
    gdispDrawString(120, 215, str, font1, White);

}

/* -------------------------------------------
 *@desc Draws the game over screen
 *@param gSelectionArrowPosition - Possible selection according to the joystick
 *@param gWhoWon - displays who won the game ( in case of two player mode)
 *@param gWhoWon - 0 = 1 player mode; 1 = You won (2players); 2= your opponent won (2players)
 *@return void function
 * ------------------------------------------- */

void DrawGameOver(const uint8_t * gSelectionArrowPosition, int gWhoWon)//(const joystickselection_t * joystickselection  )
{
	char str[100]; // Init buffer for message
	gdispClear(Black);

	int j = 1;
			for(int i = 0; i <= 240-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}

				gdispFillArea(0, 0+i, 10,10, Black);
				gdispFillArea(2, 2+i, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}
			j=2;
			for(int i = 10; i <= 320-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}
				gdispFillArea(0+i, 0, 10,10, Black);
				gdispFillArea(2+i, 2, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}
			j=3;
			for(int i = 10; i <= 240-10; i = i+10)
			{
					color_t mColor;
					switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
					{
						case 1: mColor = Red; break;	// For each block of the piece except the pivot
						case 2: mColor = Yellow; break;	//
						case 3: mColor = Green; break;	//
						case 4: mColor = Orange; break;	//
						case 5: mColor = Blue; break;
					}
					gdispFillArea(319-10, 0+i, 10,10, Black);
					gdispFillArea(319-10+2, 2+i, 8,8, mColor);
					j++;
					if(j>5)
						j = 1;
			}
			j=5;
			for(int i = 10; i <= 320-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}
				gdispFillArea(0+i, 239-10, 10,10, Black);
				gdispFillArea(2+i, 239-10+2, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}


		font_t font1; // Load font for ugfx
		font1 = gdispOpenFont("DejaVuSans32");

		sprintf(str, "GAME OVER");
		gdispDrawString(70, 30, str, font1, Lime);



	if(*gSelectionArrowPosition==0)//(*joystickselection==JoyStickUp)
	{
		gdispFillArea(115, 103, 10,10, Black);
		gdispFillArea(115+2, 103, 8,8, Lime);
	}
	if(*gSelectionArrowPosition==1)//(*joystickselection==JoyStickDown)
	{
		gdispFillArea(105, 143, 10,10, Black);
		gdispFillArea(105+2, 143, 8,8, Lime);
	}

	gdispCloseFont(font1);
	font1 = gdispOpenFont("DejaVuSans16");

	sprintf(str, "Restart");
	gdispDrawString(130, 100, str, font1, White);

	sprintf(str, "Main menu");
	gdispDrawString(120, 140, str, font1, White);

	if(gWhoWon == 1)
	{
		sprintf(str, "You won!");
		gdispDrawString(120, 70, str, font1, Lime);
	}
	if(gWhoWon == 2)
	{
		sprintf(str, "You lost!");
		gdispDrawString(120, 70, str, font1, Lime);
	}

	gdispCloseFont(font1);
	font1 = gdispOpenFont("UI1");

	sprintf(str, "Press B to select an option");
	gdispDrawString(30, 200, str, font1, White);
}

/* -------------------------------------------
 *@desc Draws the pause screen
 *@param gSelectionArrowPosition - Possible selection according to the joystick
 *@return void function
 * ------------------------------------------- */
void DrawPauseMenu(uint8_t * selectionArrowPosition){
	char str[100]; // Init buffer for message
	gdispClear(Black);

	int j = 1;
			for(int i = 0; i <= 240-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}

				gdispFillArea(0, 0+i, 10,10, Black);
				gdispFillArea(2, 2+i, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}
			j=2;
			for(int i = 10; i <= 320-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}
				gdispFillArea(0+i, 0, 10,10, Black);
				gdispFillArea(2+i, 2, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}
			j=3;
			for(int i = 10; i <= 240-10; i = i+10)
			{
					color_t mColor;
					switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
					{
						case 1: mColor = Red; break;	// For each block of the piece except the pivot
						case 2: mColor = Yellow; break;	//
						case 3: mColor = Green; break;	//
						case 4: mColor = Orange; break;	//
						case 5: mColor = Blue; break;
					}
					gdispFillArea(319-10, 0+i, 10,10, Black);
					gdispFillArea(319-10+2, 2+i, 8,8, mColor);
					j++;
					if(j>5)
						j = 1;
			}
			j=5;
			for(int i = 10; i <= 320-10; i = i+10)
			{
				color_t mColor;
				switch (j)//(GetAPeiceFromTheShape (type, orientation, j, i))
				{
					case 1: mColor = Red; break;	// For each block of the piece except the pivot
					case 2: mColor = Yellow; break;	//
					case 3: mColor = Green; break;	//
					case 4: mColor = Orange; break;	//
					case 5: mColor = Blue; break;
				}
				gdispFillArea(0+i, 239-10, 10,10, Black);
				gdispFillArea(2+i, 239-10+2, 8,8, mColor);
				j++;
				if(j>5)
					j = 1;
			}

		font_t font1; // Load font for ugfx
		font1 = gdispOpenFont("DejaVuSans32");

		sprintf(str, "PAUSE");
		gdispDrawString(110, 30, str, font1, Lime);

	//if(getState() == 4)
	if(*selectionArrowPosition==0)
	{
		gdispFillArea(115, 103, 10,10, Black);
		gdispFillArea(115+2, 103, 8,8, Lime);
	}
	else if(*selectionArrowPosition==1)//else if(getState() == 5)
	{
		gdispFillArea(95, 143, 10,10, Black);
		gdispFillArea(95+2, 143, 8,8, Lime);
	}
	else if(*selectionArrowPosition==2)//else if(getState() == 6)
	{
		gdispFillArea(108, 183, 10,10, Black);
		gdispFillArea(108+2, 183, 8,8, Lime);
	}

	gdispCloseFont(font1);
	font1 = gdispOpenFont("DejaVuSans16");

	sprintf(str, "Resume");
	gdispDrawString(130, 100, str, font1, White);

	sprintf(str, "Restart game");
	gdispDrawString(110, 140, str, font1, White);

	sprintf(str, "Exit game");
	gdispDrawString(123, 180, str, font1, White);

	gdispCloseFont(font1);
	font1 = gdispOpenFont("UI1");

	sprintf(str, "Press B to select an option");
	gdispDrawString(30, 210, str, font1, White);
}

/* -------------------------------------------
 *@desc Get color depending on the given number
 *@param color - number of the color
 *@return color type (default White)
 * ------------------------------------------- */
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

/* -------------------------------------------
 *@desc Checks the state of a piece of the matrix
 *@param pX, pY - Coordinate of the piece
 *@return char - state of the piece in the matrix
 * ------------------------------------------- */
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

/* -------------------------------------------
 *@desc Deletes a line of blocks in the matrix
 *@param pY - line coordinate to delete
 *@return void function
 * ------------------------------------------- */
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

/***********************************************************************************************************
 *
 * Checks, if there is a shape block just above the upper limit block of the column
 * If the shape is just above, it changes the value of ShapeAbove to true so the addLine function knows that
 * the shape has to be moved 1 position up.
 * It also stores the value of the gap between the lower block of the falling shape and the limit base block,
 * so when we move up the shape, we only move it the number of unremovable lines added minus the gap that
 * were between the lower block of the shape and the limit block we are checking.
 *
 ***********************************************************************************************************/
void CheckAbove(boolean_t * ShapeAbove, int row, int column, shape_t * ptrShape, int NumOfLines, int *gapBtwShapeAndTopBlock){
	int tempGap;

	for(int i1 = 0, i2 = ptrShape->y; i1<SHAPE_MATRIX_DIMENSION; i1++, i2++){ //Go over all the positions of the shape matrix
		for(int j1 = 0, j2 = ptrShape->x; j1<SHAPE_MATRIX_DIMENSION; j1++, j2++){

			int pieceOfShape = ptrShape->GetAPeiceFromShape(ptrShape->pArr,ptrShape->shapeOrientation,i1,j1);

			int offset=0;								//If we are checking the base block (value = 0), we also have to check
			if(BoardMatrix[row][column]==0) offset=-1;  //the current block, not only the above ones

			for(int w= 1 + offset; w<=NumOfLines + offset; w++){			//Checks the sensible positions that may cause overlapping
				if((pieceOfShape!=0) && (i2 == column - w) && (j2== row)){  //from the first above block (or 0) to NumOfLines (or NumOfLines - 1).
					*ShapeAbove = true;

					if(w==0) tempGap=0; //Measures the gap between the lower block of the shape and the block of the base we are checking.
					else tempGap=w-1;
					if(tempGap<*gapBtwShapeAndTopBlock) *gapBtwShapeAndTopBlock = tempGap;
				}
			}
		}
	}
}

/***********************************************************************************************************
 *
 * Checks, column by column, if there is a shape block just above the upper base block of the column
 * so it can cause an overlapping while introducing the unremovable lines.
 * If the shape is above and may cause an overlapping, then it is moved the number of unremovable lines positions up
 * minus the gap between the lower block of the shape and the upper block of the base. Then the unremovable line is introduced.
 *
 ***********************************************************************************************************/
void AddLine_idea(int NumOfLines, shape_t * ptrShape){
	boolean_t ShapeAbove = false; //True if there is a block of a shape over matrix block we are checking
	boolean_t ColumnChecked;
	int gapBtwShapeAndTopBlock = 20; //We initialize the gap with a high number (it will never reach this value)

	//Check all the rows of one column until it founds the first non-zero block or the matrix base (limit blocks)
	for(int j=0; j< BOARD_WIDTH_IN_BLOCKS; j++){
		ColumnChecked = false;
		for(int i=0; i<BOARD_HEIGHT_IN_BLOCKS; i++){
			if((ColumnChecked == false) && ((BoardMatrix[j][i]!=0) || ((BoardMatrix[j][i]==0) && (i==BOARD_HEIGHT_IN_BLOCKS-1)))){
				CheckAbove(&ShapeAbove, j, i, ptrShape, NumOfLines, &gapBtwShapeAndTopBlock);
				ColumnChecked=true;  //Only checks for the first non-zero block or the matrix base
			}
		}
	}
	if(ShapeAbove==true){ //If one of the limit non-zero blocks has a shape block just above it, we move the shape up
		//Move the shape up the number of unremovable lines added minus the gap between the lower block of the shape and the limit block.
		ptrShape->y = ptrShape->y - NumOfLines + gapBtwShapeAndTopBlock;
		ShapeAbove=false;
		gapBtwShapeAndTopBlock = 20; //We reset the gap with a high number
	}


	for(int v = 0; v<NumOfLines; v++){ //Move "NumOfLines" times all the lines up and add "NumOfLines" lines of unremovable blocks
		//=====================MOVE MATRIX LINES UP===========================
		for(int i=0; i<BOARD_HEIGHT_IN_BLOCKS - 1; i++){
			for(int j=0; j<BOARD_WIDTH_IN_BLOCKS; j++){
				BoardMatrix[j][i] = BoardMatrix[j][i+1];
			}
		}
		//====================================================================

		//====================INTRODUCE UNREMOVABLE LINES=====================
		for(int j=0; j<BOARD_WIDTH_IN_BLOCKS; j++){
			BoardMatrix[j][BOARD_HEIGHT_IN_BLOCKS-1] = 5;
		}
		//====================================================================
	}
}


/* -------------------------------------------
 *@desc Checks and deletes the lines that are already filled with blocks
 *@param void parameters
 *@return int - number of deleted lines
 * ------------------------------------------- */
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

/* -------------------------------------------
 *@desc Checks the middle top of the matrix to see if there's blocks.
 *@param void parameters
 *@return boolean - true if the middle top of the matris is full.
 * ------------------------------------------- */
boolean_t isGameOver()
{
	if(BoardMatrix[4][0]!=0 || BoardMatrix[5][0]!=0 || BoardMatrix[6][0]!=0) // top - middle of the board
		return true;
	return false;
}
