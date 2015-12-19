#ifndef _DRAW_
#define _DRAW_

#include "includes.h"
#include "Shape.h"

#define BOARD_POSITION 50
#define BOARDER_THICKNESS 11
#define BOARD_WIDTH 110				// Board width in blocks
#define SCREEN_HEIGHT 240
#define BOARD_WIDTH_IN_BLOCKS 10	// Board width in blocks
#define BOARD_HEIGHT_IN_BLOCKS 20
#define BOARD_OFFSET_Y_AXIS 11		//This is used to allign the display with the board Grid which is set to 10 by 20.
#define BOARD_OFFSET_X_AXIS 0		//This is used to allign the display with the board Grid which is set to 10 by 20.
#define BLOCK_SIZE 11
#define SHAPE_MATRIX_DIMENSION 5


extern char verticalMove;
extern char rotation;
extern int BoardMatrix [BOARD_WIDTH_IN_BLOCKS][BOARD_HEIGHT_IN_BLOCKS];

void DrawMainMenu();
void DrawShapeWithHandle(shape_t * shape);
void DrawBoardMatrix(int nextShape,int nextRotation,int lines,int score,int level, int receiving, int sending);
void DrawNextShape(int type, int orientation);
void DrawGameOver(const joystickselection_t * joystickselection  );

void InitializeBoardMatrix();
char GetPeiceType (int pX, int pY);
boolean_t IsMoveMentPossible (int pX, int pY, int pShape, int pRotation);
bool_t isGameOver();
void StoreShape (int pX, int pY, int pPiece, int pRotation);
void DeleteLine (int pY);
int DeletePossibleLines();
color_t GetColor(char color);

#endif
