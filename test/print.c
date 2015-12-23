#include <stdio.h>

int main(){
#ifdef __APPLE__
	printf("OSX\n");
#elif defined(__linux__)
	printf("linux\n");
#elif defined(win32)
	printf("windows\n");
#else
	printf("other\n");
#endif
	return 0;
}
