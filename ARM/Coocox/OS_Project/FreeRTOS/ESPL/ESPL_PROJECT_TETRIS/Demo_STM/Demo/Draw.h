#ifndef DRAW_H
#define DRAW_H

#include "includes.h"
#include "Shape.h"
#include "SystemState.h"

/*-------------------PLEASE DO NOT EDIT!!!----------------*/
#define BOARD_POSITION 50
#define SCREEN_HEIGHT 240
#define BOARD_WIDTH_IN_BLOCKS 10	// Board width in blocks


#define BOARD_WIDTH (BOARD_WIDTH_IN_BLOCKS * BLOCK_SIZE)// Board width in blocks//110
#define BOARD_HEIGHT_IN_BLOCKS ((SCREEN_HEIGHT/BLOCK_SIZE)-1)//19//20
#define BOARD_OFFSET_Y_AXIS 7//11

#define SHAPE_MATRIX_DIMENSION 5
#define BLOCK_SIZE 12//can be changed to 11
#define BOARD_OFFSET_X_AXIS 0 //This is used to align the display with the board Grid which is set to 10 by 20.
#define BOARDER_THICKNESS 5//11
/*---------------------------------------------------------------*/
void DrawPauseMenu();
void DrawMainMenu(const uint8_t * gSelectionArrowPosition, const playermode_t * playerMode, const int * gHighestScore );//(const joystickselection_t * joystickselection , const playermode_t * playerMode );
void DrawShapeWithHandle(shape_t * ptrShape);
void DrawGameFrame(shape_t * ptrShape, int lines, int score, int level, int g2playerGameNoOfRoundsWon,
		int g2playerGameNoOfRounds, int gPlayer2NumOfLinesCompleted, systemState_t state);
void DrawGameOver(const uint8_t * gSelectionArrowPosition, int gWhoWon);//(const joystickselection_t * joystickselection );

void AddLine(int NumOfLines, shape_t * ptrShape);
void InitializeBoardMatrix();
boolean_t IsMoveMentPossible (shape_t * ptrShape);
boolean_t isGameOver();
void StoreShape (shape_t * ptrShape);
int DeletePossibleLines();


#endif
