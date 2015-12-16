#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdarg.h>

#ifndef bzero
#include <string.h>
#define bzero(ptr,size) memset((ptr),0,size)
#else
#include <strings.h>
#endif

typedef struct sockaddr SA;

static int bind_socket(const char *host, const char *service);
static void make_socket_nonblock(int sockfd);
static void str_echo(int sockfd);
static void err_sys(const char *msg,...);
static void err_quit(const char *msg,...);

#define MAX_CONN 1024
#define MAXLINE 1024

int main(int argc, char **argv) {
	int		sockfd, epollfd, n, i;
	const	char *host, *service;
	struct	epoll_event event, *events;
	if (argc < 2) 
		err_quit("usage: %s <hostname> <service>",argv[0]);
	if (argc == 2) {
		host = NULL;
		service = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		service = argv[2];
	}

	daemon(0,0);

	if ( (sockfd = bind_socket(host,service)) < 0)
		err_quit("bind_socket error");

	make_socket_nonblock(sockfd);

	if ( (epollfd = epoll_create1(0)) < 0)
		err_sys("epoll_create1");

	event.data.fd = sockfd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) < 0)
		err_sys("epoll_ctl");

	if (listen(sockfd,SOMAXCONN) < 0)
		err_sys("listen");

	if ( (events = calloc(MAX_CONN, sizeof(struct epoll_event))) == NULL)
		err_sys("calloc");

	int connfd;
	struct sockaddr_storage cliaddr;
	socklen_t socklen = sizeof(cliaddr);
	char client_host[512];
	char client_port[32];

	for ( ; ; ) {
		if ( (n = epoll_wait(epollfd, events, MAX_CONN, -1)) < 0)
			err_sys("epoll_wait");
		for (i = 0; i < n; i++) {
			if ((events[i].events & EPOLLERR) ||
					(events[i].events & EPOLLHUP) ||
					(!(events[i].events & EPOLLIN))) {
				syslog(LOG_PID | LOG_USER, "%d error\n", events[i].data.fd);
				close(events[i].data.fd);
				continue;
			} else if (events[i].data.fd == sockfd) {
				for ( ; ; ) {
					if ( (connfd = accept(sockfd, (SA *)&cliaddr, &socklen)) < 0) {
						if (errno == EINTR || errno == EWOULDBLOCK)
							break;
						err_sys("accept");
					}

					make_socket_nonblock(connfd);

					if (getnameinfo((SA *)&cliaddr,socklen,client_host,sizeof(client_host),client_port,sizeof(client_port),0) == 0) {
						syslog(LOG_PID | LOG_USER, "come a connection for %s:%s",client_host,client_port);
					}
					
					event.data.fd = connfd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) < 0)
						err_sys("epoll_ctl");
					break;
				}
			} else {
				str_echo(events[i].data.fd);
			}
		}
	}

	return 0;
}

static int bind_socket(const char *host, const char *service) {
	struct		addrinfo hints, *res, *ressave;
	int			n, sockfd;

	bzero(&hints,sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;

	if ( (n = getaddrinfo(host, service, &hints, &res)) < 0)
		err_quit("getaddrinfo error for %s, %s: %s", host, service, gai_strerror(n));

	ressave = res;

	do {
		if ( (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;
		if (bind(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
		if (close(sockfd) < 0)
			err_sys("close");
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) {
		err_quit("bind_socket error");
	}

	freeaddrinfo(ressave);

	return sockfd;
}

static void make_socket_nonblock(int sockfd) {
	int		val;
	if ( (val = fcntl(sockfd, F_GETFL, 0)) < 0)
		err_sys("fcntl");

	val |= O_NONBLOCK;

	if (fcntl(sockfd, F_SETFL, val) < 0)
		err_sys("fcntl");
}

static void str_echo(int sockfd) {
	char	recvline[MAXLINE + 1];
	int		n;
	size_t	done = 0;

	while (1) {
		if ( (n = read(sockfd, recvline, sizeof(recvline))) < 0) {
			if (errno == EINTR || errno == EAGAIN)
				break;
			done = 1;
		} else if (n == 0) {
			done = 1;
			break;
		}

		if (write(sockfd, recvline, n) != n) {
			err_sys("write");
		}

		if (done) {
			syslog(LOG_PID | LOG_USER, "the client is terminated\n");
			close(sockfd);
			close(sockfd);
		}
	}
}

static void err_doit(int errorflag, int error, const char *fmt, va_list ap) {
	char buff[MAXLINE];
	vsnprintf(buff,MAXLINE,fmt,ap);
	if (errorflag) {
		size_t len = strlen(buff);
		snprintf(buff + len, MAXLINE - len, ": %s",strerror(errno));
	}
	strcat(buff,"\n");
	fflush(stdout);
	fputs(buff,stderr);
	fflush(NULL);
}

static void err_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,errno,fmt,ap);
	va_end(ap);
	exit(1);
}

static void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(0,0,fmt,ap);
	va_end(ap);
	exit(1);
}
