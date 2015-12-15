#include "../tools.h"

void str_cli(int sockfd, FILE* in);

int main(int argc, char **argv) {
	const	char *host, *service;
	if (argc < 2) 
		err_quit("usage: %s <hostname>",argv[0]);
	else if (argc == 2) {
		host = NULL;
		service = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		service = argv[2];
	}
	int		sockfd, n;
	struct	addrinfo hint, *res, *ressave;

	bzero(&hint, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_family = AF_UNSPEC;

	if ( (n = getaddrinfo(host, service, &hint, &res)) < 0)
		err_sys("getaddrinfo %s", gai_strerror(n));

	ressave = res;

	do {
		if ( (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(sockfd);
	}while ( (res = res->ai_next) != NULL);
	
	if (res == NULL)
		err_sys("udp_connect error for %s, %s", host, service);

	freeaddrinfo(ressave);


	str_cli(sockfd,stdin);

	return 0;
}

void str_cli(int sockfd, FILE *fp) {
	char	recvline[MAXLINE + 1], sendline[MAXLINE];
	int		n;

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		write(sockfd, sendline, strlen(sendline));

		n = read(sockfd, recvline, MAXLINE);
		recvline[n] = 0;

		fputs(recvline, stdout);
	}
}
