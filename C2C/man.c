#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#include "../tools.h"

void doit(int sockfd, FILE *fp) {
	char recvline[MAXLINE];
	char sendline[MAXLINE];
	int n;
	size_t maxfd;
	fd_set rset;

	FD_ZERO(&rset);

	for ( ; ; ) {
		FD_SET(sockfd, &rset);
		FD_SET(fileno(fp),&rset);
		maxfd = max(sockfd,fileno(fp));
		select(maxfd + 1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(sockfd,&rset)) {
			if ( (n = readline(sockfd, recvline, MAXLINE)) > 0) {
				if (fputs(recvline,stdout) == EOF)
					err_sys("fputs");
			}
			if (n < 0)
				err_sys("read error");
			else if (n == 0)
				err_quit("server terminated error");

		} else if (FD_ISSET(fileno(fp),&rset)) {
			fgets(sendline,MAXLINE,fp);
			printf("man: %s\n", sendline);
			if (writen(sockfd, sendline, strlen(sendline)) < 0)
					err_sys("writen error");
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 3)
		err_sys("usage: client <hostname> <service/port#>");

	const char *hostname = argv[1];
	const char *service  = argv[2];

	int sockfd = tcp_connect(hostname, service);
	if (sockfd < 0)
		err_quit("tcp_connect error for %s. %s\n",hostname, service);

	doit(sockfd,stdin);

	return 0;
}
