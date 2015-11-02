#include "tools.h"

void err_doit(int errorflag, int error, const char *fmt, va_list ap) {
	char buff[MAXLINE];
	vsnprintf(buff,MAXLINE,fmt,ap);
	if (errorflag) {
		size_t len = strlen(buff);
		snprintf(buff + len, MAXLINE - len, ": %s",strerror(errno));
	}
	strcat(buff,"\n");
	fflush(stdout);
	fputs(buff,stderr);
	fflush(NULL);
}

void err_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,errno,fmt,ap);
	va_end(ap);
	exit(1);
}

void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,errno,fmt,ap);
	va_end(ap);
	exit(1);
}

int tcp_connect(const char *host,const char *service) {
	int sockfd;
	struct addrinfo hint, *res, *ressave;

	bzero(&hint,sizeof(struct addrinfo));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	
	if (getaddrinfo(host,service,&hint,&res) < 0)
		err_sys("getaddrinfo");

	ressave = res;

	do {
		if ( (sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) < 0)
			continue;
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
			break;
	} while ( (res = res->ai_next) != NULL);

	freeaddrinfo(ressave);

	if (res == NULL) 
		err_quit("tcp_connect can't connect");

	return sockfd;
}

ssize_t readn(int fd, void *buf, size_t n) {
	size_t nleft = n;
	char *ptr = buf;
	ssize_t nread;

	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0) 
			break;

		nleft -= nread;
		ptr += nread;
	}

	return n - nleft;
}

ssize_t writen(int fd, void *buf, size_t n) {
	ssize_t nwrite;
	size_t nleft;
	char *ptr = buf;

	while (nleft > 0) {
		if ( (nwrite = write(fd, ptr, nleft)) <= 0) {
			if (nwrite < 0 && errno == EINTR)
				nwrite = 0;
			else
				return -1;
		}
		nleft -= nwrite;
		ptr += nwrite;
	}

	return n;
}
