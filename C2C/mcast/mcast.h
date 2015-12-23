#ifndef __MCAST_H__
#define __MCAST_H__

#include <sys/types.h>
#include <sys/socket.h>

typedef struct sockaddr SA;

#define MAXLINE 1024

int mcast_join(int sockfd, const struct sockaddr *grp, socklen_t grplen,
		const char *ifname, u_int ifindex);

int mcast_leave(int sockfd, const struct sockaddr *grp, socklen_t grplen);

int mcast_block_source(int sockfd, const struct sockaddr *src, socklen_t srclen, const struct sockaddr *grp, socklen_t grplen);

int mcast_unblock_source(int sockfd, const struct sockaddr *src, socklen_t srclen, const struct sockaddr *grp, socklen_t grplen);

int mcast_join_source_group(int sockfd, const struct sockaddr *src, socklen_t srclen, const struct sockaddr *grp, socklen_t grplen, const char *ifname, u_int ifindex);

int mcast_leave_source_group(int sockfd, const struct sockaddr *src, socklen_t srclen, const struct sockaddr *grp, socklen_t grplen);

int mcast_set_if(int sockfd, const char *ifname, u_int ifindex);

int mcast_set_loop(int sockfd, int flag);

int mcast_set_ttl(int sockfd, int ttl);

int mcast_get_if(int sockfd);

int mcast_get_loop(int sockfd);

int mcast_get_ttl(int sockfd);

#endif
