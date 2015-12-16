#include <stdio.h>

int main() {
#ifdef HAVE_BZEO
	printf("epoll\n");
#else
	printf("Other\n");
#endif
	return 0;
}
