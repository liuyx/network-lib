#include "../tools.h"

#include <sys/un.h>

#define PATH "/tmp/unix-domain"
#define MAXLINE 1024

int main(int argc, char **argv) {
	int		sockfd;
	socklen_t len;
	struct	sockaddr_un addr1, cliaddr;

	unlink(PATH);

	bzero(&addr1, sizeof(addr1));
	addr1.sun_family = AF_LOCAL;
	memcpy(addr1.sun_path,PATH, sizeof(PATH));

	if ( (sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bind(sockfd, (struct sockaddr *)&addr1, SUN_LEN(&addr1));

	epoll_server(sockfd,str_echo);

	//listen(sockfd, SOMAXCONN);
	//int		connfd;

	//for ( ; ; ) {
	//	len = sizeof(cliaddr);

	//	if ( (connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
	//		if (errno == EINTR)
	//			continue;
	//		else {
	//			perror("accept");
	//			exit(1);
	//		}
	//	}

	//	char	recvline[MAXLINE];
	//	int		n;
	//	struct	sockaddr_un cliaddr;
	//	while (1) {
	//		if ( (n = read(connfd, recvline, MAXLINE)) > 0) {
	//			recvline[n] = 0;
	//			write(connfd, recvline, n);
	//		}

	//		if (n < 0) {
	//			perror("read");
	//			exit(1);
	//		}
	//	}
	//}

	return 0;
}
