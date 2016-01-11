#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define GROUP "225.0.0.37"
#define PORT 12345

#define BUF_SIZE 1024

int main(int argc, char **argv) {
	int		sockfd;
	struct	sockaddr_in servaddr;
	char	buff[BUF_SIZE];

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, GROUP, &servaddr.sin_addr.s_addr) < 0) {
		perror("Get address of server failed");
		exit(1);
	}

	while(fgets(buff,sizeof(buff),stdin) != NULL) {
		if (sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			perror("sendto");
			exit(1);
		}
	}


	exit(0);
}

