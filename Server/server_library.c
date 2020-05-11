#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server_library.h"
#ifndef BOARD_LIBRARY_H
#include "board_library.h"
#endif
#ifndef UI_LIBRARY_H
#include "UI_library.h"
#endif

#define MAXPLAYERS 50


int* nplayers;
struct sockaddr_in server, client;
int color[MAXPLAYERS][4]; //[4 colors][R=0, G=1, B=2, ASSIGNED=3] //[4 colors][R=0, G=1, B=2, ASSIGNED=3]
playerls *head= NULL;
int wait_time = 0;
pthread_mutex_t mutex;
pthread_mutex_t players;
pthread_mutex_t game_done;
pthread_t new_game;

void* Wait_new_game(){
	sleep(10);
	wait_time=0;
	pthread_exit(NULL);
}

void init_color(){
	int i=4;
	//orange
	color[0][0] = 255; color[0][1] = 128; color[0][2] = 0; color[0][3] = 0;
	//yellow
	color[1][0] = 255; color[1][1] = 255; color[1][2] = 0; color[1][3] = 0;
	//pink
	color[2][0] = 255; color[2][1] = 0; color[2][2] = 128; color[2][3] = 0;
	//blue
	color[3][0] = 0; color[3][1] = 128; color[3][2] = 255; color[3][3] = 0;
	while(i<MAXPLAYERS){
		color[i][3]=0;
		i++;
	}
}

play_info fillinfo(int status, int t1, int t2, int t3, int c1, int c2, int c3, int coord[2], char text[2]){
	play_info info;
	info.status = status;
	info.text_color[0]=t1;
	info.text_color[1]=t2;
	info.text_color[2]=t3;
	info.card_color[0]=c1;
	info.card_color[1]=c2;
	info.card_color[2]=c3;
	info.coord[0]=coord[0];
	info.coord[1]=coord[1];
	info.text[0]=text[0];
	info.text[1]=text[1];
	return info;
}

wait_play fillwaitinfo(play_response resp, int sock_fd, int play1[2]){
		wait_play wait_info;
		wait_info.resp = resp;
		wait_info.play = play1;
		return wait_info;
}

void* IgnorePlays(void* info){
	int ignored_plays[2];
	ignore_info ignore = *(ignore_info*)info;
	while(1){
		if(*ignore.disconnect == 0 && read(ignore.sock, &ignored_plays, sizeof(ignored_plays)) <= 0){
			*ignore.disconnect = 1;
			pthread_mutex_lock(&mutex);
			remove_player(ignore.sock);
			pthread_mutex_unlock(&mutex);
		}

	}
	pthread_exit(NULL);
}

void* WaitTime(void* info){

	wait_play wait = *(wait_play*)info;
	play_info play;
	sleep(5);
	if(wait.play[0] !=-1){
		wait.play[0] = -1;
		reset_state(wait.resp.play1[0], wait.resp.play1[1]);
		paint_card(wait.resp.play1[0], wait.resp.play1[1] , 255, 255, 255);
		play=fillinfo(0, 255, 255, 255, 255, 255, 255, wait.resp.play1, "\0");
		send_draw_info(play);
		paint_card(wait.resp.play2[0], wait.resp.play2[1] , 255, 255, 255);
		play=fillinfo(0, 255, 255, 255, 255, 255, 255, wait.resp.play2, "\0");
		send_draw_info(play);
	}
	pthread_exit(NULL);
}

playerls *create_node(){
	playerls *node;
	return node = (playerls*) malloc(sizeof(playerls));
}

void insertnode(playerls* player_node){
	playerls *aux = head;

	if(head == NULL)
		head = player_node;
	else{
		while(aux->next != NULL){
			aux = aux->next;
		}
		aux->next = player_node;
	}
}

void Decrement_nplayers(){
	pthread_mutex_lock(&players);
	*nplayers=*nplayers-1;
	pthread_mutex_unlock(&players);
	return;
}


void remove_player(int sock){
	playerls *prev, *temp;
	temp = head;
printf("Client disconnected\n");
	Decrement_nplayers();
	if (temp != NULL && temp->client.sock == sock){ 
        head = temp->next;
        free_colour(temp->client.sock);
        free(temp);         
        return; 
    }    
    while (temp != NULL && temp->client.sock != sock){ 
        prev = temp; 
        temp = temp->next; 
    } 
    if (temp == NULL) return;

    prev->next = temp->next;
    free_colour(temp->client.sock); 
    free(temp);
} 


player insert_player(int sock,int player_ID, int* nplayers){
	playerls *new_player = create_node();
	int *new_color;
	new_color = assign_color(sock);
	new_player->client.sock = sock;
	new_player->client.playerID = player_ID;
	new_player->client.nplayers=nplayers;
	new_player->client.color[0] = new_color[0];
	new_player->client.color[1] = new_color[1];
	new_player->client.color[2] = new_color[2];
	new_player->next = NULL;
	insertnode(new_player);
	return new_player->client;
}

int* assign_color(int ID){
	int i=0;
	srand(time(NULL));

	while(color[i][3] != 0 )
	{
		if (i > 3){
			color[i][0]= rand()%256;
			color[i][1]= rand()%256;
			color[i][2]= rand()%256;
			return color[i];
		}
		i++;
	}
	color[i][3] = ID;
	return color[i];
}
void free_colour(int player_ID)
{
	for(int i =0; i<MAXPLAYERS;i++)
		if(color[i][3] == player_ID)
			color[i][3] = 0;

	return;
}

