#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <strings.h>

#define PATH "/tmp/uds.tmp"
#define MAXLINE 1024

#define CONTROL_LEN (sizeof(struct cmsghdr) + sizeof(struct cmsgcred))

ssize_t read_cred(int fd, void *ptr, size_t nbytes, struct cmsgcred *cmsgcredptr) {
	struct	msghdr msg;
	struct	iovec iov[1];
	char	control[CONTROL_LEN];
	int		n;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = control;
	msg.msg_controllen = sizeof(control);
	msg.msg_flags = 0;

	if ( (n = recvmsg(fd, &msg, 0)) < 0)
		return n;

	cmsgcredptr->cmcred_ngroups = 0; /* indicates no credentials returned */
	if (cmsgcredptr && msg.msg_controllen > 0) {
		struct cmsghdr *cmptr = (struct cmsghdr *)control;

		if (cmptr->cmsg_len < CONTROL_LEN) {
			fprintf(stderr,"control length = %d\n",cmptr->cmsg_len);
			exit(1);
		}
		if (cmptr->cmsg_level != SOL_SOCKET) {
			fprintf(stderr,"control level != SOL_SOCKET\n");
			exit(1);
		}

		if (cmptr->cmsg_type != SCM_CREDS) {
			fprintf(stderr,"control type != SCM_CREDS");
			exit(1);
		}

		memcpy(cmsgcredptr, CMSG_DATA(cmptr), sizeof(struct cmsgcred));
	}

	return n;

}

static void dg_echo(int sockfd,struct sockaddr *cliaddr, socklen_t clilen) {
	char	buff[MAXLINE];
	int		n;
	socklen_t len;
	for ( ; ; ) {
		len = clilen;
		n = recvfrom(sockfd, buff, MAXLINE, 0, cliaddr, &len);
		sendto(sockfd, buff, n, 0, cliaddr, len);
	}
}

int main(int argc, char **argv) {
	int		sockfd;
	struct	sockaddr_un servaddr,cliaddr;
	char	buff[MAXLINE];

	daemon(0,0);

	unlink(PATH);

	if ( (sockfd = socket(AF_LOCAL,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	memcpy(servaddr.sun_path,PATH,sizeof(PATH));

	if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0) {
		perror("bind");
		exit(1);
	}

	dg_echo(sockfd,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

}
