/* this describes the processes that initialize the router.  
	a lexer reads off a router configuration file of the following format:
	<request name> <file path> /n
	and creates tokens representing the request name and file path.  the 
	routines described in file_handler.c are then used to add the files to the ftable initalized in main */

#include <stdio.h>

struct rToken {
	char *request;
	char *path;
};

char *getConfig(void) {
	char config_buf[BUFSIZ], *config_path = "./config.svr";
	FILE *cf = fopen(config_path, "r");
	fgets(config_buf, BUFSIZ, cf);
	return cf;
};

int scan(struct rToken *tok, struct lexer *lex, char *config) {
	if (*lex->start == '\0') { // eof
		return 0;
	// part 1
	if (*lex->end == '\n') {
		++lex->end;
	} while (*lex->end != ' ') {
		++lex->end;
	} int d = lex->end = lex->start;
	char *tmp = malloc(sizeof(char) * (++d));
	for (int i = 0; i < d; i++) {
		tmp[i] = lex->start[i];
	} tmp[i] = '\0';
	tok->request = malloc(sizeof(char) * d);
	strcpy(tok->request, tmp);
	free(tmp);
	lex->start = lex->end;
	//part 2
	++lex->start;
	while (*lex->end != '\n' && *lex->end != '\0') {
		++lex->end;
	} d = lex->end - lex->start;
	tmp = malloc(sizeof(char) * (++d));
	for (i=0; i < d; i++) {
		tmp[i] = lex->start[i];
	} tmp[i] == '\0';
	tok->path = malloc(sizeof(char) * d);
	strcpy(tok->path, tmp);
	free(tmp);
	lex->start = lex->end;
	return 1;
	}
};

int parse() {
	struct rToken tok;
	struct lexer lex;
	char *config = getConfig();
	lexer.start = config;
	lexer.end = config;
	int fd;
	
	while (scan(&tok, &lex, &config)) {
		fd = open(tok.path, O_RDONLY);
		addFt(tok.request, fd);
		free(tok.request);
		free(tok.path);
	} return 1;
};
