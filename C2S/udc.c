#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <strings.h>

#define PATH "/tmp/uds.tmp"
#define MAXLINE 1024

static void dg_cli(int sockfd, struct sockaddr *pservaddr, socklen_t servlen) {
	int		n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen);
		n = recvfrom(sockfd,recvline,MAXLINE,0,NULL,NULL);
		recvline[n] = 0;
		fputs(recvline,stdout);
	}
}

int main() {
	int		sockfd;
	struct  sockaddr_un cliaddr, servaddr;

	if ( (sockfd = socket(AF_LOCAL,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	strcpy(cliaddr.sun_path,tmpnam(NULL));
	if (bind(sockfd,(struct sockaddr *)&cliaddr,sizeof(cliaddr)) < 0) {
		perror("bind");
		exit(1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	memcpy(servaddr.sun_path,PATH,sizeof(PATH));
	//if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0) {
	//	perror("bind!");
	//	exit(1);
	//}

	dg_cli(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	exit(0);


}
