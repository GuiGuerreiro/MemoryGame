#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#ifndef UI_LIBRARY_H
#include "UI_library.h"
#endif
#ifndef SERVER_LIBRARY_H
#include "server_library.h"
#endif
#ifndef BOARD_LIBRARY_H
#include "board_library.h"
#endif
#ifndef MAXPLAYERS
#define MAXPLAYERS 50
#endif

int dim;

int main(int argc, char* argv[])
{
	int i=0;
	int nplayers = 0;
	int sock_client; //10 player max
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	//int addr_len = sizeof(struct sockaddr_in);
	pthread_t client_tid;
	player player_info;
	if(argc < 2){
		printf("second argument should be size of board\n");
		exit(-1);
	}

	dim = atoi(argv[1]);
	if(dim%2)
	{
		printf("size of board should be an even number\n");
		exit(-1);
	}

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		exit(-1);
	}
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	// Defines server information and binds
	if (init_server(sock_fd) < 0){
		perror("socket: ");
		exit(-1);
	}
	printf(" socket created and binded \n");

	create_board_window(300, 300, dim);
	init_board(dim, 1);


	printf("Waiting for players\n");
	listen(sock_fd, 5);

	while(nplayers<MAXPLAYERS){
			sock_client = accept(sock_fd, NULL, NULL);
			i++;
			nplayers++;
			player_info = insert_player(sock_client, i, &nplayers);
			write(sock_client, &dim, sizeof(dim));
			pthread_create(&client_tid, NULL, PlayGame, (void*)&player_info);
			printf("Client connected\n");
			printf("Nplayers: %d\n", nplayers);
	}

	//freeAll();
	return 0;
}
