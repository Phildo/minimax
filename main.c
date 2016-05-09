#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 5
#define INIT_ROWS 1
#define MAX_DEPTH 9999
#define PRUNE 0

struct board;
typedef struct board
{
  unsigned char player;
  unsigned char pos[(BOARD_S*BOARD_S)/2];
  unsigned char has_move;
  struct board *best;
  char heuristic;
  char score;
} board;
int i(int x,int y)
{
  return (y*BOARD_S+x)/2;
}
int posValid(int x, int y)
{
  return
    (x+y)%2 != 0 &&
    x >= 0 && x < BOARD_S &&
    y >= 0 && y < BOARD_S;
}

void zeroBoard(board *b)
{
  b->best = 0;
  b->has_move = 0;
}
void initBoard(board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(!posValid(x,y)) continue;
      if(y < INIT_ROWS)
        b->pos[i(x,y)] = (y%2+x)%2;
      else if(y >= BOARD_S-INIT_ROWS)
        b->pos[i(x,y)] = ((y%2+x)%2)*2;
      else
        b->pos[i(x,y)] = 0;
    }
  }
}

void copyBoard(board *from, board *to)
{
  to->player = from->player;
  for(int i = 0; i < (BOARD_S*BOARD_S)/2; i++)
    to->pos[i] = from->pos[i];
}

void boardSwap(int x0, int y0, int x1, int y1, board *b)
{
  int tmp = b->pos[i(x0,y0)];
  b->pos[i(x0,y0)] = b->pos[i(x1,y1)];
  b->pos[i(x1,y1)] = tmp;
}

int cmpBoard(board *a, board *b)
{
  for(int i = 0; i < (BOARD_S*BOARD_S)/2; i++)
    if(a->pos[i] != b->pos[i]) return 0;
  return 1;
}

void printBoard(board *b)
{
  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(!posValid(x,y)) printf(".");
      else
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

int rateBoard(board *b) //+ for p1, - for p2
{
  int n_1 = 0;
  int n_2 = 0;
  for(int i = 0; i < (BOARD_S*BOARD_S)/2; i++)
  {
    if(b->pos[i] == 1) n_1++;
    if(b->pos[i] == 2) n_2++;
  }
  b->heuristic = n_1-n_2;
  return b->heuristic;
}

int plotMoves(board *b, int pivot, int depth)
{
  board test; zeroBoard(&test);
  int xdir = 0;
  int ydir = 0;
  int move_found = 0;
  int s;

  rateBoard(b);
  if(depth >= MAX_DEPTH) return b->heuristic;

  for(int y = 0; y < BOARD_S; y++)
  {
    for(int x = 0; x < BOARD_S; x++)
    {
      if(b->pos[i(x,y)] == b->player)
      {
        ydir = (b->player == 1 ? 1 : -1);
        move_found = 0;
        for(xdir = -1; xdir <= 1; xdir+=2)
        {
          if(posValid(x+xdir,y+ydir))
          {
            if(b->pos[i(x+xdir,y+ydir)] == 0) //move
            {
              copyBoard(b,&test);
              boardSwap(x,y,x+xdir,y+ydir,&test);
              test.player = b->player == 1 ? 2 : 1;
              move_found = 1;
            }
            else if( //jump
              b->pos[i(x+xdir,y+ydir)] != b->player &&
              posValid(x+(2*xdir),y+(2*ydir)) &&
              b->pos[i(x+(2*xdir),y+(2*ydir))] == 0
            )
            {
              copyBoard(b,&test);
              boardSwap(x,y,x+(2*xdir),y+(2*ydir),&test);
              test.pos[i(x+xdir,y+ydir)] = 0;
              test.player = b->player == 1 ? 2 : 1;
              move_found = 1;
            }

            if(move_found)
            {
              s = plotMoves(&test,b->heuristic,depth+1);

              if(
                !b->has_move ||
                (b->player == 1 && s > b->score) ||
                (b->player == 2 && s < b->score)
              )
              {
                if(!b->has_move)
                {
                  b->best = (board *)malloc(sizeof(board));
                  b->has_move = 1;
                }
                b->score = s;
                copyBoard(&test,b->best);
                #if PRUNE
                if(
                  (b->player == 1 && b->score > pivot) ||
                  (b->player == 2 && b->score < pivot)
                )
                {
                  printf("prune %d->%d @ depth %d\n",b->heuristic,b->score,depth);
                  return b->score;
                }
                #endif
              }
            }
          }
        }
      }
    }
  }

  if(!b->has_move)
  {
    b->score = b->heuristic;
    return b->score;
  }

  return b->score;
}

void cleanupBoard(board *b)
{
  if(b->has_move)
  {
    cleanupBoard(b->best);
    free(b->best);
  }
}

int main()
{
  board b;
  initBoard(&b);
  zeroBoard(&b);
  b.player = 1;
  plotMoves(&b,0,0);

  board *t = &b;

  while(t->has_move)
  {
    printBoard(t);
    printf("\n");
    fflush(0);
    t = t->best;
    if(!t->has_move) plotMoves(t,t->heuristic,0);
  }
  printBoard(t);

  cleanupBoard(&b);
}

