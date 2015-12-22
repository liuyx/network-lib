#include "../tools.h"

int main(int argc, char **argv) {
	int		sockfd;
	const	char *host, *service;
	if (argc < 2) 
		err_quit("usage: %s <hostname> <service>",argv[0]);
	if (argc == 2) {
		host = NULL;
		service = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		service = argv[2];
	}

	daemon(0,0);

	if ( (sockfd = tcp_listen(host,service,NULL)) < 0)
		err_quit("bind_socket error");


	epoll_server(sockfd,str_echo);

	return 0;
}

