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
#include <fcntl.h>
#include <signal.h>

#define MAXLINE 1024
#define LISTNQ 1024

#define max(a,b)	((a) > (b) ? (a) : (b))

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

int tcp_connect(const char *hot,const char *service);

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);
ssize_t readline(int fd, void *ptr, size_t maxlen);


void cli_heartbeat(int sockfd, int nsec, int max_try_times);
void serv_heartbeat(int sockfd, int nsec, int max_try_times);

enum type {
	SERVER,CLIENT
};

/**
  *
  * alias for void (*sig_func)(int) function pointer
  */
typedef void sig_func(int);

/**
  * an simply function wrap for sigaction
  */
sig_func *my_signal(int signo, sig_func *func);

/**
 * start communication
 */
void start_communication(enum type, const char *self_name, const char *other_name, int sockfd, FILE *fp);

#endif
