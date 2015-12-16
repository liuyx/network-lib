#include <stdio.h>

int main() {
#ifdef __linux__
	printf("Linux\n");
#elif defined(__APPLE__)
	printf("mac\n");
#else
	printf("other\n");
#endif
	return 0;
}
