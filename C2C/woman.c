#include "../tools.h"

#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>

void *run(void *arg) {
	int sockfd = *((int *)arg);

	char buff[MAXLINE + 1];
	int n;
	size_t maxfd;
	fd_set rset;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	FD_SET(STDIN_FILENO,&rset);

	maxfd = max(sockfd, STDIN_FILENO);

	select(maxfd + 1, &rset, NULL, NULL, NULL);

	if (FD_ISSET(sockfd, &rset)) {
		while ((fgets(buff,sizeof(buff), stdin)) != NULL) {
			if (writen(sockfd,buff,strlen(buff)) < 0)
				err_quit("writen error");
		}
	} else if (FD_ISSET(STDIN_FILENO, &rset)) {
		while ( (n = readline(sockfd,buff,sizeof(buff))) > 0) {
			buff[n] = 0;
			if (fputs(buff,stdout) == EOF)
				err_sys("fputs");
		}
	}

	return NULL;
}

int main(int argc,char **argv){
	int sockfd, connfd;
	char buff[MAXLINE];
	struct sockaddr_in servaddr;
	socklen_t socklen;
	time_t ticks;
	int on = 1;
	pthread_t tid;

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
		if ( (connfd = accept(sockfd, (struct sockaddr *)&servaddr,&socklen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("accept");
		}

		printf("connect from %s: %d\n",inet_ntop(AF_INET,&servaddr.sin_addr,buff,sizeof(buff)), ntohs(servaddr.sin_port));

		pthread_create(&tid, NULL, run, &connfd); 
		close(connfd);
	}
	exit(0);
}
