#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#define MAXLINE 1024

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

int tcp_connect(const char *fmt,const char *service);

#endif
