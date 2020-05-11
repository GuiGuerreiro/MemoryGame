#include <stdlib.h>

typedef struct board_place{
  char v[3];
} board_place;

typedef struct play_response{
  int code; // 0 - filled
            // 1 - 1st play
            // 2 2nd - same play
            // 3 END
            // -2 2nd - diffrent
  int play1[2]; //play1[X] play1[Y]
  int play2[2];
  char str_play1[3], str_play2[3];
} play_response;

typedef struct wait_play{
	play_response resp;
	int* play;
} wait_play;

char * get_board_place_str(int i, int j);
void init_board(int dim, int first_game);
play_response board_play (int x, int y, int play[2]);
void reset_state(int x, int y);
