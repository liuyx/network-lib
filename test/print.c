#include <stdio.h>

int main() {
#ifdef __linux__
	printf("linux\n");
#elif defined(win32)
	printf("win32\n");
#elif defined(__APPLE__)
	printf("apple\n");
#endif
	return 0;
}
