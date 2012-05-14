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
#include "header.h"
// function definitons
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
// file handling
void initFt(void) {
	c("in initFt");
	ft.size = 0;
	ft.table = malloc(sizeof(struct table));
	ft.table[0].path = malloc(sizeof(char));
	*ft.table[0].path = 'g';
	ft.table[0].fd = -1;
	ft.flag = 0;
	c("out of initFt");
};

int inFt(char *path) {
	c("in inFT");
	int i=0;
	for (i; i<=ft.size; i++) {
		if (strcmp(ft.table[i].path, path) == 0) {
			c("out of inFt");
			return ft.table[i].fd;
		}
	} c("out of inFt");
	return -1;
};

int addFt(char *path, int fd) {
	c("in addFT");
	if (ft.flag == 0) {
		ft.flag == 1;
		free(ft.table[0].path);
		ft.table[0].path = malloc(sizeof(char) * strlen(path));
		strcpy(ft.table[0].path, path);
		ft.table[0].fd = fd;
	} else {
		ft.size++;
		ft.table = realloc(ft.table, sizeof(struct table) * (ft.size + 1));
		ft.table[ft.size].path = malloc(sizeof(char) * strlen(path));
		strcpy(ft.table[ft.size].path, path);
		ft.table[ft.size].fd = fd;
	} c("out of addFt");
	return 1;
};

int Read(int *fd, char *buf, int buf_siz) {
	c("in Read");
	if (read(*fd, buf, buf_siz)<0) {
		fprintf(stderr, "Read Error\n");
		c("out of Read");
		return 1; //keep on pushing
	}else {
		c("out of Read");
		return 1;
	}
};

int Write(int *fd, char *msg, int len) {
	c("in Write");
	fprintf(stderr, "%s\n", msg);
	if (write(*fd, msg, len)<0) {
		fprintf(stderr, "Write Error\n");
		c("out of Write");
		exit(EXIT_FAILURE);
	} else {
		c("out of Write");
		return 1;
	}
};

int handleFileError(char *response, int *i) {
	c("in handleFileError");
	addResponse("HTTP/1.0 404 Not Found\n\n", response, i);
	addResponse("<!DOCTYPE HTML><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>", response, i);
	c("out of handleFileError");
	return 1;
};
//Lexing
int getDiff(struct lexer *lex) {
	c("in getDiff");
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
	} c("out of getDiff");
	return 1;
};

void initLex(struct lexer *lex, char *buf) {
	c("in initLex");
	lex->start = buf;
	lex->end = buf;
	lex->flag=OFF;
	c("out of initLex");
};

int restartLex(struct lexer *lex) {
	c("in restartLex");
	while (*lex->end==' ' || *lex->end=='\n') {
		++lex->end;
		if (*lex->end=='\0' || *lex->end==EOF) {
			c("out of restartLex");
			return 1;
		}
	} c("out of restartLex");
	return 1;
};

Token *getToken(char *request, struct lexer *lex) {
	c("in getToken");
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
		c("out of getToken");
		return tok;
	} getDiff(lex);
	diff = lex->end - lex->start;
	tmp = calloc(diff+1, sizeof(char));
	strncpy(tmp, lex->start, diff);
	tmp[diff+1] = '\0';
	if((n=checkSym(tmp))<0) {
		tok->type = OTHER;
		tok->value = malloc(diff+1 * sizeof(char));
		strcpy(tok->value, tmp);
		free(tmp);
		c("out of getToken");
		return tok;
	} tok->type=n; //otherwise, the type is defined in the symbol table
	restartLex(lex);
	getDiff(lex);
	diff = lex->end - lex->start;
	free(tmp);
	tmp = calloc(diff+1,  sizeof(char));
	strncpy(tmp, lex->start, diff);
	tmp[diff+1] = '\0';
	tok->value = malloc(diff+1 * sizeof(char));
	strcpy(tok->value, tmp);
	free(tmp);
	c("out of getToken");
	return tok;
};

int checkSym(char *string) { // returns a symbol value if string is in table
	c("in checksym");
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
			c("out of checkSym");
            return table[i].value;
        }
		++i;
	} if (i>tablelen) { // unable to find symbol
		c("out of checkSym");
		return -1;
	} else {
		fprintf(stderr, "ERROR IN CHECKSYM\n");
		exit(EXIT_FAILURE);
	}
};

//request parsing and response building

