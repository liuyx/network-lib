#include "tools.h"

void err_doit(int errorflag, int error, const char *fmt, va_list ap) {
	char buff[MAXLINE];
	vsnprintf(buff,MAXLINE,fmt,ap);
	if (errorflag) {
		size_t len = strlen(buff);
		snprintf(buff + len, MAXLINE - len, ": %s",strerror(errno));
	}
	strcat(buff,"\n");
	fflush(stdout);
	fprintf(buff,stderr);
	fflush(NULL);
}

void err_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(0,0,fmt,ap);
	va_end(ap);
	exit(1);
}

void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,errno,fmt,ap);
	va_end(ap);
	exit(1);
}
