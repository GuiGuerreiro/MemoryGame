#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "UI_library.h"
#include "client_library.h"


int main(int argc, char * argv[]){
	SDL_Event event;
	pthread_t recv_thread;
	int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	int done = 0;
	int exitplay[2] = {-1, -1};

	if (argc < 2){
    	printf("second argument should be server address\n");
    	exit(-1);
  	}
  	if (sock_fd == -1){
    	perror("socket: ");
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
	if(connect_server(argv[1], sock_fd) == -1){
		printf("Failed connecting to server\n");
		exit(-1);
	}

	//read(sock_fd, buff, sizeof(buff));
	//printf("buff: %s", buff);

	//receive board size from server and inits window
	create_board_window(300, 300, board_size(sock_fd));


	if(pthread_create(&recv_thread, NULL, recv_play, &sock_fd)){
		printf("Failed to create receive thread\n");
		exit(-1);
	}

	while(!done){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:{
					write(sock_fd, exitplay, sizeof(int)*2);
					done = SDL_TRUE;
					break;
				}
				case SDL_MOUSEBUTTONDOWN:{
					send_play(event.button.x, event.button.y, sock_fd);
					break;
				}
			}
		}
	}
}
