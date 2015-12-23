#include "../tools.h"

#define UNIXDG_PATH "/tmp/domain"
#define MAXLINE 1024

//void str_cli(int sockfd, FILE *fp) {
//	char	recvline[1024], sendline[1024];
//	size_t	len;
//	int		n;
//
//	while (fgets(sendline,sizeof(sendline),fp) != NULL) {
//		len = strlen(sendline);
//
//		if (write(sockfd, sendline, len) < 0) {
//			perror("write");
//			exit(1);
//		}
//
//		if ( (n = read(sockfd, recvline, 1024)) > 0) {
//			recvline[n] = 0;
//			fputs(recvline, stdout);
//		}
//	}
//}

//static ssize_t writen(int fd, void *buf, size_t n) {
//	ssize_t nwrite;
//	size_t nleft = n;
//	const char *ptr = buf;
//
//	while (nleft > 0) {
//		if ( (nwrite = write(fd, ptr, nleft)) <= 0) {
//			if (nwrite < 0 && errno == EINTR)
//				nwrite = 0;
//			else
//				return -1;
//		}
//		nleft -= nwrite;
//		ptr += nwrite;
//	}
//
//	return n;
//}

//void str_cli(int sockfd, FILE *fp) {
//	int		kq, n, nchanges, isfile, i, stdineof = 0;
//	char	buf[MAXLINE];
//	struct	kevent keys[2];
//	struct	stat st;
//	struct	timespec ts;
//
//	isfile = ((fstat(fileno(fp),&st) == 0) &&
//			  (st.st_mode & S_IFMT) == S_IFREG);
//
//	if ( (kq = kqueue()) < 0) {
//		perror("kqueue");
//		exit(1);
//	}
//
//	EV_SET(&keys[0],sockfd,EVFILT_READ,EV_ADD,0,0,NULL);
//	EV_SET(&keys[1],fileno(fp),EVFILT_READ,EV_ADD,0,0,NULL);
//
//	ts.tv_nsec = ts.tv_sec = 0;
//
//	if (kevent(kq,keys,2,NULL,0,&ts) < 0) {
//		perror("kevent");
//		exit(1);
//	}
//
//	for ( ; ; ) {
//		nchanges = kevent(kq,NULL,0,keys,2,&ts);
//
//		for (i = 0; i < nchanges; i++) {
//			if (keys[i].ident == sockfd) {
//				if ( (n = read(sockfd,buf,sizeof(buf))) == 0) {
//					if (stdineof == 1)
//						break;
//					else {
//						fprintf(stderr,"server terminated prematurely\n");
//						exit(1);
//					}
//				} else if (n < 0) {
//					perror("read");
//					exit(1);
//				} else {
//					if (write(fileno(stdout),buf,n) != n) {
//						perror("write");
//						exit(1);
//					}
//				}
//
//			} else if (keys[i].ident == fileno(fp)) {
//				n = read(fileno(fp),buf,sizeof(buf));
//
//				if (n == 0 || (isfile && keys[i].data == n)) {
//					stdineof = 1;
//					if (shutdown(sockfd,SHUT_WR) < 0) {
//						perror("shutdown");
//						exit(1);
//					}
//
//					keys[i].flags = EV_DELETE;
//					if (kevent(kq,&keys[i],1,NULL,0,&ts) < 0) {
//						perror("kevent");
//						exit(1);
//					}
//
//					continue;
//					
//				} else if (n < 0) {
//					perror("read");
//					exit(1);
//				} else {
//					if (writen(sockfd,buf,n) != n) {
//						fprintf(stderr,"writen");
//						exit(1);
//					}
//				}
//			}
//		}
//	}
//}

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
