#include "../tools.h"

#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>

void doit(int sockfd,FILE *fp) {
	int		maxfd;
	fd_set	rset;
	int 	n;
	char	sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);

	while(1) {
		maxfd = max(sockfd,fileno(fp));
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd, &rset);

		select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd,&rset)) {
			if ( (n = readline(sockfd,recvline, sizeof(recvline))) < 0)
				err_sys("readn");
			else if (n == 0)
				return;
			if (fputs(recvline,stdout) == EOF)
				err_sys("fputs");
		} else if (FD_ISSET(fileno(fp),&rset)) {
			if (fgets(sendline,sizeof(sendline),fp) == NULL)
				return;
			printf("woman: %s\n", sendline);
			if (writen(sockfd, sendline, strlen(sendline)) < 0)
				err_sys("writen");
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
