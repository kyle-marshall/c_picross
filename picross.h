#ifndef PICROSS
#define PICROSS

#define CELL_EMPTY 0
#define CELL_FILLED 1

#define MARK_NONE 0
#define MARK_FILLED 1
#define MARK_CROSS 2

typedef struct pic_board_st pic_board;
pic_board* new_pic_board(int w, int h);
pic_board* rand_pic_board(int w, int h);
void print_pic_board(pic_board*);

int alpha_to_int_index(char* alpha);
char* get_cell_value(pic_board* b, int r, int c);
char* get_cell_mark(pic_board* b, int r, int c);

#endif
