/*
 * This file contains declarations of public drawing functions, and configuration settings for drawing and displaying
 * meaningful objects on the screen.
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 *
 * */

#ifndef DRAW_H
#define DRAW_H

#include "includes.h"
#include "Shape.h"
#include "SystemState.h"

/*---------------------------------------------PLEASE DO NOT EDIT!!!----------------------------------------------------------*/
#define SCREEN_HEIGHT											240	//The total number of pixels that make up the heigt of the board
#define BOARD_WIDTH_IN_BLOCKS									10	//The number of blocks that make up the width of the board


#define BOARD_WIDTH (BOARD_WIDTH_IN_BLOCKS * BLOCK_SIZE)			//The number of pixels that determine the width of the board. It is automatically generated based on the block size
#define BOARD_HEIGHT_IN_BLOCKS ((SCREEN_HEIGHT/BLOCK_SIZE)-1)		//The number of blocks that determine the height of the board. This is automatically evaluated based on the block size.
#define BOARD_OFFSET_Y_AXIS 									7	//The offset of the board from the Y axis

#define SHAPE_MATRIX_DIMENSION									5	//The size of the matrix that is used to describe a shape in one of its 4 possible orientations
#define BLOCK_SIZE 												12	//This is the size of the block in pixels. It can be changed to 11
#define BOARD_OFFSET_X_AXIS										0 	//The offset of the board on the X axis. This is used to align the display with the board Grid which is set to 10 by 20.
#define BOARDER_THICKNESS 										5	//This is the number of pixels used for the border thickness of the board
/*-----------------------------------------------------------------------------------------------------------------------------*/
void DrawPauseMenu();
void DrawMainMenu(const uint8_t * gSelectionArrowPosition, const int * gHighestScore );
void DrawShapeWithHandle(shape_t * ptrShape);
void DrawGameFrame(shape_t * ptrShape, int lines, int score, int level, int g2playerGameNoOfRoundsWon,
		int g2playerGameNoOfRounds, int gPlayer2NumOfLinesCompleted, systemState_t state);
void DrawGameOver(const uint8_t * gSelectionArrowPosition, int gWhoWon);

void InitializeBoardMatrix();
boolean_t IsMoveMentPossible (shape_t * ptrShape);
boolean_t isGameOver();
void StoreShape (shape_t * ptrShape);
int DeletePossibleLines();

void AddLine(int NumOfLines, shape_t * ptrShape);
void CheckAbove(boolean_t * ShapeAbove, int row, int column, shape_t * ptrShape, int NumOfLines, int *gapBtwShapeAndTopBlock);



#endif
