// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char ttyin(void) {
	FILE *tty;
	char nc;
	if ((tty=fopen("/dev/tty", "r"))==NULL) {
		fprintf(stderr, "ttyin error\n");
		exit(EXIT_FAILURE);
	} if ((nc=fgetc(tty))==EOF) { // nothing happened
		return 'c';
	} else {
		return nc;
	}
};

int main(int argc, char *argv[]) {
	char nc, *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n;
	char buffer[BUFSIZ];
	struct sockaddr_in serv_addr, cli_addr;
	if (argc < 2 ) {
		fprintf(stderr, "\nERROR: No Port Provided\n");
		exit(EXIT_FAILURE);
	}if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		fprintf(stderr, "\nERROR: %s Could Not Open Socket\n", progname);
		exit(EXIT_FAILURE);
	}bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) {
		fprintf(stderr, "ERROR: %s Error On Bindings\n", progname);
		exit(EXIT_FAILURE);
	} listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	if ((newsockfd=accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))<0){
		fprintf(stderr, "%s: ERROR ON ACCEPT\n", progname);
		exit(EXIT_FAILURE);
	} bzero(buffer, BUFSIZ);
	if ((n=read(newsockfd, buffer, BUFSIZ))<0) {
		fprintf(stderr, "%s, READ ERROR\n", progname);
		exit(EXIT_FAILURE);
	}printf("Here is the message %s\n", buffer);
	n = write(newsockfd, "I got your message", 18);
	if (n<0) {
		fprintf(stderr, "%s, ERROR WRITING TO SOCKET", progname);
		exit(EXIT_FAILURE);
	}exit(EXIT_SUCCESS);
}; 
