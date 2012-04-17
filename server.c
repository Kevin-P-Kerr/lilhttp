// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#define ON 1
#define OFF 0
#define END 128
#define GET 129
#define OTHER 130
#define HTML 131
#define JS 132

typedef struct token {
	int type;
	char *value;
} Token;

struct lexer {
	char *start;
	char *end;
	int flag;
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

int  ParseInitalLine(Token *tok, char *response, char *request, struct lexer *lex, int *i) {
	FILE *resource;
	char buf[BUFSIZ], *path;
	if (tok->type==GET) {
		path = malloc(strlen(tok->value)+1 * sizeof(char));
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
			while(fgets(&response[*i], 124, resource)!=NULL) {
				*i = CountChar(response);
			}
			return 1;
		}
	}
};

int BuildResponse(char *request, char *response, int *fd, int *i) {
	struct lexer lex;
	lex.start = request;
	lex.end = request;
	lex.flag = OFF;
	Token *tok;
	tok = GetToken(request, &lex);
	fprintf(stderr, "%s\n", tok->value);
	fprintf(stderr, "%d\n", tok->type);
	ParseInitalLine(tok, response, request, &lex, i);
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

int Log(char *buf) {
	int fd;
	FILE *log;
	if ((log=fopen("./log", "w"))==NULL) {
		if ((fd=creat("./log", "w"))<0) {
			fprintf(stderr, "ERROR CREATING LOG FILE\n");
			exit(EXIT_FAILURE);
		} log=fopen("./log", "w");
	} fputs(buf, log);
};

int main(int argc, char *argv[]) {
	char *progname=argv[0];
	int sockfd, newsockfd, portno, clilen, n, pid;
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
	while (1) {
			Accept(&newsockfd, &sockfd, &cli_addr, &clilen);
			if ((pid=fork())<0) {
				fprintf(stderr, "%s, Forking Error\n", progname);
				exit(EXIT_FAILURE);
			} if (pid==0) {
				HandleResponse(&newsockfd, buffer);
				exit(EXIT_SUCCESS);
			} else {
				close(newsockfd);
		}
	} exit(EXIT_SUCCESS);
}; 
