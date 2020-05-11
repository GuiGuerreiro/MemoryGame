#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char * argv[]){

	int dim=0;
	int coord[2];
	int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	//pid_t pid=getpid();

	if (argc < 2){
    	printf("second argument should be server address\n");
    	exit(-1);
  	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port= htons(3000);
	inet_aton(argv[1], &server_addr.sin_addr);
	connect(sock_fd,
			(const struct sockaddr *) &server_addr,
			sizeof(server_addr));

	//receive board size from server and inits window
	while(!dim){
		read(sock_fd, &dim, sizeof(int));
	}
	printf("Board info received\n");
	//init_board(dim);

	printf("READY TO REKT\n");
	srand(time(NULL));
    while(1){
    	coord[0]=rand()%dim; coord[1]=rand()%dim;
    	write(sock_fd, coord, sizeof(int)*2);
    	usleep(500);
    }



}
