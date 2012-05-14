// A simple web  server //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "header.h"

int main(int argc, char *argv[]) {
	c("in main");
	c("running unit tests");
	//char *result = all_tests();
/*	if (*result != 0) {
		c("TESTS FAILED!");
		c(result);
		exit(EXIT_FAILURE);
	} c("ALL TESTS PASSED"); */
	//fprintf(stderr, "Tests run: %d\n", tests_run); 
	char *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n, pid, epollfd;
	struct sockaddr_in serv_addr, cli_addr;
	initFt();
	clilen = 0;
	if (argc < 2 ) {
		fprintf(stderr, "\nERROR: No Port Provided\n");
		exit(EXIT_FAILURE);
	} if(!createSocket(&sockfd)) {
		fprintf(stderr, "%s ERROR, COULD NOT CREATE SERVER SOCKET\n", progname);
		exit(EXIT_FAILURE);
	} serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	Bind(&sockfd, &serv_addr);
	listen(sockfd, 5);
	struct epoll_event *events = calloc(SOMAXCONN,  sizeof(struct epoll_event));
	struct epoll_event event;
	makeSocketNB(&sockfd);
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = sockfd;
	if ((epollfd = createpoll())<0) {
		fprintf(stderr, "epoll create error\n");
		exit(EXIT_FAILURE);
	} if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
		fprintf(stderr, "error with epoll_ctl on sockfd");
		exit(EXIT_FAILURE);
	}while (1) {
		int n, e; //e for events
		e = epoll_wait(epollfd, events, SOMAXCONN, -1);
		for (n=0; n<e; n++) {
			if ((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP) || !(events[n].events & EPOLLIN)) {
				fprintf (stderr, "epoll error\n");
				close (events[n].data.fd);
				continue;
			} else if (events[n].data.fd == sockfd) {
				while (1) {
					newsockfd=accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					if (newsockfd>0) {
						makeSocketNB(&newsockfd);
						fprintf(stderr, "Accepted a new connection on fd %d, made nonblocking\n", newsockfd);
					} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
						fprintf (stderr, "we have accepted all the clients on this event\n");
						break;
					} else {
						if (errno == ENETDOWN) 
							c("ENETDOWN");
						else if (errno == EPROTO)
							c("EPROTO");
						else if (errno == ENOPROTOOPT)
							c("ENOPROTOOPT");
						else if (errno == EHOSTDOWN)
							c("EHOSTDOWN");
						else if (errno == ENONET)
							c("ENONET");
						else if (errno == EHOSTUNREACH)
							c("EHOSTUNREACH");
						else if (errno == EOPNOTSUPP) 
							c("EOPNOTSUPP");
						else if (errno == ENETUNREACH)
							c("ENETUNREACH");
						else {
							c("what is going on here?");
							perror(sys_errlist[errno]);
						} while (newsockfd<0) {
							newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					 }
					} event.data.fd = newsockfd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newsockfd, &event)<0) {
						fprintf(stderr, "epoll_ctl error\n");
						if (errno == EEXIST)
							fprintf(stderr, "fd already registered\n");
						else if (errno == EBADF) {
							fprintf(stderr, "fd bad\n%d\n", newsockfd);
							break;
						}else if (errno == ENOMEM)
							fprintf(stderr, "no memory\n");
						else if (errno == ENOSPC)
							fprintf(stderr, "enospc\n");
						exit(EXIT_FAILURE);
					}
					continue;
				}
			} else { // there is stuff for us to read
					handleResponse(&events[n].data.fd);
					fprintf(stderr, "we are about to close file descriptor %d\n", events[n].data.fd);
					close (events[n].data.fd);
					fprintf(stderr, "connection closed\n");
			}
		} continue;
	} exit(EXIT_SUCCESS);
};
