#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 10
#define INIT_ROWS 2
#define MAX_POSS_MOVES_FOR_BOARD 20
#define MAX_DEPTH 5

struct board;
typedef struct board
{
  unsigned char player;
  unsigned char pos[(BOARD_S*BOARD_S)/2];
  struct board *moves;
  unsigned char moves_known;
  unsigned char n_moves;
  unsigned char best_i;
  char score;
  char heuristic;
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
  b->moves = 0;
  b->moves_known = 0;
  b->n_moves = 0;
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
  if(boards == 0) printf("nomem!\n");
  for(int i = 0; i < MAX_POSS_MOVES_FOR_BOARD; i++)
    zeroBoard(&boards[i]);

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

  if(boards_i >= MAX_POSS_MOVES_FOR_BOARD)
    printf("badmax! %d\n",boards_i);

  if(boards_i > 0)
  {
    b->moves = (board *)malloc(sizeof(board)*boards_i);
    if(b->moves == 0) printf("nomem!\n");

    for(int i = 0; i < boards_i; i++)
      b->moves[i] = boards[i];
  }
  free(boards);

  b->n_moves = boards_i;
  b->moves_known = 1;
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

int plotMoves(board *b, int depth)
{
  if(!b->moves_known && depth < MAX_DEPTH) possibleMoves(b);

  if(!b->moves_known && depth >= MAX_DEPTH)
  {
    return rateBoard(b);
  }
  if(b->moves_known && b->n_moves == 0)
  {
    b->score = rateBoard(b);
    return b->score;
  }

  int best_i = 0;
  int best_s = plotMoves(&b->moves[0],depth+1);
  int s;
  for(int i = 1; i < b->n_moves; i++)
  {
    s = plotMoves(&b->moves[i],depth+1);

    if(
      (b->player == 1 && s > best_s) ||
      (b->player == 2 && s < best_s)
    )
    {
      best_s = s;
      best_i = i;
    }
  }

  b->best_i = best_i;
  b->score = best_s;
  return best_s;
}

void cleanupBoard(board *b)
{
  if(b->moves_known && b->n_moves)
  {
    for(int i = 0; i < b->n_moves; i++)
      cleanupBoard(&b->moves[i]);
    free(b->moves);
  }
}

int main()
{
  board b;
  initBoard(&b);
  zeroBoard(&b);
  b.player = 1;
  plotMoves(&b,0);

  board *t = &b;

  while(t->n_moves)
  {
    printBoard(t);
    printf("\n");
    fflush(0);
    t = &t->moves[t->best_i];
    plotMoves(t,0);
  }
  printBoard(t);

  cleanupBoard(&b);
}

