#include "../tools.h"

void cli_doit(int sockfd) {
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
			printf("gets %s\n", buff);
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
}

int main(int argc,char **argv){
	if (argc != 3) 
		err_quit("usage: man <hostname> <service#port>");

	int sockfd;
	char *host;
	char *service;

	host = argv[1];
	service = argv[2];

	if ( (sockfd = tcp_connect(host,service)) < 0)
			err_quit("tcp_connect error");
	
	cli_doit(sockfd);

	exit(0);
}