int init_server(int sock_fd){

		int on = 1; 
		init_color();
		server.sin_family = AF_INET;	//defining type of adress for socket
  		server.sin_port= htons(3000); //using the port 3000
  		server.sin_addr.s_addr= INADDR_ANY;
		setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  	return bind(sock_fd, (struct sockaddr *)&server, sizeof(server));
}

void *PlayGame(void* client_info){
	int playcoord[2];
	int play1[2];
	int done = 0;
	SDL_Event event;
	player client = *(player*) client_info;
	nplayers = client.nplayers;		//sets global pointer to nplayers in server.c
	play_info play;
	ignore_info info_connection;
	play_response resp;

	pthread_t wait;
	pthread_t ignore;
	pthread_t waiting;

	play1[0]=-1;
	info_connection.sock = client.sock;
	info_connection.disconnect = &done;

	while(!done)
	{	
		if(wait_time){
			pthread_create(&waiting, NULL, IgnorePlays, (void*)&info_connection);
			while(!pthread_join(new_game, NULL));
			pthread_cancel(waiting);
		}
		if(read(client.sock, &playcoord, sizeof(playcoord)) > 0){
			printf("%d\n", *nplayers);
			if(*nplayers<2){
				pthread_create(&ignore, NULL, IgnorePlays, (void*)&info_connection);
				while(*nplayers<2);
				pthread_cancel(ignore);
				continue;
			}
			if(playcoord[1]==-1){
				pthread_mutex_lock(&mutex);
				remove_player(client.sock);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}
			printf("Received play (%d %d)\n", playcoord[0], playcoord[1]);
			resp = board_play(playcoord[0], playcoord[1], play1);
			wait_play wait_info = fillwaitinfo(resp, client.sock, play1);
			SDL_PollEvent(&event);

			switch (event.type) {
				case SDL_QUIT: {
					done = SDL_TRUE;
					break;
					}
				default:
					switch (resp.code) {
						case 1:
							paint_card(resp.play1[0], resp.play1[1] , client.color[0], client.color[1], client.color[2]); //mudar
							write_card(resp.play1[0], resp.play1[1], resp.str_play1, 200, 200, 200);
							play=fillinfo(0, 200, 200, 200, client.color[0], client.color[1], client.color[2], resp.play1, resp.str_play1);
							send_draw_info(play);
							pthread_create(&wait, NULL, WaitTime, (void*)&wait_info);
							break;
						case 3:
						  done = 1;
						  wait_time=1;
						case 2:
							pthread_cancel(wait);
							paint_card(resp.play1[0], resp.play1[1] , client.color[0], client.color[1], client.color[2]);
							write_card(resp.play1[0], resp.play1[1], resp.str_play1, 0, 0, 0);
							play=fillinfo(0, 0, 0, 0, client.color[0], client.color[1], client.color[2], resp.play1, resp.str_play1);
							send_draw_info(play);
							paint_card(resp.play2[0], resp.play2[1] , client.color[0], client.color[1], client.color[2]);
							write_card(resp.play2[0], resp.play2[1], resp.str_play2, 0, 0, 0);
							play=fillinfo(0, 0, 0, 0, client.color[0], client.color[1], client.color[2], resp.play2, resp.str_play2);
							send_draw_info(play);
							break;
						case -2:
							pthread_cancel(wait);
							pthread_create(&ignore, NULL, IgnorePlays, (void*)&info_connection);
							paint_card(resp.play1[0], resp.play1[1] , client.color[0], client.color[1], client.color[2]);
							write_card(resp.play1[0], resp.play1[1], resp.str_play1, 255, 0, 0);
							play=fillinfo(0, 255, 0, 0, client.color[0], client.color[1], client.color[2], resp.play1, resp.str_play1);
							send_draw_info(play);
							paint_card(resp.play2[0], resp.play2[1] , client.color[0], client.color[1], client.color[2]);
							write_card(resp.play2[0], resp.play2[1], resp.str_play2, 255, 0, 0);
							play=fillinfo(0, 255, 0, 0, client.color[0], client.color[1], client.color[2], resp.play2, resp.str_play2);
							send_draw_info(play);
							sleep(2);
							reset_state(resp.play1[0], resp.play1[1]);
							paint_card(resp.play1[0], resp.play1[1] , 255, 255, 255);		
							play=fillinfo(0, 255, 255, 255, 255, 255, 255, resp.play1, "");																										
							send_draw_info(play);	
							reset_state(resp.play2[0], resp.play2[1]);
							paint_card(resp.play2[0], resp.play2[1] , 255, 255, 255);
							play=fillinfo(0, 255, 255, 255, 255, 255, 255, resp.play2, "");
							send_draw_info(play);
							pthread_cancel(ignore);
							break;
					}
				}
			}else{
				remove_player(client.sock);
				break;
				}
			if(done){
				pthread_create(&new_game,NULL, Wait_new_game, NULL);
				play1[0]=-1;
				play=fillinfo(0, 0, 0, 0, 0, 0, 0, play1, "");
				send_draw_info(play);
				init_board(0,0);
				done=0;
			}
		}
	
	pthread_exit(NULL);
}

 void send_draw_info(play_info play){
 	playerls *aux = head;
 	while(aux != NULL){

 		write(aux->client.sock, &play, sizeof(play));
 		aux = aux->next;
 	}
 	return;
 }




