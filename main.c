#include "stdio.h"
#include "stdlib.h"

#define BOARD_S 5
#define MAX_POSS_MOVES_FOR_BOARD 100

typedef struct
{
  int pos[BOARD_S*BOARD_S];
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

void appendBoards(board *b, int player, int x, int y, int xdir, int ydir, board *boards, int *boards_i)
{
  if(posValid(x+xdir,y+ydir))
  {
    if(b->pos[i(x+xdir,y+ydir)] == 0) //move
    {
      copyBoardSwap(b,x,y,x+xdir,y+ydir,&boards[*boards_i]);
      *boards_i = *boards_i+1;
    }
    else if( //jump
      b->pos[i(x+xdir,y+ydir)] != player &&
      posValid(x+(2*xdir),y+(2*ydir)) &&
      b->pos[i(x+(2*xdir),y+(2*ydir))] == 0
    )
    {
      copyBoardSwap(b,x,y,x+(2*xdir),y+(2*ydir),&boards[*boards_i]);
      boards[*boards_i].pos[i(x+xdir,y+ydir)] = 0;
      *boards_i = *boards_i+1;
    }
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
        appendBoards(b, player, x, y, -1, (player == 1 ? 1 : -1), boards, &boards_i);
        appendBoards(b, player, x, y,  1, (player == 1 ? 1 : -1), boards, &boards_i);
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

void bestMove(board *b, int player, board *best);
int rateBoard(board *b, int player) //+ for p1, - for p2
{
  board best;
  bestMove(b, player, &best);

  //simple heuristic
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
  int best_s = rateBoard(&moves[0], player == 1 ? 2 : 1);
  int s;
  for(int i = 1; i < n_moves; i++)
  {
    s = rateBoard(&moves[i], player == 1 ? 2 : 1);
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
  initBoard(&best);
  printf("\n");

  int turn = 0;
  while(turn == 0 || !cmpBoard(&b,&best))
  {
    printBoard(&best);
    copyBoard(&best,&b);
    bestMove(&b,turn%2 ? 2 : 1,&best);
    turn++;
    printf("\n");
  }
}

