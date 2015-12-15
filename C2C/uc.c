#include "../tools.h"

void str_cli(int sockfd, FILE* in);

int main(int argc, char **argv) {
	if (argc != 2) 
		err_quit("usage: %s <hostname>",argv[0]);
	int		sockfd, n;
	struct	addrinfo hint, *res, *ressave;

	bzero(&hint, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_family = AF_UNSPEC;

	if ( (n = getaddrinfo(argv[1], SERV_PORT_STR, &hint, &res)) < 0)
		err_sys("getaddrinfo %s", gai_strerror(n));

	ressave = res;

	do {
		if ( (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;
		if (connect(res->ai_socktype, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(sockfd);
	}while ( (res = res->ai_next) != NULL);
	
	if (res == NULL)
		err_quit("res error");

	freeaddrinfo(ressave);


	str_cli(sockfd,stdout);

	return 0;
}

void str_cli(int sockfd, FILE *fp) {
	char	recvline[MAXLINE + 1];
	while (fgets(recvline,MAXLINE,fp) != NULL) {
		writen(sockfd,recvline,strlen(recvline));
	}
}
