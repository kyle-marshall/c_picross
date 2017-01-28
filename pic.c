
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picross.h"


int a_validate(char* a){
  int i, len;
  char dif = 'a' - 'A';
  len = strlen(a);
  for(i=0;i<len;i++){
    if(a[i] >= 'a' && a[i] <= 'z'){
      a[i] = a[i] - dif;
    }
    else if(a[i] < 'A' || a[i] > 'Z'){
      return 0;
    }
  }
  return 1;
}

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
      printf("Error: Invalid dims.\n");
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

    if(!a_validate(a_buf)){
      printf("Invalid coordinate! (Enter 'quit' followed by something else to quit.)\n");
      continue;
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
  } while(!quit && !check_win(board));
  printf("YOU WIN!\n");
}
