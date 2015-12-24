#include "../tools.h"

#define UNIXDG_PATH "/tmp/domain"

int main(int argc, char **argv) {
	int		sockfd;
	struct	sockaddr_un cliaddr;

	if ( (sockfd = socket(AF_LOCAL,SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	memcpy(cliaddr.sun_path,UNIXDG_PATH,sizeof(UNIXDG_PATH));

	if (connect(sockfd, (struct sockaddr *)&cliaddr,sizeof(cliaddr)) < 0) {
		perror("connect");
		exit(1);
	}

	str_cli(sockfd,stdin);

	return 0;


}
