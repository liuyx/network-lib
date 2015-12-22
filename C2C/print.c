#include <stdio.h>

int main() {
#ifdef BZEO
	printf("Linux\n");
#elif defined(__APPLE__)
	printf("mac\n");
#else
	printf("other\n");
#endif
	return 0;
}
