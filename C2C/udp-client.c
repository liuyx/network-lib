#include "../tools.h"
#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
	if (argc != 3) 
		err_quit("usage: %s <hostname> <#port>",argv[0]);

	const char *host = argv[1];
	const char *service = argv[2];
	int	  sockfd = udp_connect(host,service);

	start_communication(CLIENT,"client","server",sockfd, stdin);

	return 0;

}
