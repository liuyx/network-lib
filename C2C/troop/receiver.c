#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define GROUP "225.0.0.37"
#define PORT 12345
#define BUF_SIZE 1024

typedef struct sockaddr SA;

static inline int family_to_level(int family) {
	switch(family) {
		case AF_INET:
			return IPPROTO_IP;
		default:
		case AF_INET6:
			return IPPROTO_IPV6;
	}
}

int mcast_join(int sockfd, const SA *grp, socklen_t grplen, const char *ifname, int ifindex) {
	 /*
	  * struct group_req {
	  *		unsigned int            gr_interface; // interface index, or 0
	  *		struct sockaddr_storage gr_group;	  // IPv4 or IPv6 multicast addr
	  * };
	  */
#ifdef MCAST_JOIN_GROUP2
	struct group_req req;
	if (ifindex > 0) {
		req.gr_interface = ifindex;
	} else if (ifname != NULL) {
		if ( (req.gr_interface = if_nametoindex(ifname)) == 0) {
			errno = ENXIO;
			return -1;
		}
	} else {
		req.gr_interface = 0;
	}

	if (grplen > sizeof(req)) {
		errno = EINVAL;
		return -1;
	}

	memcpy(req.gr_group, grp, grplen);
	return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_JOIN_GROUP, &req, sizeof(req));
#else
	switch (grp->sa_family) {
		case AF_INET: {
			struct ip_mreq mreq;
			struct ifreq ifreq;
			memcpy(&mreq.imr_multiaddr, &((const struct sockaddr_in *)grp)->sin_addr, sizeof(struct in_addr));

			if (ifindex > 0) {
				if (if_indextoname(ifindex, ifreq.ifr_name) == NULL) {
					errno = ENXIO;
					return -1;
				}
				goto doioctl;
			} else if (ifname != NULL) {
				strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
doioctl:
				if (ioctl(sockfd, SIOCGIFADDR, &ifreq) < 0) {
					return -1;
				}
				memcpy(&mreq.imr_interface, &((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr, sizeof(struct in_addr));
			} else {
				mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			}

			return setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	}

#ifdef IPV6
		case AF_INET6:
			break;
#endif
	}
	return 0;
#endif
}

int main(int argc, char **argv) {
	int		sockfd, n;
	const	int on = 1;
	struct	sockaddr_in servaddr;
	struct	ip_mreq mreq;
	char	buff[BUF_SIZE];
	socklen_t addrlen;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind");
		exit(1);
	}

	struct sockaddr_in group;
	group.sin_family = AF_INET;
	if (inet_pton(AF_INET, GROUP, &group.sin_addr) < 0) {
		perror("inet_pton");
		exit(1);
	}
	mcast_join(sockfd, (struct sockaddr *)&group,sizeof(group),"eth0",0);
	addrlen = sizeof(servaddr);

	while (1) {
		if ( (n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		buff[n] = 0;
		fputs(buff, stdout);
	}

	exit(0);
}

