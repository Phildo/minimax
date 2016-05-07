#include "stdio.h"

#define BOARD_S 4

typedef struct
{
  int pos[BOARD_S*BOARD_S];
} board;
int i(int x,int y)
{
  return y*BOARD_S+x;
}

void initBoard(board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      b->pos[i(x,y)] = (y%2+x)%2;
    }
  }
}

void printBoard(board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      printf("%i",b->pos[i(x,y)]);
    }
    printf("\n");
  }
}

int main()
{
  board b;
  initBoard(&b);
  printBoard(&b);
}

