#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "UI_library.h"
#include "client_library.h"



struct sockaddr_in server_addr;
int dim=0;

int board_size(int sock_fd){
	printf("Acquiring board info\n");
	while(!dim){
		read(sock_fd, &dim, sizeof(int));
	}
	printf("Board info received\n");
	return dim;
}

void clean_board(){
	int i,j;
	for(i=0; i<dim; i++){
		for(j=0; j<dim; j++){
			paint_card(i, j, 255, 255, 255);
		}
	}
	return;
}

int connect_server(char* sv_address, int sock_fd){


	server_addr.sin_family = AF_INET;
	server_addr.sin_port= htons(3000);
	inet_aton(sv_address, &server_addr.sin_addr);

  	return connect(sock_fd,
			(const struct sockaddr *) &server_addr,
			sizeof(server_addr));
}

void *recv_play(void* sock_fd){

	int *sock = (int*) sock_fd;
	game_info play;
	play.status=1;
	
	while(1){
		if(read(*sock, &play, sizeof(play))){
			printf("received play info\n");
			if(play.coord[0] == -1){
				printf("END GAME\n");
				sleep(10);
				clean_board();
				continue;
			}
			paint_card(play.coord[0], play.coord[1], play.card_color[0], play.card_color[1], play.card_color[2]);
			write_card(play.coord[0], play.coord[1], play.text, play.text_color[0], play.text_color[1], play.text_color[2]);
		}
	}
	return NULL;
}

void send_play(int x, int y, int sock_fd){
	
	int grid_coord[2];
	get_board_card(x, y, &grid_coord[0], &grid_coord[1]);
	write(sock_fd, grid_coord ,sizeof(int) * 2);
	printf("Sent client play (%d,%d)\n", grid_coord[0], grid_coord[1]);
	return;
}