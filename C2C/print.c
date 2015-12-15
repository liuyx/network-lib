#include <stdio.h>

int main() {
#ifdef __linux__
	printf("Linux\n");
#elif __mac__
	printf("mac\n");
#else
	printf("other\n");
#endif
	return 0;
}
