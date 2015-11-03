#include "../tools.h"

#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>


int main(int argc,char **argv){
	int sockfd, connfd;
	char buff[MAXLINE];
	struct sockaddr_in servaddr;
	socklen_t socklen;
	time_t ticks;
	int on = 1;
	size_t maxfd;
	fd_set rset;
	ssize_t n;

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

	maxfd = max(sockfd,STDIN_FILENO);
	FD_ZERO(&rset);

	for ( ; ; ) {
		if ( (connfd = accept(sockfd, (struct sockaddr *)&servaddr,&socklen)) < 0)
			err_sys("accept");

		if (getpeername(connfd, (struct sockaddr *)&servaddr, &socklen) < 0)
			err_sys("getpeername");

		printf("connect from %s: %d\n",inet_ntop(AF_INET,&servaddr.sin_addr,buff,sizeof(buff)), ntohs(servaddr.sin_port));

		maxfd = max(connfd,STDIN_FILENO);
		FD_SET(STDIN_FILENO,&rset);
		FD_SET(connfd, &rset);

		select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd,&rset)) {
			if ( (n = readn(sockfd,buff, sizeof(buff))) < 0)
				err_sys("readn");
			buff[n] = 0;
			if (fputs(buff,stdout) == EOF)
				err_sys("fputs");
		} else if (FD_ISSET(STDIN_FILENO,&rset)) {
			fgets(buff,sizeof(buff),stdin);
			if (writen(sockfd, buff, strlen(buff)) < 0)
				err_sys("writen");
		}

		printf("close\n");

		close(connfd);
	}
	exit(0);
}
