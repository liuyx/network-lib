#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {
#ifdef IP_ADD_MEMBERSHIP
	printf("IP_ADD_MEMBERSHIP\n");
#elif defined (MCAST_JOIN_GROUP)
	printf("MCAST_JOIN_GROUP\n");
#else
	printf("Other\n");
#endif
	return 0;
}
