#include "stdio.h"
#include "stdlib.h"

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
      if(y == 0)
        b->pos[i(x,y)] = (y%2+x)%2;
      else if(y == BOARD_S-1)
        b->pos[i(x,y)] = ((y%2+x)%2)*2;
      else
        b->pos[i(x,y)] = 0;
    }
  }
}

void copyBoardSwap(board *from, int x0, int y0, int x1, int y1, board *to)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      to->pos[i(x,y)] = from->pos[i(x,y)];
    }
  }
  int tmp = to->pos[i(x0,y0)];
  to->pos[i(x0,y0)] = to->pos[i(x1,y1)];
  to->pos[i(x1,y1)] = tmp;
}

void printBoard(board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      switch(b->pos[i(x,y)])
      {
        case 0: printf("."); break;
        case 1: printf("x"); break;
        case 2: printf("o"); break;
      }
    }
    printf("\n");
  }
}

void possibleMoves(board *b, int player, board **moves, int *n_moves)
{
  board *boards = (board *)malloc(sizeof(board)*100);
  int boards_i = 0;
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(b->pos[i(x,y)] == player)
      {
        if(player == 1)
        {
          if(y+1 < BOARD_S)
          {
            if(x+1 < BOARD_S && b->pos[i(x+1,y+1)] == 0) { copyBoardSwap(b,x,y,x+1,y+1,&boards[boards_i]); boards_i++; }
            if(x-1 >= 0      && b->pos[i(x-1,y+1)] == 0) { copyBoardSwap(b,x,y,x-1,y+1,&boards[boards_i]); boards_i++; }
          }
        }
        if(player == 2)
        {
          if(y-1 >= 0)
          {
            if(x+1 < BOARD_S && b->pos[i(x+1,y-1)] == 0) { copyBoardSwap(b,x,y,x+1,y-1,&boards[boards_i]); boards_i++; }
            if(x-1 >= 0      && b->pos[i(x-1,y-1)] == 0) { copyBoardSwap(b,x,y,x-1,y-1,&boards[boards_i]); boards_i++; }
          }
        }
      }
    }
  }

  board *trimmed_boards = (board *)malloc(sizeof(board)*boards_i);
  for(int i = 0; i < boards_i; i++)
    trimmed_boards[i] = boards[i];
  free(boards);

  *moves = trimmed_boards;
  *n_moves = boards_i;
}

int main()
{
  board b;
  initBoard(&b);
  printBoard(&b);

  board *moves;
  int n_moves;
  possibleMoves(&b, 1, &moves, &n_moves);

  for(int i = 0; i < n_moves; i++)
  {
    printf("Move %d:\n",i);
    printBoard(&moves[i]);
  }
}

