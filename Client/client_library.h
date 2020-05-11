#include <stdlib.h>


typedef struct _game_info
{
	int status; //0=game end; 1=game playing
	int text_color[3];
	int card_color[3];
	int coord[2];
	char text[2];
	
}game_info;

int connect_server(char* sv_address, int sock_fd);
void *recv_play(void* sock_fd);
void send_play(int x, int y, int sock_fd);
int board_size(int sock_fd);