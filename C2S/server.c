#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <strings.h>
#include <sys/uio.h>

#include <syslog.h>

#include <errno.h>
#define UNIXDG_PATH "/tmp/domain"

//#define CONTROL_LEN (sizeof(struct cmsghdr) + sizeof(struct cmsgcred))

//ssize_t read_cred(int fd, void *ptr, size_t nbytes, struct cmsgcred *cmsgcredptr) {
//	struct	msghdr msg;
//	struct	iovec iov[1];
//	char	control[CONTROL_LEN];
//	int		n;
//
//	msg.msg_name = NULL;
//	msg.msg_namelen = 0;
//	iov[0].iov_base = ptr;
//	iov[0].iov_len = nbytes;
//	msg.msg_iov = iov;
//	msg.msg_iovlen = 1;
//	msg.msg_control = control;
//	msg.msg_controllen = sizeof(control);
//	msg.msg_flags = 0;
//
//	if ( (n = recvmsg(fd, &msg, 0)) < 0)
//		return n;
//
//	cmsgcredptr->cmcred_ngroups = 0; /* indicates no credentials returned */
//	if (cmsgcredptr && msg.msg_controllen > 0) {
//		struct cmsghdr *cmptr = (struct cmsghdr *)control;
//
//		if (cmptr->cmsg_len < CONTROL_LEN) {
//			fprintf(stderr,"control length = %d\n",cmptr->cmsg_len);
//			exit(1);
//		}
//		if (cmptr->cmsg_level != SOL_SOCKET) {
//			fprintf(stderr,"control level != SOL_SOCKET\n");
//			exit(1);
//		}
//
//		if (cmptr->cmsg_type != SCM_CREDS) {
//			fprintf(stderr,"control type != SCM_CREDS");
//			exit(1);
//		}
//
//		memcpy(cmsgcredptr, CMSG_DATA(cmptr), sizeof(struct cmsgcred));
//	}
//
//	return n;
//
//}

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

//static void str_echo(int sockfd) {
//	ssize_t		n;
//	int			i;
//	char		buf[1024];
//	struct		cmsgcred cred;
//
//again:
//	while ( (n = read_cred(sockfd, buf, sizeof(buf), &cred)) > 0) {
//		if (cred.cmcred_ngroups == 0) {
//			printf("no credentials returned\n");
//		} else {
//			printf("PID of sender = %d\n", cred.cmcred_pid);
//			printf("real user ID = %d\n",cred.cmcred_uid);
//			printf("real group ID = %d\n",cred.cmcred_gid);
//			printf("effective user ID = %d\n",cred.cmcred_euid);
//			for (i = 1; i < cred.cmcred_ngroups; i++)
//				printf(" %d",cred.cmcred_groups[i]);
//			printf("\n");
//		}
//		write(sockfd,buf,n);
//	}
//
//	if (n < 0 && errno == EINTR)
//		goto again;
//	else if (n < 0) {
//		perror("read");
//		exit(1);
//	}
//}
//

void *run(void *arg) {
	pthread_detach(pthread_self());
	int connfd = arg;
	syslog(LOG_NOTICE,"common a connection\n");
	str_echo(connfd);

	return NULL;
}

int main(int argc, char **argv) {
	int		sockfd, connfd;
	struct	sockaddr_un servaddr, cliaddr;
	socklen_t len;

	pthread_t tid;

	daemon(0,0);

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
