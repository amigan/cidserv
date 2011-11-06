/*
 * log.c - logfile stuff
 * (C)2005, Dan Ponte.
 * BSDL with advert.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <phoned.h>
#include <time.h>
#include <pthread.h>

FILE*	logf;
pthread_mutex_t logfmx = PTHREAD_MUTEX_INITIALIZER;
extern struct conf cf;
extern pthread_mutex_t cfmx;

int check_loglevel(lt, ll)
	enum ltype lt;
	int ll;
{
	switch(lt) {
		case debug:
			if(!(ll & LL_DEBUG))
				return -1;
		case garbage:
			if(!(ll & LL_GARBAGE))
				return -1;
		case info:
			if(!(ll & LL_INFO))
				return -1;
		case warn:
			if(!(ll & LL_WARN))
				return -1;
		case error:
			if(!(ll & LL_ERROR))
				return -1;
		case critical:
			if(!(ll & LL_CRITICAL))
				return -1;
			/* we cannot ignore FATAL. Because it's fatal. */
		default:
			return 1;
	}
}


int lprintf(enum ltype logtype, const char* fmt, ...)
{
	va_list ap;
	const char* fmtp;
	char* ofmt;
	int cnt = 0;
	int j, i, l;
	unsigned uns;
	double dou;
	char cha;
	char* str;
	void* voi;
	int maxsize;
	int tmp;
	const char *p;
	unsigned num;
	unsigned n;
	time_t now;
	char tmt[128];
	now = time(NULL);
	pthread_mutex_lock(&cfmx);
	l = cf.loglevels;
	pthread_mutex_unlock(&cfmx);
	if(check_loglevel(logtype, l) != 1)
		return -1;
	strftime(tmt, 128, "%d%b %H:%M:%S: ", localtime(&now));
	fmtp = fmt;
	maxsize = strlen(fmt) + 1;
	ofmt = malloc(sizeof(char) * (strlen(fmt) + 2));
	pthread_mutex_lock(&logfmx);
	fputs(tmt, logf);
	va_start(ap, fmt);
	while(*fmtp) {
		for(i = 0; fmtp[i] && fmtp[i] != '%' && i < maxsize; i++)
			ofmt[i] = fmtp[i];
		if(i) {
			ofmt[i] = '\0';
			cnt += fprintf(logf, ofmt);
			fmtp += i;
		} else {
			for(i = 0; !isalpha(fmtp[i]) && i < maxsize; i++) {
				ofmt[i] = fmtp[i];
				if(i && fmtp[i] == '%') break;
			}
			ofmt[i] = fmtp[i];
			ofmt[i + 1] = '\0';
			fmtp += i + 1;
			switch(ofmt[i]) {
				case 'd':
					j = va_arg(ap, int);
					cnt += fprintf(logf, ofmt, j);
					break;
				case 'o':
				case 'x':
				case 'X':
				case 'u':
					uns = va_arg(ap, unsigned);
					cnt += fprintf(logf, ofmt, uns);
					break;
				case 'b':
					num = (unsigned int)va_arg(ap, int);
					p = va_arg(ap, char *);
					cnt += fprintf(logf, "0x%x", num);
					if(num == 0) break;
					/* fprintf(logf, "%d and %d", num, *p); */
					for(tmp = 0; *p;) {
						n = (int)*p++;
						if(num & (1 << (n - 1))) {
							fputc(tmp ? ',' : '<', logf);
							cnt++;
							for(;(n = *p) > ' '; ++p) {
								fputc(n, logf);
								cnt++;
							}
							tmp = 1;
						} else {
							for(;*p > ' '; ++p)
								continue;
						/*fprintf(logf, "n=0x%x,nxs=0x%x,c=%d\n", n, (1 << (n-1)), (char)n); */
						}
					}
					if(tmp)
						fputc('>', logf);
					break;
				case 'c':
					cha = (char) va_arg(ap, int);
					cnt += fprintf(logf, ofmt, cha);
					break;
				case 's':
					str = va_arg(ap, char*);
					cnt += fprintf(logf, ofmt, str);
					break;
				case 'f':
				case 'e':
				case 'E':
				case 'G':
				case 'g':
					dou = va_arg(ap, double);
					cnt += fprintf(logf, ofmt, dou);
					break;
				case 'p':
					voi = va_arg(ap, void*);
					cnt += fprintf(logf, ofmt, voi);
					break;
				case 'n':
					cnt += fprintf(logf, "%d", cnt);
					break;
				case '%':
					fputc('%', logf);
					cnt++;
					break;
				default:
					fprintf(stderr, "Invalid format in log!\n");
					break;
			}
		}
	}
	if(fmt[strlen(fmt)-1] != '\n')
		fputc('\n', logf);
	fflush(logf);
	pthread_mutex_unlock(&logfmx);
	va_end(ap);
	free(ofmt); /* MUST do this */
	return cnt;
}
void cid_log(cid_t* c)
{
	lprintf(info, "Call! %02d%02d:%02d%02d:%s:%s\n", c->month, c->day, c->hour, c->minute,
			c->name, c->number);
}
