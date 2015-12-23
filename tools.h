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

#include <syslog.h>

#ifdef __linux__
#include <sys/epoll.h>
#endif

#define DEBUG 

#ifdef DEBUG
	#define log(format,...) printf((format),##__VA_ARGS__)
#else
	#define log(format,...) 
#endif

#ifndef bzero
#define bzero(buff,size)	memset(buff,0,size);
#endif

#define MAXLINE 1024
#define LISTNQ 1024  // just a guess
#define MAX_CONN 1024

#define SERV_PORT	9877  // the port that the server listen to
#define SERV_PORT_STR "9877"

#define max(a,b)	((a) > (b) ? (a) : (b))

#define HEART_BEAT_FREQUENCY (45 * 6)		                  // the frequency that client and server query the other side's existence.
#define HEART_BEAR_MAX_TRY   (HEART_BEAT_FREQUENCY * 3)       // the max time passed that the client and server will try to acquire the other side's existence, otherwide, it'll die!

// set linger
#define LINGER_ONOFF 1
#define LINGER_LINGER 3

typedef struct sockaddr SA;

//-----------------------NON Block IO------------------------------
void make_fd_nonblock(int fd);

void epoll_server(int listenfd, void (*callback)(int));


int g_connfd;

void err_sys(const char *fmt,...);
void err_quit(const char *fmt,...);

int tcp_connect(const char *host,const char *service);
int	udp_connect(const char *host, const char *service);

int tcp_listen(const char *host, const char *service, socklen_t *lenp);
int	udp_server(const char *host, const char *service, socklen_t *lenp);


ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);
ssize_t readline(int fd, void *ptr, size_t maxlen);


void cli_heartbeat(int sockfd, int nsec, int max_try_times);
void serv_heartbeat(int sockfd, int nsec, int max_try_times);

enum type {
	SERVER,CLIENT
};

enum transport_protocol_t {
	TCP,UDP,SCTP
};

void make_fd_nonblock(int fd);


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

// for echo test
void str_echo(int fd);

#endif
