// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define ON 1
#define OFF 0
#define END 128
#define GET 129
#define OTHER 130
#define HTML 131
#define JS 132
// declare structs
struct table {
	char *path;
	int fd;
};

struct ftable {
	struct table *table;
	int size;
};

typedef struct token {
	int type;
	char *value;
} Token;

struct lexer {
	char *start;
	char *end;
	int flag;
};

struct ftable ft;
//declare functions

int createSocket(int *);

int Bind(int *, struct sockaddr_in *);

int Accept(int *, int *, struct sockaddr_in *, int *);

int Read(int *, char *, int);

int Write(int *, char *, int);

int getDiff(struct lexer *);

int handleResponse(int *);

int parseGet(char *, char *, int *);

int formatPath(char *);

int restartLex(struct lexer *);

Token *getToken(char *, struct lexer *);

int determineDocType(char *, char *, int *);

int countChar(char *);

int addResponse(char *, char*, int *);

int handleFileError(char *, int *);

int makeSocketNB(int *);

int createpoll(void);

void initFt(void);

//main 
int main(int argc, char *argv[]) {
	char *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n, pid, epollfd;
	struct sockaddr_in serv_addr, cli_addr;
	initFt();
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
						fprintf (stderr, "we have accepted all the clients on this event");
						break;
					} event.data.fd = newsockfd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newsockfd, &event)<0) { 						fprintf(stderr, "\nepoll_ctl error\n");
						if (errno == EEXIST)
							fprintf(stderr, "fd already registered\n");
						else if (errno == EBADF) {
							fprintf(stderr, "\nfd bad\n%d\n", newsockfd);
							break;
						}else if (errno == ENOMEM)
							fprintf(stderr, "\nno memory\n");
						else if (errno == ENOSPC)
							fprintf(stderr, "\n enospc\n");
						exit(EXIT_FAILURE);
					}
					continue;
				}
			} else { // there is stuff for us to read
					handleResponse(&events[n].data.fd);
					fprintf(stderr, "we are about to close file descriptor %d\n", events[n].data.fd);
					close (events[n].data.fd);
					fprintf(stderr, "connection closed\n");
					continue;
			}
		}
	} exit(EXIT_SUCCESS);
};
// function definitons
// socket creation and binding
int createSocket(int *fd) {
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0))>=0)
		return 1;
	else {
		exit(EXIT_FAILURE);
	}
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

int makeSocketNB (int *sfd) {
	int flags, s;
	flags = fcntl (*sfd, F_GETFL, 0);
	if (flags == -1) {
		perror ("fcntl");
		return -1;
	} flags |= O_NONBLOCK;
	s = fcntl(*sfd, F_SETFL, flags);
	if (s == -1) {
		perror ("fcntl2");
		return -1;
	}
	return 0;
};

int createpoll(void) {
	int fd;
	if ((fd=epoll_create(SOMAXCONN))<0)
		return -1;
	else
		return fd;
};
// file handling
void initFt(void) {
	ft.size = 0;
	ft.table = malloc(sizeof(struct table));
	ft.table[0].path = malloc(sizeof(char));
	*ft.table[0].path = 'g';
	ft.table[0].fd = -1;
};

