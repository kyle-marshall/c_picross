
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "picross.h"

#define DEBUG 0

/* the picross board struct (stores puzzle and user input) */
typedef struct pic_board_st
{
  int w;
  int h;
  char* cell_values; /* stored as one long array, row after row */
  char* cell_marks;
  int** h_clues;
  int** v_clues;
} pic_board;

/* PROTOTYPES */
static void gen_clues(pic_board* board);

int check_win(pic_board* board){
  int i, max;
  char* vals;
  char* marks;
  vals = board->cell_values;
  marks = board->cell_marks;
  max = board->w * board->h;
  for(i=0;i<max;i++){
    if(vals[i] == CELL_FILLED &&
       marks[i] != MARK_FILLED)
      return 0;
    else if(vals[i] == CELL_EMPTY &&
	    marks[i] == MARK_FILLED)
      return 0;
  }
  return 1;
}

/* HELPERS */
/* maximum horizontal clues = ceiling of the width / 2 */
static int max_h_clues(int w){
  int m = w / 2;
  if(w%2==1)
    m++;
  return m;
}

static int max_v_clues(int h){
  int m = h / 2;
  if(h%2==1)
    m++;
  return m;
}

/* create a new picross board of a given size */
pic_board* new_pic_board(int w, int h){
  int cell_ct, i, max;
  pic_board* result = (pic_board*)malloc(sizeof(pic_board));
  result->w = w;
  result->h = h;
  cell_ct = w * h;

  result->cell_values = (char*)calloc(cell_ct, sizeof(char));
  result->cell_marks = (char*)calloc(cell_ct, sizeof(char));

  // makimum possible clues in column or row is found by
  // considering the case of a 'checkerboard' puzzle
  result->h_clues = (int**)malloc(h*sizeof(int*));
  result->v_clues = (int**)malloc(w*sizeof(int*));
  max = max_h_clues(w);
  for(i=0;i<h;i++){
    result->h_clues[i] = (int*)calloc(max, sizeof(int));
  }
  max = max_v_clues(h);
  for(i=0;i<w;i++){
    result->v_clues[i] = (int*)calloc(max, sizeof(int));
  }
  
  return result;
}

pic_board* rand_pic_board(int w, int h){
  int i, c;
  pic_board* result = new_pic_board(w, h);
  c = w * h;
  for(i=0;i<c;i++)
    result->cell_values[i] = (rand() % 3 != 0);
  gen_clues(result);
  return result;
}

char* get_cell_value(pic_board* b, int r, int c){
  return &b->cell_values[r * b->w + c];
}

char* get_cell_mark(pic_board* b, int r, int c){
  return &b->cell_marks[r * b->w + c];
}

static
void mark_cell(pic_board* b, int r, int i, char mark_type){
  *get_cell_mark(b,r,i) = mark_type;
}

static void gen_clues(pic_board* board){
  int i, j, w, h;
  int max;
  int* clue_ptr;
  char* cell_ptr;
  int group_size;
  w = board->w;
  h = board->h;

  max = max_h_clues(w);
  /* calculate horizontal clues row by row */
  for(i=0;i<h;i++){
    board->h_clues[i] = (int*)calloc(max, sizeof(int));
    clue_ptr = board->h_clues[i];
    group_size = 0;
    for(j=w-1;j>=0;j--){
      switch(*get_cell_value(board, i, j)){
      case CELL_FILLED:
	group_size++;
	break;
      default:
      case CELL_EMPTY:
	if(group_size > 0){
	  *clue_ptr = group_size;
	  *clue_ptr++;
	  group_size = 0;
	}
	break;
      }
    }
    /* add the last group on that row if applicable*/
    if(group_size > 0){
      *clue_ptr = group_size;
    }
  }

  /* calculate vertical clues col by col */
  max = max_v_clues(board->h);
  for(i=0;i<w;i++){
    board->v_clues[i] = (int*)calloc(max, sizeof(int));
    clue_ptr = board->v_clues[i];
    cell_ptr = board->cell_values;
    group_size=0;
    for(j=h-1;j>=0;j--){
      switch(*get_cell_value(board, j, i)){ /* this time, j decides which row is accessed */
      case CELL_FILLED:
	group_size++;
	break;
      case CELL_EMPTY:
	if(group_size > 0){
	  *clue_ptr = group_size;
	  *clue_ptr++;
	  group_size = 0;
	}
	break;
      }
    }
    /* add the last group */
    if(group_size > 0){
      *clue_ptr = group_size;
    }
  }  
}

static char mark_char(int m){
  switch(m){
  default:
  case MARK_NONE:
    return ' ';
  case MARK_FILLED:
    return '#';
  case MARK_CROSS:
    return 'X';
  }
}

