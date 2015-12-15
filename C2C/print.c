#include <stdio.h>

int main() {
#ifdef __linux__
	printf("Linux\n");
#else
	printf("Other\n");
#endif
	return 0;
}
