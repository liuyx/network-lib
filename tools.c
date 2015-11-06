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
	err_doit(0,0,fmt,ap);
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
	size_t nleft = n;
	const char *ptr = buf;

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


static ssize_t read_1_char(int fd, char *ptr) {
	static ssize_t len;
	static char buff[MAXLINE];
	static char *read_ptr;

	if (len <= 0) {
again:
		if ( (len = read(fd, buff, sizeof(buff))) < 0) {
			if (errno == EINTR)
				goto again;
			return -1;
		} else if (len == 0) 
			return 0;
		read_ptr = buff;
	}
	len--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
	ssize_t n = 0, rc;
	char *ptr = vptr;
	char c;

	while (n++ < maxlen) {
		if ( (rc = read_1_char(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			*ptr = 0;
			return n - 1;
		} else
			return -1;
	}

	*ptr = 0;
	return n;
}

//--------------------im function-----------------------

static inline int read_retry_on_eintr(int fd, void *buff, size_t len) {
	int n;
again:
	if ( (n = read(fd, buff, len)) < 0) {
		if (errno == EINTR)
			goto again;
		else
			err_sys("read");
	}

	return n;
}

// 1. simple input from stdin, and write to other
void start_communication(enum type type, const char *self_name, int sockfd, FILE *input) {
	char	buff[MAXLINE];
	int		n, stdineof = 0;
	int		input_fd = fileno(input);
	size_t	maxfd = max(sockfd, input_fd);
	fd_set	rset;

	FD_ZERO(&rset);

	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(input_fd, &rset);
		FD_SET(sockfd, &rset);

		if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0)
			err_sys("select");

		if (FD_ISSET(sockfd, &rset)) {
			if ( (n = read_retry_on_eintr(sockfd, buff, MAXLINE)) == 0) {
				if (stdineof == 1) 
					return;
				else {
					if (type == SERVER)
						return;
					else
						err_sys("the opsite terminated prematurely");
				}
			} 
			if (write(fileno(stdout), buff, n) != n)
				err_sys("write");
		} else if (FD_ISSET(input_fd, &rset)) {
			if ( (n = read_retry_on_eintr(input_fd, buff, MAXLINE)) == 0) {
				stdineof = 1;
				FD_CLR(input_fd,&rset);
				if (shutdown(input_fd, SHUT_WR) < 0)
					err_sys("shutdown");
				continue;
			}

			if (writen(sockfd, buff, n) != n)
				err_quit("writen sockfd error");
		}
	}
}
