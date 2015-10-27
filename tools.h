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

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

int tcp_connect(const char *fmt,const char *service);

#endif