int Read(int *fd, char *buf, int buf_siz) {
	if (read(*fd, buf, buf_siz)<0) {
		fprintf(stderr, "Read Error\n");
		return 1; //keep on pushing
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

int handleFileError(char *response, int *i) {
	addResponse(response, "HTTP/1.0 404 Not Found\n\n", i);
	addResponse(response, "<!DOCTYPE HTML><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>", i);
	return 1;
};
//Lexing
int getDiff(struct lexer *lex) {
	lex->start = lex->end;
	int i=0;
	if (*lex->end==EOF || *lex->end=='\0') {
		fprintf(stderr, "LEX ERROR, LEXER.END IS ON EOF OR 0\n");
		exit(EXIT_FAILURE);
	} else {
		for (*lex->end; *lex->end!=' ' && *lex->end!='\n' && *lex->end!=EOF && *lex->end!='\0'; ++lex->end) {
			fprintf(stderr, "%d\n", i);
			i++;
	}
	} return 1;
};

int restartLex(struct lexer *lex) {
	while (*lex->end==' ' || *lex->end=='\n') {
		++lex->end;
		if (*lex->end=='\0' || *lex->end==EOF) {
			} return 1;
	}return 1;
};


Token *getToken(char *request, struct lexer *lex) {
	int diff, n;
	char *tmp;
	Token *tok = malloc(sizeof(Token));
	if (lex->flag==ON) { // get the lexer back on track
		restartLex(lex);
	}
	lex->flag = ON;
	if (*lex->end==EOF || *lex->end=='\0') { //check if we're at end of input
		tok->type=END;
		tok->value = malloc(sizeof(char));
		*tok->value = *lex->end;
		return tok;
	} getDiff(lex);
	diff = lex->end - lex->start;
	tmp = malloc(diff+1 * sizeof(char));
	strncpy(tmp, lex->start, diff);
	tmp[diff+1] = '\0';
	if((n=Checksym(tmp))<0) {
		tok->type = OTHER;
		tok->value = malloc(diff+1 * sizeof(char));
		strcpy(tok->value, tmp);
		free(tmp);
		return tok;
	} tok->type=n; //otherwise, the type is defined in the symbol table
	restartLex(lex);
	getDiff(lex);
	diff = lex->end - lex->start;
	free(tmp);
	tmp = malloc(diff+1 * sizeof(char));
	strncpy(tmp, lex->start, diff);
	tmp[diff+1] = '\0';
	tok->value = malloc(diff+1 * sizeof(char));
	strcpy(tok->value, tmp);
	free(tmp);
	return tok;
};
//request parsing and response building

int  handleResponse(int *fd) {
	char buf[BUFSIZ], retbuf[BUFSIZ] //bufsiz is the maximum allowable request
	int i = 0;
	Token tok;
	Read(*fd, buf, BUFSIZ);
	tok = parseRequest(buf);
	if tok.type == GET {
		addResponse("HTTP/1.0 200 OK\n", retbuf, &i);
		addResponse("Server: KevServer/0.3\n", retbuf, &i);
		parseGet(buf, retbuf, &i);
		Write(*fd, retbuf, strlen(retbuf));
		return 1; 
	} else {
		addResponse("Unkown Request Type\n", retbuf, &i)
		Write(*fd, retbuf, strlen(retbuf));
		return 1;
	}
};

int parseGet(char *request, char *response, int *i) {
	Token *tok;
	struct lexer lex;
	int nc=0, rfd;
	char *path;
	char tmp[BUFSIZ];

	tok = getToken(request, &lex);
	memset(tmp, '\0', BUFSIZ);
	if tok.type == GET {
		path = malloc(sizeof(char) * strlen(tok.value));
		strcpy(path, tok.value);
		determineDocType(path, response, i);
		path = formatPath(path);
		if(rfd=inFt(path)<0) {
			rfd = open(path, O_RDONLY);
			addFt(rfd);
			Read(rfd, tmp, BUFSIZ);
			addResponse(tmp, response, i);
			free(path);
			free(tok);
			return 1;
		} else {
			pread(rfd, tmp, BUFSIZ, 0);
			addResponse(tmp, response, i);
			free(path);
			free(tok);
			return 1;
		}
	}
};

char *formatPath(char *path) {
	char *new_path;
	if path[0]!='.' {
		new_path = malloc((sizeof(char) * strlen(path))+1);
		new_path[0] = '.';
		strcpy(&new_path[1], path);
		free(path);
		return new_path;
	} else {
		return path;
	}
};

int determineDocType(char *path, char *response, int *i) {
	int n=1;
	char *tmp;
	if (strlen(path)<=2) {
		strcpy(&response[*i], "Content-Type: text; charset=utf-8\n\n");
		return 1;
	}while (path[n]!='.') {
		n++;
	}
	tmp = malloc((strlen(path)-n) * sizeof(char));
	strcpy(tmp, &path[n]);
	if((n=Checksym(tmp))<0) {
		fprintf(stderr, "FILE TYPE NOT SUPPORTED\n");
		exit(EXIT_FAILURE);
	}if (n==HTML) {
		strcpy(&response[*i], "Content-Type: text/html; charset=utf-8\n\n");
		*i = countChar(response);
	}else if (n==JS) {
		strcpy(&response[*i], "Content-Type: text/javascript; charset=utf-8\n\n");
		*i = countChar(response);
	} return 1;
};

int countChar(char *buf){
	int nc=0;
	while(buf[nc]!='\0') {
		nc++;
	}
	return nc;
};

int addResponse(char *response, char *src, int *i) {
	strcpy(&response[*i], src);
	*i = countChar(response);
	return 1;
};
