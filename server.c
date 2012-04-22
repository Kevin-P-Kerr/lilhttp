// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
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
#define INDEX "/index.html"
#define ROOT "/"

typedef struct token {
	int type;
	char *value;
} Token;

struct lexer {
	char *start;
	char *end;
	int flag;
};

FILE *efopen(char *path) {
	FILE *fp;
	
	if ((fp=fopen(path, "r"))==NULL){
		fprintf(stderr, "ERROR COULD NOT OPEN %s\n", path);
		exit(EXIT_FAILURE);
	} return fp;
};

int CreateSocket(int *fd) {
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

int GetDiff(struct lexer *lex) {
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

int RestartLex(struct lexer *lex) {
	while (*lex->end==' ' || *lex->end=='\n') {
		++lex->end;
		if (*lex->end=='\0' || *lex->end==EOF) {
			} return 1;
	}return 1;
};

int Checksym(char *string) { // returns a symbol value if string is in table
	static struct pair {
		char *key;
		int value;
	} table[] = {
		"GET", GET,
		".html", HTML,
		".js", JS
	};

	int i=0, tablelen=2;
	while(i<=tablelen) {
		if (strcmp(table[i].key, string)==0)
        {
            return table[i].value;
        }
		++i;
	} if (i>tablelen) { // unable to find symbol
		return -1;
	} else {
		fprintf(stderr, "ERROR IN CHECKSYM\n");
		exit(EXIT_FAILURE);
	}
};

Token *GetToken(char *request, struct lexer *lex) {
	int diff, n;
	char *tmp;
	Token *tok = malloc(sizeof(Token));
	if (lex->flag==ON) { // get the lexer back on track
		RestartLex(lex);
	}
	lex->flag = ON;
	if (*lex->end==EOF || *lex->end=='\0') { //check if we're at end of input
		tok->type=END;
		tok->value = malloc(sizeof(char));
		*tok->value = *lex->end;
		return tok;
	} GetDiff(lex);
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
	RestartLex(lex);
	GetDiff(lex);
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

int ParseHeaders(Token *tok, char *response, char *request, struct lexer *lex, int *i) { // not parsing headers for right now
	int n; // length for strncpy
	fprintf(stderr, "We Are Parsing The Headers\n");
	strncpy(&response[*i], "Server: KevServer/0.1\n", strlen("Server: KevServer/0.1\n"));
	*i = CountChar(response);
	return 1;
};

int DetermineDocType(char *path, char *response, int *i) {
	int n=1;
	char *tmp;
	while (path[n]!='.') {
		n++;
	}
	tmp = malloc((strlen(path)-n) * sizeof(char));
	strcpy(tmp, &path[n]);
	if((n=Checksym(tmp))<0) {
		fprintf(stderr, "FILE TYPE NOT SUPPORTED\n");
		exit(EXIT_FAILURE);
	}if (n==HTML) {
		strcpy(&response[*i], "Content-Type: text/html; charset=utf-8\n\n");
		*i = CountChar(response);
	}else if (n==JS) {
		strcpy(&response[*i], "Content-Type: text/javascript; charset=utf-8\n\n");
		*i = CountChar(response);
	} return 1;
};

int CountChar(char *buf){
	int nc=0;
	while(buf[nc]!='\0') {
		nc++;
	}
	return nc;
};

int LoadFile(FILE *resource, char *response, int *i) {
		while(fgets(&response[*i], 124, resource)!=NULL) {
			*i = CountChar(response);
		}
		return 1;
};

int  ParseGet(Token *tok, char *response, char *request, struct lexer *lex, int *i) {
	FILE *resource;
	char buf[BUFSIZ], *path;

	if (CheckRedirect(tok, response, i)) {
		Redirect(tok, response, request, lex, i);
		return 1;
	}path = malloc(strlen(tok->value)+1 * sizeof(char));
	path[0] = '.';
	strcpy(&path[1], tok->value);
	if ((resource=fopen(path, "r"))==NULL) {
		fprintf(stderr, "We Could Not Open The File\nThe Path Was\n%s\n", path);
		strcpy(response, "HTTP/1.0 404 Not Found\n\n<!DOCTYPE HTML> <html> <head> <title>404:Could Not Find File</title> </head> <h1> 404: Could Not Find File </h1> </body> </html>");
		*i = CountChar(response);
		free(tok->value);
		free(tok);
		return 1; 
	} else {
		strcpy(response, "HTTP/1.0 200 OK\n");
		*i = CountChar(response);
		fprintf(stderr, "We Could Open The File\nThe Path Was\n%s\n", path);
		free(tok->value);
		free(tok);
		tok=GetToken(request, lex);
		if (tok->type==END)
			return 1; // end the parse process
		ParseHeaders(tok, response, request, lex, i);
		DetermineDocType(path, response, i);
		LoadFile(resource, response, i);

		while(fgets(&response[*i], 124, resource)!=NULL) {
			*i = CountChar(response);
		} //fclose(resource);
		return 1;
	}
};

int CheckRedirect(Token *tok, char *response, int *i) {
	static struct pair {
		char *key;
		char *value;
	} table[] = {
		ROOT, INDEX
	};
	
	int n=0;
	int tablen = 0;

	while (n<=tablen) {
		if (strcmp(tok->value, table[n].key)==0) {
			free(tok->value);
			tok->value = malloc(sizeof(char) * (strlen(table[n].value)+1));
			strcpy(tok->value, table[n].value);
			return 1;
		} ++n;
	}return 0;
};

int Redirect(Token *tok, char *response, char *request, struct lexer *lex, int *i) {
	FILE *resource;
	char *path;

	path = malloc(sizeof(char) * strlen(tok->value)+1);
	path[0] = '.';
	strcpy(&path[1], tok->value);
	resource = efopen(path);
	strcpy(&response[*i], "HTTP/1.0 301 Moved Permamently\n");
	*i = CountChar(response);
	strcpy(&response[*i], tok->value);
	*i = CountChar(response);
	strcpy(&response[*i], "\n\n");
	*i = CountChar(response);
	LoadFile(resource, response, i);
	return 1;
};	
		

int BuildResponse(char *request, char *response, int *fd, int *i) {
	struct lexer lex;
	lex.start = request;
	lex.end = request;
	lex.flag = OFF;
	Token *tok;
	tok = GetToken(request, &lex);
	if (tok->type == GET) { // begin dispatching on token type
		ParseGet(tok, response, request, &lex, i);
	} fprintf(stderr, "%s\n", tok->value);
	fprintf(stderr, "%d\n", tok->type);
	return 1;
};

int HandleResponse(int *fd, char *buf) {
	char retbuf[(4 * BUFSIZ)+1];
	int i=0; // index into retbuf
	Read(fd, buf, BUFSIZ);
	fprintf(stderr,"We are in HANDLERESPONSE\nTHE REUQEST IS\n%s\n", buf);
	memset(retbuf, '\0', (4*BUFSIZ)+1);
	BuildResponse(buf, retbuf, fd, &i);
	fprintf(stderr, "We are in HANDLERESPONSE\nTHE REPSONSE IS\n%s\n", retbuf);
	Write(fd, retbuf, i);
	return 1;
};

/*int Log(char *buf) {
	int fd;
	FILE *log;
	if ((log=fopen("./log", "w"))==NULL) {
		if ((fd=creat("./log", "w"))<0) {
			fprintf(stderr, "ERROR CREATING LOG FILE\n");
			exit(EXIT_FAILURE);
		} log=fopen("./log", "w");
	} fputs(buf, log);
}; */

int MakeSocketNB (int *sfd) {
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

int Createpoll(void) {
	int fd;
	if ((fd=epoll_create(SOMAXCONN))<0)
		return -1;
	else
		return fd;
};

int main(int argc, char *argv[]) {
	char *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n, pid, epollfd;
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
	struct epoll_event *events = calloc(SOMAXCONN,  sizeof(struct epoll_event));
	struct epoll_event event;
	MakeSocketNB(&sockfd);
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = sockfd;
	if ((epollfd = Createpoll())<0) {
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
						MakeSocketNB(&newsockfd);
						fprintf(stderr, "Accepted a new connection on fd %d, made nonblocking\n", newsockfd);
					} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
						fprintf (stderr, "we have accepted all the clients on this event");
						break;
					} event.data.fd = newsockfd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newsockfd, &event)<0) {
						fprintf(stderr, "epoll_ctl error\n");
						exit(EXIT_FAILURE);
					}
					continue;
				}
			} else { // there is stuff for us to read
					HandleResponse(&events[n].data.fd, buffer);
					fprintf(stderr, "we are about to close file descriptor %d\n", events[n].data.fd);
					close (events[n].data.fd);
					fprintf(stderr, "connection closed\n");
					continue;
			}
		}
	} exit(EXIT_SUCCESS);
};
