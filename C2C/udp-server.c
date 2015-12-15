#include "../tools.h"

int main(int argc, char **argv) {

	int		sockfd;
	socklen_t len;
	const	char *service;

	if (argc == 2)
		service = argv[1];
	else
		service = SERV_PORT_STR;

	sockfd = udp_server(NULL, service, &len);
	start_communication(SERVER,"server","client",sockfd, stdin);
	return 0;
}
