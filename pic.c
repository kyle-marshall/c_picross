
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picross.h"

int main(int argc, char** argv){
  pic_board* board;
  int w, h;
  int quit = 0;
  int col;
  char a_buf[10];
  int row;
  char m;
  if(argc == 3){
    w = atoi(argv[1]);
    h = atoi(argv[2]);
    if(w == 0 || h == 0){
      printf("error: invalid dims\n");
    }
  }
  else{
    w = 5;
    h = 5;
  }
  board = rand_pic_board(w, h);
  print_pic_board(board);
  do{
    printf("Enter the coordinate (e.g. 'A 1'): "); 
    scanf("%s %d", &a_buf, &row);
    if(strcmp(a_buf,"Quit") == 0 ||
       strcmp(a_buf,"quit") == 0){
      quit = 1;
      break;
    }
    col = alpha_to_int_index(a_buf);
    row--;
    printf("row=%d, col=%d\n", row, col);
    m = *get_cell_mark(board, row, col);
    switch(m){
    case MARK_NONE:
      *get_cell_mark(board,row,col) = MARK_FILLED;
      break;
    case MARK_FILLED:
      *get_cell_mark(board,row,col) = MARK_CROSS;
      break;
    case MARK_CROSS:
      *get_cell_mark(board,row,col) = MARK_NONE;
      break;
    }
    
    print_pic_board(board);
  } while(!quit);
}
