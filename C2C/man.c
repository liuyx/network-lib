#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#include "../tools.h"

//void doit(int sockfd, FILE *fp) {
//	char	buff[MAXLINE];
//	int		maxfd, n, stdineof;
//	fd_set  rset;
//
//	stdineof = 0;
//	FD_ZERO(&rset);
//
//	for ( ; ; ) {
//		if (stdineof == 0)
//			FD_SET(fileno(fp), &rset);
//		FD_SET(sockfd, &rset);
//		maxfd = max(sockfd, fileno(fp));
//
//		if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0)
//			err_sys("select");
//
//		if (FD_ISSET(sockfd, &rset)) {
//read_sock_again:
//			if ( (n = read(sockfd, buff, MAXLINE)) < 0) {
//				if (errno == EINTR)
//					goto read_sock_again;
//				else
//					err_sys("read");
//			} else if (n == 0) {
//				if (stdineof == 1)
//					return;
//				else
//					err_sys("server be killed");
//			}
//			if (write(fileno(stdout),buff, n) != n)
//				err_sys("write");
//		} else if (FD_ISSET(fileno(fp), &rset)) {
//read_fp_again:
//			if ( (n = read(fileno(fp), buff, MAXLINE)) < 0) {
//				if (errno == EINTR)
//					goto read_fp_again;
//				else
//					err_sys("read fp");
//			} else if (n == 0) {
//				stdineof = 1;
//				FD_CLR(fileno(fp),&rset);
//				if (shutdown(sockfd, SHUT_WR) < 0)
//					err_sys("shutdown");
//				continue;
//			}
//
//			if (writen(sockfd, buff, n) != n) 
//				err_sys("writen sockfd");
//		}
//	}
//}

int main(int argc, char **argv) {
	if (argc != 3)
		err_sys("usage: client <hostname> <service/port#>");

	const char *hostname = argv[1];
	const char *service  = argv[2];

	int sockfd = tcp_connect(hostname, service);
	if (sockfd < 0)
		err_quit("tcp_connect error for %s. %s\n",hostname, service);

	//doit(sockfd,stdin);

	start_communication(CLIENT, "client", sockfd, stdin);

	return 0;
}
