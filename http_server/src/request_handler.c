#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "header.h"
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
	int rfd;
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
