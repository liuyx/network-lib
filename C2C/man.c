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

	int sockfd = tcp_connect(hostname, service);
	if (sockfd < 0)
		err_quit("tcp_connect error for %s. %s\n",hostname, service);

	start_communication(CLIENT, "client", "server", sockfd, stdin);

	return 0;
}
