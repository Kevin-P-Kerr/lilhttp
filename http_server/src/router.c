/* this describes the processes that initializes the router.
	a lexer reads off a router configuration file of the following format:
	<request name> <file path> /n
	and creates tokens representing the request name and file path.  the 
	routines described in file_handler.c are then used to add the files to the ftable initalized in main */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "header.h"

char *getConfig(void) {
	char *config_buf, *config_path = "./config.svr";
	FILE *cf = fopen(config_path, "r");
	config_buf = malloc(sizeof(char) * BUFSIZ);
	memset(config_buf, '\0', BUFSIZ);
	int i = 0;
	while (fgets(&config_buf[i], BUFSIZ, cf)) {
		i = strlen(config_buf);
	} if (config_buf[i-1] == '\n')
		config_buf[i-1] = '\0'; // kludge
	return config_buf;
};

int scan(struct rToken *tok, struct lexer *lex, char *config) {
	int i;

	if (*lex->start == '\0') // eof
		return 0;
	// part 1
	if (*lex->end == '\n') {
		++lex->end;
		lex->start = lex->end;
	} while (*lex->end != ' ') {
		++lex->end;
	} int d = (lex->end - lex->start);
	char *tmp = malloc(sizeof(char) * (d+1));
	for (i = 0; i < d; i++) {
		tmp[i] = lex->start[i];
	} tmp[i] = '\0';
	tok->request = malloc(sizeof(char) * (d+1));
	strcpy(tok->request, tmp);
	free(tmp);
	lex->start = (++lex->end);
	//part 2
	while (*lex->end != '\n' && *lex->end != '\0') {
		++lex->end;
	} d = (lex->end - lex->start);
	tmp = malloc(sizeof(char) * (d+1));
	for (i=0; i < d; i++) {
		tmp[i] = lex->start[i];
	} tmp[i] = '\0';
	tok->path = malloc(sizeof(char) * (d+1));
	strcpy(tok->path, tmp);
	free(tmp);
	lex->start = lex->end;
	return 1;
};

int parse() {
	struct rToken tok;
	struct lexer lex;
	char *config = getConfig();
	lex.start = config;
	lex.end = config;
	int fd;
	
	while (scan(&tok, &lex, config)) {
		fd = open(tok.path, O_RDONLY);
		if (fd<0)
			perror("error in parse");
		addFt(tok.request, fd);
		free(tok.request);
		free(tok.path);
	} return 1;
};
