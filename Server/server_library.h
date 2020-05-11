#include <stdlib.h>
#include <unistd.h>


typedef struct _player
{
	int sock;
	int color[3]; //color assigned to each player
	int playerID;
	int *nplayers;
}player;

typedef struct _playerls
{
	player client;
	struct _playerls *next;
}playerls;

typedef struct _play_info
{
	int status; //0=game end; 1=game playing
	int text_color[3];
	int card_color[3];
	int coord[2];
	char text[2];
}play_info;

typedef  struct _igore_info
{
	int sock;
	int *disconnect;	//indica se cliente saiu enquanto eram ignoradas as jogadas 
}ignore_info;




playerls *create_node();
void insertnode(playerls* player_node);
player insert_player(int sock, int player_ID, int* nplayers);
void remove_player(int sock);
int init_server(int sock);
void* PlayGame(void*);
void send_draw_info(play_info play);
play_info fillinfo(int status, int t1, int t2, int t3, int c1,
	int c2, int c3, int coord[2], char text[2]);
void free_colour(int player_ID);
int* assign_color(int ID);