#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define GROUP "225.0.0.37"
#define PORT 12345
#define BUF_SIZE 1024

int main(int argc, char **argv) {
	int		sockfd, n;
	const	int on = 1;
	struct	sockaddr_in servaddr;
	struct	ip_mreq mreq;
	char	buff[BUF_SIZE];
	socklen_t addrlen;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(1);
	}

#ifdef MCAST_JOIN_GROUP
	struct group_req greq;
	struct sockaddr_in group;
	greq.gr_interface = if_nametoindex("eth0");
	if (inet_pton(AF_INET, GROUP, &group.sin_addr.s_addr) < 0) {
		perror("inet_pton");
		exit(1);
	}
	memcpy(&greq.gr_group,(struct sockaddr *)&group,sizeof(group));

	if (setsockopt(sockfd, IPPROTO_IP, MCAST_JOIN_GROUP, &greq, sizeof(greq)) < 0) {
		perror("setsockopt error");
		exit(1);
	}
#else
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (inet_pton(AF_INET, GROUP, &mreq.imr_multiaddr.s_addr) < 0) {
		perror("inet_pton");
		exit(1);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}
#endif

	addrlen = sizeof(servaddr);

	while (1) {
		if ( (n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		buff[n] = 0;
		fputs(buff, stdout);
	}

	exit(0);
}

