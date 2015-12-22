#include <stdio.h>
#include <string.h>
#include <strings.h>

int main(){
#ifndef HAVE_SYS_SELECT_H
	printf("not bzero\n");
#else
	printf("hava bzero\n");
#endif
}
