/*
 * Caller ID Server mgetty support
 * (C)2004, Dan Ponte
 * Licensed under the BSD license, with advertising clause.
 */
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libutil.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define VERSION "0.1"
#define CIDLOG "/var/log/cidserv.log"
int nhosts = 0;
char hosts[10][18];
static const char rcsid[] = "$Amigan: cidserv/src/cnd_mgetty.c,v 1.3 2005/06/01 00:05:33 dcp1990 Exp $";
void send_dgram(char* address, char* datar);
void load_addrs(const char* fl);
int parse_cid(char* tty, char* phone, char* name, int dist_r, char* called);
int main(int argc, char* argv[])
{
	if(argc < 6) {
		fprintf(stderr, "Needs 5 args:\n"
		"<tty> <CallerID> <Name> <dist-ring-nr.> <Called Nr.>\n");
		exit(-2);
	}
	load_addrs(ADDRS);
	parse_cid(argv[1], argv[2], argv[3], atoi(argv[4]), argv[5]);
	return 0;
}
int parse_cid(tty, phone, name, dist_r, called)
	char* tty;
	char* phone;
	char* name;
	int dist_r;
	char* called;
{
	int i = 0;
	char msg[512];
	char fdate[256];
	FILE* logf;
	time_t rnow;
	struct tm *ctm;
	memset(msg, 0, sizeof msg);
	memset(fdate, 0, sizeof fdate);
	rnow = time(NULL);
	ctm = localtime(&rnow);
	strftime(fdate, sizeof(fdate) * sizeof(char),
			"%m%d:%H%M", ctm);
	sprintf(msg, "%s:0:%s:%s", fdate, name, phone);
	for(i = 0; i <= nhosts; i++) {
		send_dgram(hosts[i], msg);
	}
	logf = fopen(CIDLOG, "a");
	if(!logf) {
		perror("fopen " CIDLOG);
		exit(-1);
	}
	memset(fdate, 0, sizeof fdate);
	strftime(fdate, sizeof(fdate) * sizeof(char), "%H:%M:%S: L%m/%d %m%d:%H%M:", ctm);
	fprintf(logf, "%s%s:%s\n", fdate, name, phone);
	fclose(logf);
	return 0;
}
void send_dgram(char* address, char* datar)
{
	char msg[212];
	int s;
	int on = 1;
	struct sockaddr_in sin;
	if(strlen(address) < 3) return;
#ifdef DEBUG
	printf("send_dgram(%s) %p\n", address, address);
#endif
	strcpy(msg, datar);
	s = socket(PF_INET, SOCK_DGRAM, 0);
	bzero(&sin, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(address);
	sin.sin_port = htons(3890);
	if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*) &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(-1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("connect");
		close(s);
		return;
	}
	write(s, msg, strlen(msg) + 1);
	close(s);
	return;
}
void load_addrs(const char* fl)
{
	FILE* tfl;
	char fbuf[128];
	if(!(tfl = fopen(fl, "r"))) {
		perror("fopen");
		exit(-1);
	}
	while(!feof(tfl)) {
		fgets(fbuf, 126, tfl);
		if(fbuf[strlen(fbuf)] == '\n') fbuf[strlen(fbuf)] = 0;
		if(strlen(fbuf) > 4 && fbuf[0] != '#') strcpy(hosts[nhosts++], fbuf);
		memset(fbuf, 0, sizeof fbuf);
	}
	fclose(tfl);
}	
