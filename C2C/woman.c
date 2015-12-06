#include "../tools.h"

#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>


int main(int argc,char **argv){
	//int sockfd, connfd;
	//char buff[MAXLINE];
	//struct sockaddr_in servaddr;
	//socklen_t socklen;
	//const int on = 1;

	////if (daemon(0,0) < 0)
	////	err_sys("daemonize");

	//if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	//	err_sys("socket");


	//bzero(&servaddr,sizeof(struct sockaddr_in));
	//servaddr.sin_family = AF_INET;
	//servaddr.sin_port = htons(SERV_PORT);
	//servaddr.sin_addr.s_addr = INADDR_ANY;

	//if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) < 0)
	//	err_sys("setsockopt SO_REUSEADDR");

	//if (setsockopt(sockfd,SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0)
	//	err_sys("setsockopt SO_KEEPALIVE");

	//struct linger linger;
	//linger.l_onoff = LINGER_ONOFF;
	//linger.l_linger = LINGER_LINGER;
	//if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) < 0)
	//	err_sys("setsockopt SO_LINGER");

	//if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
	//	err_sys("bind");

	//if (listen(sockfd, LISTNQ) < 0)
	//	err_sys("listen");

	//socklen = sizeof(servaddr);

	int sockfd, connfd;
	socklen_t socklen;
	struct sockaddr_storage servaddr;

	sockfd = tcp_listen(NULL,SERV_PORT_STR,&socklen);

	for ( ; ; ) {
		if ( (connfd = accept(sockfd, (struct sockaddr *)&servaddr,&socklen)) < 0){
			if (errno == EINTR)
				continue;
			else
				err_sys("accept");
		}

		g_connfd = connfd;

		if (getpeername(connfd, (struct sockaddr *)&servaddr, &socklen) < 0)
			err_sys("getpeername");

		//log("connect from %s: %d\n",inet_ntop(AF_INET,&servaddr.sin_addr,buff,sizeof(buff)), ntohs(servaddr.sin_port));

		start_communication(SERVER, "server", "client", connfd, stdin);

		log("the other side is closed\n");

		g_connfd = -1;

		close(connfd);
	}
	exit(0);
}
