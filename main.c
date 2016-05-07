#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 4
#define MAX_POSS_MOVES_FOR_BOARD 100

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

void copyBoard(board *from, board *to)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      to->pos[i(x,y)] = from->pos[i(x,y)];
    }
  }
}

void copyBoardSwap(board *from, int x0, int y0, int x1, int y1, board *to)
{
  copyBoard(from,to);
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
  board *boards = (board *)malloc(sizeof(board)*MAX_POSS_MOVES_FOR_BOARD);
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

int rateBoard(board *b) //+ for p1, - for p2
{
  int n_1 = 0;
  int n_2 = 0;
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(b->pos[i(x,y)] == 1) n_1++;
      if(b->pos[i(x,y)] == 2) n_2++;
    }
  }
  return n_1-n_2;
}

void bestMove(board *b, int player, board *best)
{
  board *moves;
  int n_moves;
  possibleMoves(b, player, &moves, &n_moves);

  if(n_moves == 0) { copyBoard(b,best); return; }

  int best_i = 0;
  int best_s = rateBoard(&moves[0]);
  int s;
  for(int i = 1; i < n_moves; i++)
  {
    s = rateBoard(&moves[i]);
    if(
      (player == 1 && s > best_s) ||
      (player == 2 && s < best_s)
    )
    {
      best_s = s;
      best_i = i;
    }
  }

  copyBoard(&moves[best_i],best);
  free(moves);
}

int main()
{
  board b;
  board best;
  initBoard(&b);
  printBoard(&b);

  printf("\n");
  bestMove(&b,1,&best);
  printBoard(&best);
  copyBoard(&best,&b);
  bestMove(&b,2,&best);
  printBoard(&best);
}

