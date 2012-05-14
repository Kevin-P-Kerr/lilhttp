#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include "header.h"

// socket creation and binding
int createSocket(int *fd) {
	c("in createSocket");
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0))>=0) {
		c("out of createSocket");
		return 1;
	} else {
		c("out of createSocket");
		exit(EXIT_FAILURE);
	}
};

int Bind(int *fd, struct sockaddr_in *skaddr) {
	c("in Bind");
	if (bind(*fd, (struct sockaddr *) skaddr, sizeof(*skaddr))<0) {
		fprintf(stderr, "BIND ERROR\n");
		exit(EXIT_FAILURE);
	} else {
		c("out of bind");
		return 1;
	}
};

int Accept(int *nsfd, int *sfd, struct sockaddr_in *cli_addr, int *clilen) {
	c("in Accept");
	if ((*nsfd = accept(*sfd, (struct sockaddr *) cli_addr, clilen))<0) {
		fprintf(stderr, "ACCEPT ERROR\n");
		exit(EXIT_FAILURE);
	} else {
		c("out of accept");
		return 1;
	}
};

int makeSocketNB (int *sfd) {
	c("in makeSocketNB");
	int flags, s;
	flags = fcntl (*sfd, F_GETFL, 0);
	if (flags == -1) {
		perror ("fcntl");
		return -1;
	} flags |= O_NONBLOCK;
	s = fcntl(*sfd, F_SETFL, flags);
	if (s == -1) {
		perror ("fcntl2");
		c("out of makeSocketNB");
		return -1;
	}
	c("out of makeSocketNB");
	return 0;
};

int createpoll(void) {
	c("in createpoll");
	int fd;
	if ((fd=epoll_create(SOMAXCONN))<0) {
		c("out of createpoll");
		return -1;
	} else {
		c("out of createpoll");
		return fd;
	}
};
