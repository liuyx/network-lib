#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>

#define MAXLINE 1024

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

#endif