static int pow(int x, int y){
  if(y == -1){
    return 0;
  }
  if(y == 0){
    return 1;
  }
  
  int res = x;
  int i;
  for(i=0;i<y;i++){
    res = res * x;
  }
  return res;
}

void get_alpha_index(int i, char* buf){

  int rem = i;
  
  int p = 0;
  int temp;
  if(i == 0)
    {
      buf[0] = 'A';
      buf[1] = '\0';
      return;
    }
  i = 0;
  while(pow(26, p+1) < rem)
    p++;
  
  while(rem > 0){
    temp = pow(26, p);
    
    buf[i] = 'A' + (char)(rem / temp);
    
    
    
    rem -= (rem / temp) * temp;
    p--;
    i++;
  }
  buf[i] = '\0';
}

int alpha_to_int_index(char* alpha){
  int sum = 0;
  int a_len = strlen(alpha);
  int p = a_len - 1;
  int i;
  char c;
  for(i=0;i <a_len;i++){
    c = alpha[i];
    sum += (int)(c - 'A') * pow(26, p);
    p--;
  }
  return sum;
}

/* display the picross board to stdio */
void print_pic_board(pic_board* board){
  int i,j;
  int r_i, c_i;
  char h_wall = '-';
  char v_wall = '|';
  char* mark_ptr = board->cell_marks;
  char* cell_ptr = board->cell_values;
  int cell_s = 3;
  int rows = board->h * cell_s + 1;
  int cols = board->w * cell_s + 1;
  int hc_max, vc_max;
  int hc_len = 3; /* 3 chars per horizontal clue */
  int hc_width; /* total width of horizontal clue field */
  int* clue_ptr;
  int linum_len = 2;
  char c;
  int rem;
  char col_alpha[10];
  /* the first thing printed is the vertical clues 
     which must be offset by max_h_clues char */
  vc_max = max_v_clues(board->h);
  hc_max = max_h_clues(board->w);
  
  hc_width = hc_max * hc_len + linum_len + 2; /* +2 for the ") " after each line num */

  if(DEBUG){
    printf("cell_values: ");
    j = board->h*board->w;
    for(i = 0; i < j; i++){
      printf("[%d / %d] ", board->cell_values[i], board->cell_marks[i]);
      if(i%board->w==board->w-1)
	printf("\n");
    }
    printf("\n");
  }

  /* print letter column names */
  for(i=0;i<hc_width;i++)
    printf(" ");
  for(i=0;i<board->w;i++){
    get_alpha_index(i, col_alpha);
    printf("%2s ", col_alpha, col_alpha);
  }
  printf("\n");
  
  for(i=vc_max-1;i>=0;i--){
    /* pad each row with spaces */
    for(j=0;j<hc_width;j++){
      printf(" ");
    }
    /* cols will alternate from containing a space and a clue */
    for(c_i=0;c_i<cols;c_i++){
      rem = c_i % cell_s;
      if(rem == 1){	
	/* c_i / 2 = current column index */
	clue_ptr = board->v_clues[c_i / cell_s];
	if(clue_ptr[i]==0){
	  printf("   ");
	}
	else{
	  printf(" %2d", clue_ptr[i]);
	}
      }
      /* not printing if rem == 2 */
      /* this will need to be changed if cell_s changes from 3 */
      
    }
    printf("\n");
  }

  
  for(r_i=0;r_i<rows;r_i++){
    rem = r_i % cell_s;
    if(rem==0){
      /* pad each row with spaces */
      for(j=0;j<hc_width;j++){
	printf(" ");
      }
      /* print the horiz walls */
      for(c_i=0;c_i<cols;c_i++)
	printf("%c", h_wall);
    }
    else{
      if(rem == 1){
	/* pad each row with spaces */
	for(j=0;j<hc_width;j++){
	  printf(" ");
	}
      }
      else if(rem == 2){
	/* prefix clues with line num */
	printf("%2d) ",r_i / cell_s + 1);
	/* print the clues */
	clue_ptr = board->h_clues[r_i / cell_s];
	for(i=hc_max-1; i>= 0; i--){
	  if(clue_ptr[i] == 0){
	    printf("   ");
	  }
	  else{
	    printf(" %2d", clue_ptr[i]);
	  }
	}
      }
      /* print alternating vert wall and cell marks */
      for(c_i=0;c_i<cols;c_i++){
	/* reuse rem variable */
	rem = c_i % cell_s;
	if(rem==0){
	  printf("%c", v_wall);
	}
	else{
	  c = mark_char(*get_cell_mark(board, r_i / cell_s, c_i / cell_s));
	  //c = mark_char((int)mark_ptr[r_i * board->w + c_i / cell_s]);
 	  //c = mark_char((int)cell_ptr[r_i * board->w + c_i / cell_s]);
	  printf("%c", c);
	}
	
      }
    }
    printf("\n");
  }
}


