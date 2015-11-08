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
void start_communication(enum type type, const char *self_name,const char *other_name, int sockfd, FILE *input) {
	char	buff[MAXLINE];
	char	tmp[MAXLINE];
	int		n, maxfd, stdineof;
	fd_set	rset;
	int		input_fd;
	int		self_name_len, other_name_len;

	input_fd = fileno(input);
	stdineof = 0;
	FD_ZERO(&rset);

	if (type == SERVER)
		serv_heartbeat(sockfd,HEART_BEAT_FREQUENCY,HEART_BEAR_MAX_TRY);
	else
		cli_heartbeat(sockfd,HEART_BEAT_FREQUENCY,HEART_BEAR_MAX_TRY);

	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(input_fd,&rset);
		FD_SET(sockfd, &rset);
		maxfd = max(input_fd, sockfd);

re_select:
		if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0){
			if (errno == EINTR)
				goto re_select;
			else
				err_sys("select");
		}

		if (FD_ISSET(sockfd, &rset)) {
			if ( (n = read_retry_on_eintr(sockfd, buff, MAXLINE)) == 0) {
				if (stdineof == 1) {
					return;
				} else {
					if (type == SERVER)
						return;
					err_quit("the other side terminated prematurely");
				}
			}

			other_name_len = strlen(self_name);

			bzero(&tmp,sizeof(tmp));
			snprintf(tmp,7 + other_name_len,"\t\t\t\t\t%s: ", other_name);
			memcpy(tmp + 7 + other_name_len, buff, n);

			if (write(fileno(stdout),tmp,n + 7 + other_name_len) != (n + 7 + other_name_len))
				err_sys("write");
		} else if (FD_ISSET(input_fd,&rset)) {
			if ( (n = read_retry_on_eintr(input_fd, buff, MAXLINE)) == 0) {
				stdineof = 1;
				FD_CLR(input_fd,&rset);
				if (shutdown(sockfd, SHUT_WR) < 0)
					err_sys("shutdown");
				continue;
			}

			if (writen(sockfd, buff, n) != n)
				err_quit("writen error for sockfd");
		}
	}
}

//-----------------------------signal-------------------------------
sig_func *my_signal(int signo,sig_func *func) {
	struct sigaction act, oact;
	act.sa_flags = 0;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo,&act,&oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}


//-------------------------------heartbeat---------------------------------

static void cli_sigurg(int); 
static void cli_sigalrm(int);
static int cli_try_times, cli_query_sec, cli_max_try_times, cli_sockfd;

static void serv_sigurg(int); 
static void serv_sigalrm(int);
static int serv_try_times, serv_query_sec, serv_max_try_times, serv_sockfd;

void cli_heartbeat(int sockfd, int nsec, int max_try_times) {
	if ( (cli_query_sec = nsec) < 1)
		cli_query_sec = 1;
	if ( (cli_max_try_times = max_try_times) < cli_query_sec)
		cli_max_try_times = cli_query_sec;

	if (my_signal(SIGURG,cli_sigurg) == SIG_ERR)
		err_quit("my_signal error for SIGURG");
	cli_sockfd = sockfd;
	if (fcntl(cli_sockfd, F_SETOWN, getpid()) < 0)
		err_sys("fcntl");

	if (my_signal(SIGALRM, cli_sigalrm) == SIG_ERR)
		err_quit("my_signal error for SIGALRM");

	cli_try_times = 0;
	alarm(cli_query_sec);
}

static void cli_sigurg(int signo) {
	int		n;
	char	c;
	if ( (n = recv(cli_sockfd, &c, 1, MSG_OOB)) < 0)
		if (errno != EWOULDBLOCK)
			err_sys("recv");
	cli_try_times = 0;
}

static void cli_sigalrm(int signo) {
	if (++cli_try_times > cli_max_try_times) {
		fprintf(stderr, "server is unreachable\n");
		exit(0);
	}
	if (send(cli_sockfd, "C", 1, MSG_OOB) < 0)
		err_sys("send");
	alarm(cli_query_sec);
}

void serv_heartbeat(int sockfd, int nsec, int max_try_times) {
	serv_sockfd = sockfd;
	if ( (serv_query_sec = nsec) < 1)
		serv_query_sec = 1;
	if ( (serv_max_try_times = max_try_times) < serv_query_sec)
		serv_max_try_times = serv_query_sec;

	if (my_signal(SIGURG, serv_sigurg) == SIG_ERR)
		err_quit("my_signal error for SIGURG");
	if (fcntl(serv_sockfd, F_SETOWN, getpid()) < 0)
		err_sys("fcntl");

	if (my_signal(SIGALRM, serv_sigalrm) == SIG_ERR)
		err_quit("my_signal error for SIGALRM");

	serv_try_times = 0;
	alarm(serv_query_sec);
}

static void serv_sigalrm(int signo) {
	if (++serv_try_times > serv_max_try_times) {
		fprintf(stderr, "client is unreachable\n");
		exit(0);
	}
	alarm(serv_query_sec);
}

static void serv_sigurg(int signo) {
	int		n;
	char	c;

	if ( (n = recv(serv_sockfd, &c, 1, MSG_OOB)) < 0)
		if (errno != EWOULDBLOCK)
			err_sys("recv");
	serv_try_times = 0;
	if (send(serv_sockfd, &c, 1, MSG_OOB) < 0)
		err_sys("send");
}
