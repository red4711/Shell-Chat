#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>

#define MAXDATASIZE 2048
#define PORT 1234

char name[256];

void error(char *mgs){
	perror(mgs), exit(1);
}

void server_receive_handler(int socketfd){
	char buffer[MAXDATASIZE];
	int numbytes;
	if ((numbytes = read(socketfd, buffer, MAXDATASIZE)) < 0)
		error("Read socketfd error");
	buffer[numbytes] = '\0';
	printf("\r%s\n", buffer);
}

void stdin_receive_handler(int socketfd, char *name){
	char buffer[MAXDATASIZE], temp[MAXDATASIZE];
	int numbytes;
	if ((numbytes = read(0, temp, MAXDATASIZE)) < 0)
		error("Read stdin error");
	temp[numbytes - 1] = '\0';
	if(strcmp(temp, "quit") == 0){
		char s[100];
		sprintf(s, "%s has left the chatroom", name);
		if (write(socketfd, s, strlen(s)) < 0)
			error("Write error");
		close(socketfd);
		exit(0);
	}
	sprintf(buffer, "%s%s", name, temp);
	if (write(socketfd, buffer, strlen(buffer)) < 0)
		error("Write error");
}

void initlize_name(char *blank_name){
	time_t t;
	srand((unsigned) time(&t));
	int text = rand() % 7;
	sprintf(name, "<\033[0;%dm%s\033[0m>: ", text + 31, blank_name);
}

int main(int argc, char **argv){
	int socketfd;
	struct sockaddr_in server_address;
	struct hostent* he;
	fd_set rfd, read_rfd;

	if (argc != 4)
		error("Not enough input");

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERROR on opening socket"); 

	if ((he = gethostbyname(argv[2])) == NULL)
		error("ERROR on gethostbyname");

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(argv[3]));
	server_address.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
	memset(&(server_address.sin_zero), '\0', 8);

	if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(struct sockaddr)) < 0)
		error("ERROR on connecting");

	initlize_name(argv[1]);
	{
		char temp[256];
		sprintf(temp, "%shas joined the chatroom", name);
		write(socketfd, temp, strlen(temp));
	}
	
	FD_ZERO(&rfd);
	FD_SET(socketfd, &rfd);
	FD_SET(0, &rfd);

	while(1){
		printf("%s", name);
		fflush(stdout);
		FD_ZERO(&read_rfd);
		read_rfd = rfd;
		switch(select(socketfd + 1, &read_rfd, NULL, NULL, (struct timeval*)NULL)){
		case -1:
			error("ERROR on select");
		case 0:
			break;
		default:
			if (FD_ISSET(0, &read_rfd))
				stdin_receive_handler(socketfd, name);
			else if (FD_ISSET(socketfd, &read_rfd))
				server_receive_handler(socketfd);
		}
	}
	return 0;
}

