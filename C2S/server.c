#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <strings.h>

#define UNIXDG_PATH "/tmp/domain"

static void str_echo(int sockfd) {
	char recvline[1024];
	int	 n;

	while ( (n = read(sockfd, recvline, sizeof(recvline))) > 0) {
		recvline[n] = 0;
		if (write(sockfd, recvline, n) != n) {
			perror("write");
			exit(1);
		}
	}

	if (n < 0) {
		perror("read");
		exit(1);
	}

}

void *run(void *arg) {
	pthread_detach(pthread_self());
	int connfd = arg;
	printf("come a connect\n");
	str_echo(connfd);

	return NULL;
}

int main(int argc, char **argv) {
	int		sockfd, connfd;
	struct	sockaddr_un servaddr, cliaddr;
	socklen_t len;

	pthread_t tid;

	if ( (sockfd = socket(AF_LOCAL,SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	unlink(UNIXDG_PATH);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	memcpy(servaddr.sun_path,UNIXDG_PATH,sizeof(UNIXDG_PATH));

	bind(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr));

	listen(sockfd, 1024);

	len = sizeof(cliaddr);

	for ( ; ; ) {
		if ( (connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
			perror("connfd");
			exit(1);
		}

		if (pthread_create(&tid,NULL,run,(void *)connfd) < 0) {
			perror("pthread_create");
			exit(1);
		}
	}

	return 0;
}
