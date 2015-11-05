#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define MAXLINE 1024
#define LISTNQ 1024

#define max(a,b)	((a) > (b) ? (a) : (b))

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

int tcp_connect(const char *hot,const char *service);

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);
ssize_t readline(int fd, void *ptr, size_t maxlen);

#endif
