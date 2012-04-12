// A simple inet server //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define ON 1
#define OFF 0
#define END 128

typedef struct token {
	int type;
	char *value;
} Token;

struct lexer {
	char start;
	char end;
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

int GetDiff(stuct lexer *lex) {
	if (*lex->end==EOF || *lex->end=='\0') {
		fprintf(stderr, "LEX ERROR, LEXER.END IS ON EOF OR 0\n");
		exit(EXIT_FAILURE);
	} else {
		for (*lex->end; *lex->end!=' ' && lex->end!=EOF && lex->end!='\0'; ++lex->end)
			;
	} return 1;
};

int RestartLex(Lexer *lex) {
	while (*lex->end==' ' || *lex->end=='\n') {
		++lex->end;
	} if (*lex->end=='\0' || *lex->end==EOF) {
	} return 1;
};

Token *GetToken(char *request, struct lexer *lex) {
	int diff, n;
	char *tmp;
	Token *tok = malloc(sizeof(Token));
	if (lex->flag==ON) { // get the lexer back on track
		RestartLex(lex);
	}
	GetDiff(lex);
	diff = lex->end - lex->start;
	tmp = malloc(diff+1 * sizeof(char));
	strncpy(tmp, lex->start, diff);
	tmp[diff+1] = '\0';
	if(!n=Checksym(&tmp)) {
		fprintf(stderr, "Parse Error, Problem Parsing %s\n", tmp);
		exit(EXIT_FAILURE);
	} tok->type=n;
	while (*lex->end==' ') { // skip whitespace
		if (*lex->end ==EOF || *lex->end == '\0')
			break;
		++lex->end;
	}lex->start = lex->end;
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
}

char *BuildResponse(char *request) {
	struct lexer lex;
	lex.start = response;
	lex.end = resonse;
	lex.flag = OFF;
	char *response[BUFSIZ];
	Token *tok;
	tok = GetToken(request, &lex);
	if

int HandleResponse(int *fd, char *buf) {
	char retbuf[BUFSIZ+1];
	Read(fd, buf, BUFSIZ);
	retbuf=BuildResponse(buf);
	Write(fd, retbuf, BUFSIZ+1);
	return 1;
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
