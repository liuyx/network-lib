#include "../tools.h"

#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>


int main(int argc,char **argv){
	int sockfd, connfd;
	char buff[MAXLINE];
	struct sockaddr_in servaddr;
	socklen_t socklen;
	const int on = 1;

	//if (daemon(0,0) < 0)
	//	err_sys("daemonize");

	if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		err_sys("socket");


	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9877);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) < 0)
		err_sys("setsockopt SO_REUSEADDR");

	if (setsockopt(sockfd,SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0)
		err_sys("setsockopt SO_KEEPALIVE");

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

		start_communication(SERVER, "server", "client", connfd, stdin);

		printf("the other side is closed\n");

		close(connfd);
	}
	exit(0);
}