int  handleResponse(int *fd) {
	c("in handleResponse");
	char buf[BUFSIZ], retbuf[2*BUFSIZ]; //bufsiz is the maximum allowable request
	int i = 0;
	memset(buf, '\0', BUFSIZ-1);
	memset(retbuf, '\0', (2*BUFSIZ)-1);
	Token *tok;
	Read(fd, buf, BUFSIZ);
	fprintf(stderr, "The Request is:\n%s\n", buf);
	tok = parseRequest(buf);
	if (tok->type == GET) {
		free(tok);
		parseGet(buf, retbuf, &i);
		Write(fd, retbuf, strlen(retbuf));
		c("out of handleResponse");
		return 1; 
	} else {
		addResponse("Unknown Request Type\n", retbuf, &i);
		Write(fd, retbuf, strlen(retbuf));
		c("out of handleResponse");
		return 1;
	}
};

Token *parseRequest(char *request) {
	c("in parseRequest");
	Token *tok;
	struct lexer lex;
	initLex(&lex, request);
	tok = getToken(request, &lex);
	c("out of parseRequest");
	return tok;
};

int parseGet(char *request, char *response, int *i) {
	c("in parseGet");
	Token *tok;
	struct lexer lex;
	int nc=0, rfd;
	char *path;
	char tmp[2*BUFSIZ];

	initLex(&lex, request);
	tok = getToken(request, &lex);
	memset(tmp, '\0', 2*BUFSIZ);
	if (tok->type == GET) {
		path = malloc(sizeof(char) * strlen(tok->value));
		strcpy(path, tok->value);
		path = formatPath(path);
		if((rfd=inFt(path))<0) {
			if ((rfd = open(path, O_RDONLY))<0) {
				handleFileError(response, i);
				free(path);
				free(tok);
				c("out of parseGet");
				return 1;
			} else {
			addFt(path, rfd);
			Read(&rfd, tmp, 2*BUFSIZ);
			}
		} else {
			pread(rfd, tmp, 2*BUFSIZ, 0);
		}
		free(tok);
		addResponse("HTTP/1.0 200 OK\n", response, i);
		addResponse("Server: KevServer/0.3\n", response, i);
		determineDocType(path, response, i);
		free(path);
		addResponse(tmp, response, i);
		c("out of parseGet");
		return 1;
	} else { // we only handle GET requests for right now
		addResponse("ERROR!\n", response, i);
		free(path);
		free(tok);
		c("out of parseGet");
		return 1;
	}
};

char *formatPath(char *path) {
	c("in formatPath");
	char *new_path;
	if (path[0]!='.') {
		new_path = malloc((sizeof(char) * strlen(path))+1);
		new_path[0] = '.';
		strcpy(&new_path[1], path);
		free(path);
		c("out of formatPath");
		return new_path;
	} else {
		c("out of formatPath");
		return path;
	}
};

int determineDocType(char *path, char *response, int *i) {
	c("in determineDocType");
	int n=1;
	char *tmp;
	if (strlen(path)<=2) {
		addResponse("Content-Type: text; charset=utf-8\n\n", response, i);
		c("out of determineDocType");
		return 1;
	}while (path[n]!='.') {
		n++;
	}
	tmp = malloc((strlen(path)-n) * sizeof(char));
	strcpy(tmp, &path[n]);
	if((n=checkSym(tmp))<0) {
		fprintf(stderr, "FILE TYPE NOT SUPPORTED\n");
		addResponse("Content-Type: text; charset=utf-8\n\n", response, i);
	}else if (n==HTML) {
		addResponse("Content-Type: text/html; charset=utf-8\n\n", response, i);
	}else if (n==JS) {
		addResponse("Content-Type: text/javascript; charset=utf-8\n\n", response, i);
	} c("out of determineDocType");
	return 1;
};

int countChar(char *buf){
	c("in countChar");
	int nc=0;
	while(buf[nc]!='\0') {
		nc++;
	} c("out of countChar");
	fprintf(stderr, "%d\n", nc);
	return nc;
};

int addResponse(char *response, char *resbuf, int *i) {
	c("in addResponse");
	fprintf(stderr, "i is: %d\n", *i);
	strcpy(&resbuf[*i], response);
	*i = countChar(resbuf);
	c("out of addResponse");
	return 1;
};

void c(char *string) {
	fprintf(stderr, "%s\n", string);
};
