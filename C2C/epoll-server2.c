#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

static void make_socket_nonblock(int fd) {
	int		val;
	if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl");

	val |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl");
}

int epoll_socket(int listenfd,void (*callback)(int)) {
	int		epollfd, n, i;
	struct	epoll_event event, *events;

	make_fd_nonblock(listenfd);

	if ( (epollfd = epoll_create1(0)) < 0)
		err_sys("epoll_create1");

	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) < 0)
		err_sys("epoll_ctl");

	if (listen(listenfd,SOMAXCONN) < 0)
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
				syslog(LOG_PID | LOG_USER, "listenfd = %d error\n",events[i].data.fd);
				close(events[i].data.fd);
				continue;
			} else if (events[i].data.fd == listenfd) {
				for ( ; ; ) {
					if ( (connfd = accept(listenfd, (SA *)&cliaddr, &socklen)) < 0) {
						if (errno == EINTR || errno == EWOULDBLOCK)
							break;
						err_sys("accept");
					}

					make_fd_nonblock(connfd);

					if (getnameinfo((SA *)&cliaddr, socklen, client_host, sizeof(client_host), client_port, sizeof(client_port), 0) == 0) {
						syslog(LOG_PID | LOG_USER, "come a connection for %s:%s",client_host,client_port);
					}

					event.data.fd = connfd;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &events) < 0)
						err_sys("epoll_ctl");

					break;
				}
			} else {
				callback(events[i].data.fd);
			}
		}
	}
}
