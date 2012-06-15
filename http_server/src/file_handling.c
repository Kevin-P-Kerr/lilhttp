#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "header.h"
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
	int i;
	for (i=0; i<=ft.size; i++) {
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
		ft.flag = 1;
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
