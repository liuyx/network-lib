#include "../tools.h"

void str_echo(int sockfd,struct sockaddr *cliaddr, FILE *fp);

int main() {
	int		sockfd, n;
	struct	sockaddr_in servaddr;
	struct	addrinfo hint, *res, *ressave;
	socklen_t len;

	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;


	if ( (n = getaddrinfo(NULL,SERV_PORT_STR,&hint,&res)) < 0)
		err_sys("getaddrinfo %s\n",gai_strerror(n));

	ressave = res;

	do {
		if ( (sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) < 0)
			continue;
		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		err_quit("socket error");

	freeaddrinfo(ressave);

	str_echo(sockfd,res->ai_addr, stdout);

	return 0;
}

void str_echo(int sockfd, struct sockaddr *cliaddr, FILE *fp) {
	char recvline[MAXLINE + 1];
	socklen_t len;
	int	n;

	while ( ( n = recvfrom(sockfd, recvline, MAXLINE, 0, cliaddr, &len) > 0)) {
		writen(sockfd, recvline, n);
	}

	if (n < 0)
		err_sys("recvfrom");
}
