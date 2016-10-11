#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define BACKLOG 10
#define MAXDATASIZE 2048

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void error(char *mgs){
	perror(mgs);
	exit(1);
}

void relay_all(int main_socket, int current_socket, int maxfd, char *buffer, int buffer_len){
	for (int j = 3; j < maxfd + 1; j++)
		if (j != current_socket && j != main_socket && fcntl(j, F_GETFD) != -1)
			if (write(j, buffer, buffer_len) < 0)
				error("ERROR write");
}

int main(int argc, char **argv){
	int socketfd, newsockfd, client_len, maxfd;
	fd_set rfd, c_rfd;
	sockaddr_in server_addr, client_addr;
	char buffer[MAXDATASIZE];

	if (argc != 2)
		error("Not enough inputs");


	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERROR opening socket");

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(atoi(argv[1]));
	memset(&(client_addr.sin_zero), 0, 8);

	int one = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

	if (bind(socketfd, (sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		error("ERROR on binding");

	listen(socketfd,BACKLOG);

	FD_ZERO(&rfd);
	FD_SET(socketfd, &rfd);
	maxfd = socketfd;

	while(1){
		FD_ZERO(&c_rfd);
		c_rfd = rfd;
		if (select(maxfd + 1, &c_rfd, NULL, NULL, (struct timeval *)NULL) < 0)
			error("ERROR on select");
		if (FD_ISSET(socketfd, &c_rfd)){
			if ((newsockfd = accept(socketfd, (sockaddr *) &client_addr, (socklen_t *) &client_len)) < 0)
				continue;
			FD_SET(newsockfd, &rfd);
			maxfd = maxfd > newsockfd ? maxfd : newsockfd;
			int numbytes = read(newsockfd, buffer, MAXDATASIZE);
			buffer[numbytes] = '\0';
			relay_all(socketfd, newsockfd, maxfd, buffer, numbytes);
			continue;
		}
		for (int i = 0 ; i < maxfd + 1; i++){
			if (i != socketfd && FD_ISSET(i, &c_rfd)){
				int numbytes;
				switch((numbytes = read(i, buffer, MAXDATASIZE))){
				case -1:
					error("Read error");
					break;
				case 0:
					close(i);
					FD_CLR(i, &rfd);
					break;
				default:
					buffer[numbytes] = '\0';
					relay_all(socketfd, i, maxfd, buffer, numbytes);
				}
			}
		}
		bzero(buffer, MAXDATASIZE);
	}
	close(socketfd);
	return 0;
}


