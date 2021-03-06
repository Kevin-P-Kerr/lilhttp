#define ON 1
#define OFF 0
#define END 128
#define GET 129
#define OTHER 130
#define HTML 131
#define JS 132
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "dbg.h"
// declare structs
struct table {
	char *path;
	int fd;
};

struct ftable {
	struct table *table;
	int size;
	int flag;
};

typedef struct token {
	int type;
	char *value;
} Token;

struct rToken {
	char *request;
	char *path;
};

struct lexer {
	char *start;
	char *end;
	int flag;
};

struct ftable ft;
//function prototypes

int createSocket(int *);

int Bind(int *, struct sockaddr_in *);

int Accept(int *, int *, struct sockaddr_in *, socklen_t *);

int Read(int *, char *, int);

int Write(int *, char *, int);

int getDiff(struct lexer *);

int handleResponse(int *);

Token *parseRequest(char *);

int parseGet(char *, char *, int *);

char *formatPath(char *);

int restartLex(struct lexer *);

void initLex(struct lexer *, char *);

Token *getToken(char *, struct lexer *);

int checkSym(char *);

int determineDocType(char *, char *, int *);

int countChar(char *);

int addResponse(char *, char*, int *);

int handleFileError(char *, int *);

int inFt(char *);

int makeSocketNB(int *);

int createpoll(void);

void initFt(void);

int addFt(char *, int);

char *getConfig(void);

int scan(struct rToken *, struct lexer *, char *);

int parse(void);

void c(char *);
