// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int CreateSocket(int *fd) {
	*fd = socket(AF_INET, SOCK_STREAM, 0);
	return 1;
};

int Bind(int *fd, struct sockaddr_in *skaddr) {
	if (bind(*fd, (struct sockaddr *) skaddr, sizeof(*skaddr))<0) {
		fprintf(stderr, "BIND ERROR\n");
		exit(EXIT_FAILURE);
	} else {
		return 1;
	}
};

int Accept(int *nsfd, int *sfd, struct sockaddr_in *cli_addr, int *clilen) {
	if ((*nsfd = accept(*sfd, (struct sockaddr *) cli_addr, clilen))<0) {
		fprintf(stderr, "ACCEPT ERROR\n");
		exit(EXIT_FAILURE);
	} else {
		return 1;
	}
};

int Read(int *fd, char *buf, int buf_siz) {
	if (read(*fd, buf, buf_siz)<0) {
		fprintf(stderr, "Read Error\n");
		exit(EXIT_FAILURE);
	}else {
		return 1;
	}
};

int Write(int *fd, char *msg, int len) {
	if (write(*fd, msg, len)<0) {
		fprintf(stderr, "Write Error\n");
		exit(EXIT_FAILURE);
	} else {
		return 1;
	}
};

int main(int argc, char *argv[]) {
	char *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n;
	char buffer[BUFSIZ];
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2 ) {
		fprintf(stderr, "\nERROR: No Port Provided\n");
		exit(EXIT_FAILURE);
	} if(!CreateSocket(&sockfd)) {
		fprintf(stderr, "%s ERROR, COULD NOT CREATE SERVER SOCKET\n", progname);
		exit(EXIT_FAILURE);
	} serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	Bind(&sockfd, &serv_addr);
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	Accept(&newsockfd, &sockfd, &cli_addr, &clilen);
	bzero(buffer, BUFSIZ); // reset buffer
	Read(&newsockfd, buffer, BUFSIZ);
	Write(&newsockfd, "I got your message!", 18);
	exit(EXIT_SUCCESS);
}; 
