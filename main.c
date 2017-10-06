#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 20
#define INIT_ROWS 4
#define MAX_DEPTH 4
#define PRUNE 1
#define DETERMINISTIC 0

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
  b->has_move = 0;
  b->best = 0;
  b->score = 0;
}
void initBoard(board *b)
{
  b->heuristic = 0;
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
  to->heuristic = from->heuristic;
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
  if(b->player == 1) printf("x:(%d,%d)\n",b->heuristic,b->score);
  if(b->player == 2) printf("o:(%d,%d)\n",b->heuristic,b->score);
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

void cleanupBoard(board *b)
{
  if(b->has_move)
  {
    cleanupBoard(b->best);
    free(b->best);
  }
}

int plotMoves(board *b, int bound_top, int bound_bottom, int depth)
{
  //if(b->player == 1) printf("plotting x: %d @ depth %d\n",bound_top,depth);
  //if(b->player == 2) printf("plotting o: %d @ depth %d\n",bound_bottom,depth);
  board test;
  zeroBoard(&test);
  int xdir = 0;
  int ydir = 0;
  int move_found = 0;
  int s;

  if(depth >= MAX_DEPTH) { b->score = b->heuristic; return b->score; }

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
              test.heuristic = b->heuristic; //no change
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
              test.heuristic += 3-(b->player*2); //lol 1 = +1, 2 = -1
              move_found = 1;
            }

            if(move_found)
            {
              s = plotMoves(&test,bound_top,bound_bottom,depth+1);

              if(
                !b->has_move ||
                (b->player == 1 && s > b->score) ||
                (b->player == 2 && s < b->score) ||
              #if !DETERMINISTIC
                (s == b->score && rand()%2) ||
              #endif
                0
              )
              {
                if(!b->has_move) b->best = (board *)malloc(sizeof(board));
                b->has_move = 1;
                b->score = test.score;

                b->best->player = test.player;
                copyBoard(&test,b->best);
                b->best->has_move  = test.has_move;
                b->best->best      = test.best;
                b->best->score     = test.score;
                zeroBoard(&test);

                #if PRUNE
                if(b->player == 1)
                {
                  if(b->score > bound_top)
                  {
                    //printf("prune %d->%d @ depth %d\n",b->score,bound_top,depth);
                    return b->score;
                  }
                  if(b->score > bound_bottom) bound_bottom = b->score;
                }
                if(b->player == 2)
                {
                  if(b->score < bound_bottom)
                  {
                    //printf("prune %d->%d @ depth %d\n",b->score,bound_bottom,depth);
                    return b->score;
                  }
                  if(b->score < bound_top) bound_top = b->score;
                }
                #endif
              }
              else
              {
                cleanupBoard(&test);
                zeroBoard(&test);
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

int main()
{
  board b;
  zeroBoard(&b);
  initBoard(&b);
  b.player = 1;
  board *t = &b;

  #if !DETERMINISTIC
  srand(0);//time(NULL));
  #endif

  plotMoves(t,999,-999,0);
  while(t->has_move)
  {
    printBoard(t);
    printf("\n");
    fflush(0);
    t = t->best;
    #if 1 || PRUNE
    cleanupBoard(t);
    t->has_move = 0;
    t->best = 0;
    plotMoves(t,999,-999,0);
    #endif
  }
  printBoard(t);

  cleanupBoard(&b);
}

