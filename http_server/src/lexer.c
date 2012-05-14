#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
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
