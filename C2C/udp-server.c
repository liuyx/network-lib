#include "../tools.h"

int main(int argc, char **argv) {
	if (argc != 2) 
		err_quit("usage: %s <hostname> <service>", argv[0]);

	int		sockfd;
	socklen_t len;
	const	char *service = argv[0];

	sockfd = udp_server(NULL, service, &len);
	start_communication(SERVER,"server","client",sockfd, stdin);
	return 0;
}
