#include "../tools.h"

#include <time.h>

int main(int argc,char **argv){
	int sockfd, connfd;
	char buff[MAXLINE];
	struct sockaddr_in servaddr;
	socklen_t socklen;
	time_t ticks;
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
		if ( (connfd = accept(sockfd, (struct sockaddr *)&servaddr,&socklen)) < 0)
			err_sys("accept");
		ticks = time(NULL);
		snprintf(buff,MAXLINE,"%.24s\r\n",ctime(&ticks));

		if (write(connfd, buff, strlen(buff)) != strlen(buff))
			err_sys("write");
		close(connfd);
	}
	exit(0);
}
