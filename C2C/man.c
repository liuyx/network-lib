#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#include "../tools.h"

int main(int argc, char **argv) {
	if (argc != 3)
		err_sys("usage: client <hostname> <service/port#>");

	const char *hostname = argv[1];
	const char *service  = argv[2];
	char recvline[MAXLINE + 1];
	int n;
	size_t maxfd;
	fd_set rset;


	int sockfd = tcp_connect(hostname, service);
	if (sockfd < 0)
		err_quit("tcp_connect error for %s. %s\n",hostname, service);

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	FD_SET(fileno(stdin),&rset);

	maxfd = max(sockfd,fileno(stdin));

	for ( ; ; ) {
		select(maxfd + 1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(sockfd,&rset)) {
			n = readn(sockfd, recvline, MAXLINE);
			if (n < 0)
				err_sys("read error");

			recvline[n] = 0;
			if (fputs(recvline,stdout) == EOF)
				err_sys("fputs");

			if (n < 0)
				err_sys("read error");
		} else if (FD_ISSET(STDIN_FILENO,&rset)) {
			fgets(recvline,MAXLINE,stdin);
			if (writen(sockfd, recvline, strlen(recvline)) < 0)
					err_sys("writen error");
		}
	}


	return 0;
}
