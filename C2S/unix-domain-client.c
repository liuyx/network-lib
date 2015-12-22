#include "../tools.h"
#include <sys/un.h>
#define MAXLINE 1024

#define PATH "/tmp/unix-domain"
int main(int argc, char **argv) {
	int		sockfd;
	socklen_t len;

	struct sockaddr_un cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	memcpy(cliaddr.sun_path,PATH, sizeof(PATH));

	if ( (sockfd = socket(AF_LOCAL,SOCK_STREAM,0)) < 0) {
		perror("socket");
		exit(1);
	}

	bind(sockfd,(SA *)&cliaddr,sizeof(cliaddr));

	len = sizeof(cliaddr);
	connect(sockfd, (struct sockaddr *)&cliaddr,len);


	//start_communication(CLIENT,"client","server",sockfd,stdin);


	int 	n;
	char	sendline[MAXLINE], recvline[MAXLINE];
		
	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		printf("write %s\n",sendline);
		write(sockfd, sendline, strlen(sendline));

		n = read(sockfd, recvline, MAXLINE);
		printf("recive %s\n",recvline);
		fputs(recvline, stdout);
	}

	exit(0);
}
