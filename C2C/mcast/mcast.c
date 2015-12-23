#include "mcast.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <netdb.h>
#include <net/if.h>
#include <stdarg.h>

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

int family_to_level(int family) {
	switch(family) {
#ifdef IPV6
		case AF_INET6:
			return IPPROTO_IPV6;
#endif
		case AF_INET:
		default:
			return IPPROTO_IP;
	}
}

int mcast_join(int sockfd, const SA *grp, socklen_t grplen, const char *ifname, u_int ifindex) {
#ifdef MCAST_JOIN_GROUP
	struct group_req req;
	if (ifindex > 0) {
		req.gr_interface = ifindex;
	} else if (ifname != NULL) {
		if ( (req.gr_interface = if_nametoindex(ifname)) == 0) {
			errno = ENXIO; /* i/f name not found */
			return -1;
		}
	} else {
		req.gr_interface = 0;
	}

	if (grplen > sizeof(req.gr_group)) {
		errno = EINVAL;
		return -1;
	}

	memcpy(&req.gr_group, grp, grplen);
	return setsockopt(sockfd, family_to_level(grp->sa_family),MCAST_JOIN_GROUP, &req, sizeof(req));
#else
	switch(grp->sa_family) {
		case AF_INET:
			struct	ip_mreq mreq;
			struct	ifreq ifreq;

			memcpy(&mreq.imr_multiaddr, &((const struct sockaddr_in *)grp)->sin_addr, sizeof(struct in_addr));

			if (ifindex > 0) {
				if (if_indextoname(ifindex,ifreq.ifr_name) == NULL) {
					errno = ENXIO;
					return -1;
				}
				goto doioctl;
			} else if (ifname != NULL) {
				strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
doioctl:
				if (ioctl(sockfd, SIOCGIFADDR, &ifreq) < 0)
					return -1;
				memcpy(&mreq.imr_interface, &((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
			} else {
				mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			}

			return setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

#ifdef IPV6
		case AF_INET6:
			struct ipv6_mreq mreq6;

			memcpy(&mreq6.ipv6mr_multiaddr, &((const struct sockaddr_in6 *)grp)->sin6_addr, sizeof(struct in6_addr));

			if (ifindex > 0) {
				mreq6.ipv6mr_interface = ifindex;
			} else if (ifname != NULL) {
				if ( (mreq6.ipv6mr_interface = if_nametoindex(ifname)) == 0) {
					errno = ENXIO;
					return -1;
				}
			} else 
				mreq6.ipv6mr_interface = 0;

			return setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6));
#endif
		default:
			errno = EAFNOSUPPORT;
			return -1;
	}
#endif
}

int udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp) {
	int		sockfd, n;
	struct	addrinfo hints, *res, *ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host,serv,&hints,&res)) != 0) 
		err_quit("udp_client error for %s, %s: %s", host, serv, gai_strerror(n));

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd >= 0)
			break;
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		err_sys("udp_client error for %s, %s",host, serv);

	*saptr = malloc(res->ai_addrlen);
	memcpy(*saptr, res->ai_addr,res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return sockfd;
}

void loop(int sockfd, socklen_t salen) {
}

int main(int argc,char **argv) {
	int		sockfd;
	const	int on = 1;
	socklen_t salen;
	struct	sockaddr *sa;

	sockfd = udp_client(argv[1],argv[2],&sa,&salen);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	bind(sockfd, sa, salen);

	mcast_join(sockfd, sa, salen, (argc == 4) ? argv[3] : NULL, 0);
	loop(sockfd, salen);

	return 0;
}
