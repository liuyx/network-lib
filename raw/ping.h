#ifndef __PING_H
#define __PING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFSIZE 1500

char sendbuf[BUFSIZE];

int		datalen; /* # bytes of data following ICMP header */
char	*host;
int		nsent;
pid_t	pid;
int		sockfd;
int		verbose;

/* function prototypes */
void	init_v6();
void	proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void	proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
void	send_v4();
void	send_v6();
void	readloop();
void	sig_alrm(int);
void	tv_sub(struct timeval *, struct timeval *);

struct proto {
	void	(*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
	void	(*fsend)();
	void	(*finit)();
	struct sockaddr *sasend; 
	struct sockaddr *sarecv;
	socklen_t salen;
	int	   icmpproto; /* IPPROTO_xxx value for ICMP */
} *pr;

#ifdef IPV6
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#endif
#endif
