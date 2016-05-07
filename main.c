#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 5
#define MAX_POSS_MOVES_FOR_BOARD 5

struct board;
typedef struct board
{
  int player;
  int pos[BOARD_S*BOARD_S];
  struct board *moves;
  int n_moves;
  int best;
  int score;
} board;
int i(int x,int y)
{
  return y*BOARD_S+x;
}
int posValid(int x, int y)
{
  return
    x >= 0 && x < BOARD_S &&
    y >= 0 && y < BOARD_S;
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
  to->player = from->player;
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      to->pos[i(x,y)] = from->pos[i(x,y)];
    }
  }
}

void boardSwap(int x0, int y0, int x1, int y1, board *b)
{
  int tmp = b->pos[i(x0,y0)];
  b->pos[i(x0,y0)] = b->pos[i(x1,y1)];
  b->pos[i(x1,y1)] = tmp;
}

int cmpBoard(board *a, board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(a->pos[i(x,y)] != b->pos[i(x,y)]) return 0;
    }
  }
  return 1;
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

void appendBoards(board *b, int x, int y, int xdir, int ydir, board *boards, int *boards_i)
{
  if(posValid(x+xdir,y+ydir))
  {
    if(b->pos[i(x+xdir,y+ydir)] == 0) //move
    {
      copyBoard(b,&boards[*boards_i]);
      boardSwap(x,y,x+xdir,y+ydir,&boards[*boards_i]);
      boards[*boards_i].player = b->player == 1 ? 2 : 1;
      *boards_i = *boards_i+1;
    }
    else if( //jump
      b->pos[i(x+xdir,y+ydir)] != b->player &&
      posValid(x+(2*xdir),y+(2*ydir)) &&
      b->pos[i(x+(2*xdir),y+(2*ydir))] == 0
    )
    {
      copyBoard(b,&boards[*boards_i]);
      boardSwap(x,y,x+(2*xdir),y+(2*ydir),&boards[*boards_i]);
      boards[*boards_i].pos[i(x+xdir,y+ydir)] = 0;
      boards[*boards_i].player = b->player == 1 ? 2 : 1;
      *boards_i = *boards_i+1;
    }
  }
}

void possibleMoves(board *b)
{
  board *boards = (board *)malloc(sizeof(board)*MAX_POSS_MOVES_FOR_BOARD);
  int boards_i = 0;
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(b->pos[i(x,y)] == b->player)
      {
        appendBoards(b, x, y, -1, (b->player == 1 ? 1 : -1), boards, &boards_i);
        appendBoards(b, x, y,  1, (b->player == 1 ? 1 : -1), boards, &boards_i);
      }
    }
  }

  b->moves = (board *)malloc(sizeof(board)*boards_i);
  for(int i = 0; i < boards_i; i++)
    b->moves[i] = boards[i];
  free(boards);

  b->n_moves = boards_i;
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

int bestMove(board *b)
{
  possibleMoves(b);

  if(b->n_moves == 0)
  {
    b->score = rateBoard(b);
    return b->score;
  }

  int best_i = 0;
  int best_s = bestMove(&b->moves[0]);
  int s;
  for(int i = 1; i < b->n_moves; i++)
  {
    s = bestMove(&b->moves[i]);
    if(
      (b->player == 1 && s > best_s) ||
      (b->player == 2 && s < best_s)
    )
    {
      best_s = s;
      best_i = i;
    }
  }

  b->best = best_i;
  b->score = best_s;
  return best_s;
}

int main()
{
  board b;
  initBoard(&b);
  b.player = 1;
  bestMove(&b);

  board *t;
  t = &b;

  while(t->n_moves)
  {
    printBoard(t);
    printf("\n");
    t = &t->moves[t->best];
  }
  printBoard(t);
}

