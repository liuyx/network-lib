#include "../tools.h"

int main(int argc, char **argv) {
	const	char *host, *service;
	if (argc < 2)
		err_quit("usage: %s <hostname> <service>",argv[0]);
	else if (argc == 2) {
		host = NULL;
		service = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		service = argv[2];
	}

	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	int	n;

	if ( (n = getaddrinfo(host, service, &hints, &res)) < 0)
		err_sys("getaddrinfo error for %s, %s: %s",host, service, gai_strerror(n));

	ressave = res;
	int		sockfd;

	const	int on = 1;

	do {
		if ( (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			err_sys("setsockopt");

		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(sockfd);

	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		err_sys("getaddrinfo error for %s, %s", host, service);

	freeaddrinfo(ressave);

	char	recvline[MAXLINE + 1];
	struct	sockaddr_storage cliaddr;
	socklen_t socklen = sizeof(cliaddr);
	while (1) {
		if ( (n = recvfrom(sockfd, recvline, sizeof(recvline), 0, (struct sockaddr *)&cliaddr, &socklen)) < 0)
			err_sys("recvfrom");

		recvline[n] = 0;

		if (sendto(sockfd, recvline, n, 0, (struct sockaddr *)&cliaddr, socklen) != n)
			err_sys("sendto");
	}


	exit(0);
}
