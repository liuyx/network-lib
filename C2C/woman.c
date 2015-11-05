#include "../tools.h"

#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>

void doit(int sockfd,FILE *fp) {
	int		maxfd, stdineof, n;
	char	buff[MAXLINE];
	fd_set	rset;

	stdineof = 0;
	FD_ZERO(&rset);

	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd, &rset);
		maxfd = max(sockfd, fileno(fp));

		if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0)
			err_sys("select");

		if (FD_ISSET(sockfd,&rset)) {
read_sock_again:
			if ( (n = read(sockfd, buff, MAXLINE)) < 0) {
				if (errno == EINTR)
					goto read_sock_again;
				else
					err_sys("read");
			} else if (n == 0) 
				return;

			if (write(fileno(fp),buff, n) < 0)
				err_sys("write");

		} else if (FD_ISSET(fileno(fp),&rset)) {
read_fp_again:
			if ( (n = read(fileno(fp), buff, MAXLINE)) < 0) {
				if (errno == EINTR)
					goto read_fp_again;
				err_sys("read");
			} else if (n == 0) {
				stdineof = 1;
				if (shutdown(sockfd, SHUT_WR) < 0)
					err_sys("shutdown");
				FD_CLR(fileno(fp),&rset);
				continue;
			}

			if (writen(sockfd, buff, n) != n) 
				err_quit("writen error");
		}
	}
}


int main(int argc,char **argv){
	int sockfd, connfd;
	char buff[MAXLINE];
	struct sockaddr_in servaddr;
	socklen_t socklen;
	int on = 1;

	//if (daemon(0,0) < 0)
	//	err_sys("daemonize");

	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		err_sys("socket");


	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9877);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) < 0)
		err_sys("setsockopt");

	if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
		err_sys("bind");

	if (listen(sockfd, LISTNQ) < 0)
		err_sys("listen");

	socklen = sizeof(servaddr);

	for ( ; ; ) {
		if ( (connfd = accept(sockfd, (struct sockaddr *)&servaddr,&socklen)) < 0){
			if (errno == EINTR)
				continue;
			else
				err_sys("accept");
		}

		if (getpeername(connfd, (struct sockaddr *)&servaddr, &socklen) < 0)
			err_sys("getpeername");

		printf("connect from %s: %d\n",inet_ntop(AF_INET,&servaddr.sin_addr,buff,sizeof(buff)), ntohs(servaddr.sin_port));


		doit(connfd,stdin);

		printf("close\n");

		close(connfd);
	}
	exit(0);
}
