/*
 * Caller ID Server
 * (C)2004-2006, Dan Ponte
 * Licensed under the GPL v2
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
#define VERSION "0.3"
#ifdef SPEAKER
#include <machine/speaker.h>
int wantspkr = 1;
#else
int wantspkr = 0;
#endif
int usespkr = 0;
int ring = 0, nhosts = 0;
char hosts[10][18];
FILE* logfh;
char* devi;
static const char rcsid[] = "$Amigan: cidserv/src/cidserv.c,v 1.6 2006/03/22 22:18:44 dcp1990 Exp $";
int modemfd, sfd;
struct tm *ct;
time_t now;
short error = 0;
short logrings = 0;
int evalrc(char* result);
void send_dgram(char* address, char* datar);
int parse_cid(char* cidstring);
void load_addrs(char* fl);
short unsigned int longformat = 0;
char* logtime(void)
{
	static char tstring[20];
	bzero(tstring, sizeof tstring);
	now = time(NULL);
	ct = localtime(&now);
	sprintf(tstring, "%02d:%02d:%02d: L%02d/%02d",
			ct->tm_hour, ct->tm_min, ct->tm_sec, ct->tm_mon + 1,
			ct->tm_mday);
	return tstring;
}
#ifdef SPEAKER
void brring(void)
{
	tone_t tstr;
	int ctr;
	for(ctr = 0; ctr <= 4; ctr++) {
		tstr.frequency = 1000;
		tstr.duration = 10;
		ioctl(sfd, SPKRTONE, &tstr);
		tstr.frequency = 900;
		ioctl(sfd, SPKRTONE, &tstr);
	}
}
#endif
/* old
static void trap_hup(int nused)
{
	fprintf(stderr, 
		"Caught signal %d\n", nused);
	fflush(logfh);
	parse_cid("80190108313232313135303508014F020A3430313437343737343063\n");
	return;
}
static void trap_usr1(int nused)
{
	fprintf(stderr, 
		"Caught signal %d\n", nused);
	parse_cid("802701083039303532303130070F574952454C4553532043414C4C2020020A30303332303532363239AF\n");
	return;
}
static void trap_usr2(int nused)
{
	fprintf(stderr, 
		"Caught signal %d\n", nused);
	parse_cid("802701083132323130383234070F5354414E444953482048454154494E020A343031333937333337325C\n");
	return;
}
static void trap_term(int nused)
{
	fprintf(stderr, 
		"Caught signal %d, cleaning up...\n", nused);
	fprintf(logfh, "%s Caught signal %d, cleaning up...\n",
		       logtime(), nused);
	fflush(logfh);
	uu_unlock((devi+(sizeof("/dev/")-1)));
	close(modemfd);
	close(sfd);
	fclose(logfh);
	exit(0);
}*/
static void trap_sig(int sig)
{
	switch(sig) {
		case SIGHUP:
			fprintf(logfh,
				"Caught signal %d\n", sig);
			fflush(logfh);
			parse_cid("80190108313232313135303508014F020A3430313437343737343063\n");
			signal(SIGHUP, SIG_IGN);
			break;
		case SIGUSR1:
			fprintf(logfh,
				"Caught signal %d\n", sig);
			parse_cid("802701083039303532303130070F574952454C4553532043414C4C2020020A30303332303532363239AF\n");
			signal(SIGUSR1, SIG_IGN);
			break;
		case SIGUSR2:
			fprintf(logfh,
				"Caught signal %d\n", sig);
			parse_cid("802701083132323130383234070F5354414E444953482048454154494E020A343031333937333337325C\n");
			signal(SIGUSR2, SIG_IGN);
			break;
		case SIGTERM:
		case SIGINT:
#ifdef DEBUG
			fprintf(stderr, 
				"Caught signal %d, cleaning up...\n", sig);
#endif
			fprintf(logfh, "%s Caught signal %d, cleaning up...\n",
				       logtime(), sig);
			fflush(logfh);
			uu_unlock((devi+(sizeof("/dev/")-1)));
			close(modemfd);
			close(sfd);
			fclose(logfh);
			exit(0);
			break;
		default:
			fprintf(logfh, "Caught signal %d\n", sig);
	}
}
void usage(char* pname)
{
	fprintf(stderr, "Usage: %s %s[-Dfr] -d device -l logfile -a addresses\n"
			"-D to run as daemon (detach)\n-f for long log format\n%s", pname,
			wantspkr ? "[-s] " : "", 
			wantspkr ? "-s uses speaker(4) (FreeBSD) when the"
			" phone rings.\n" : "");
}
int main(int argc, char* argv[])
{
	char buffer[512], hipad[50];
	char cbuf[1];
	char *dev, *logfle, *addres;
	int cou = 0, ch;
	int runasd = 0;
	int deset = 0, logset = 0, addrset = 0;
	int doing_cid = 0, lres;
	struct timeval tv;
	fd_set fds_read;
	FILE* modem;
	int s;
	int length;
	struct sockaddr_in sin;
	struct sockaddr_in from;
	char netbuf[256];
	FD_ZERO(&fds_read);
#ifdef SPEAKER
	while ((ch = getopt(argc, argv, "sDd:l:a:fr")) != -1) {
#else
	while ((ch = getopt(argc, argv, "Dd:l:a:fr")) != -1) {
#endif
		switch(ch) {
			case 'd':
				dev = strdup(optarg);
				deset = 1;
				break;
			case 'l':
				logfle = strdup(optarg);
				logset = 1;
				break;
			case 'a':
				addres = strdup(optarg);
				addrset = 1;
				break;
			case 'r':
				logrings = 1;
				break;
#ifdef SPEAKER
			case 's':
				usespkr = 1;
				break;
#endif
			case 'D':
				runasd = 1;
				break;
			case 'f':
				longformat = 1;
				break;
			default:
				usage(argv[0]);
				exit(-1);
		}
	}	
	if(!deset || !logset || !addrset) {
		usage(argv[0]);
		exit(-1);
	} else {
		if(!(dev[0] == '/' && dev[3] == 'v')) {
			fprintf(stderr, "Must be a device!\n");
			exit(-1);
		}
	}
	memset(netbuf, 0, sizeof(netbuf));
	load_addrs(addres);
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return 1;
	}
	bzero(&sin, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(1450);
	if(INADDR_ANY)
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(s, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("bind");
		return 2;
	}
	/* end socket */
	devi = dev;
	if(!(logfh = fopen(logfle, "a"))) {
		perror("logf open");
		exit(-1);
	}
	now = time(NULL);
	ct = localtime(&now);
	fprintf(logfh, "%s CIDServ v%s started, opening modem...\n"
			, logtime(), VERSION);
	fflush(logfh);
	modemfd = open(dev, O_RDWR);
	lres = uu_lock((dev+(sizeof("/dev/")-1))); 
	signal(SIGTERM, trap_sig);
	signal(SIGHUP, trap_sig);
	signal(SIGUSR1, trap_sig);
	signal(SIGUSR2, trap_sig);
	signal(SIGINT, trap_sig);
	if(lres != 0) {
		fprintf(stderr, "%s\n", uu_lockerr(lres));
		exit(-1);
	}
	modem = fdopen(modemfd, "w+");
	if(runasd) {
		daemon(1, 0);
	}
	fprintf(modem, "ATZ\r\nAT E0 #CID=2 V0\r\n");
	fflush(modem);
#ifdef SPEAKER
	if(usespkr) {
		sfd = open("/dev/speaker", O_RDWR);
		if(sfd == -1) {
			perror("speaker open");
			exit(-1);
		}
	}
#endif
	while(1)
	{
                FD_ZERO(&fds_read);
                FD_SET(modemfd, &fds_read);
#ifdef DEBUG
		FD_SET(fileno(stdin), &fds_read);
#endif
		FD_SET(s, &fds_read);
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		length = sizeof from;
		switch(select(modemfd + 1, &fds_read, NULL, NULL, NULL))
		{
			case -1:
				perror("cidserv");
				exit(-1);
				break;
			case 0:
				fprintf(modem, "AT#CID=?\r\n");
				fflush(modem);
				break;
			default:
				{
				if(FD_ISSET(modemfd, &fds_read) != 0)
				{
					read(modemfd, cbuf, 1);
#ifdef DEBUG
					printf("%c", cbuf[0]);
#endif
					if(cou < sizeof(buffer) - 2)
						buffer[cou] = cbuf[0];
					if(buffer[0] == '8' && buffer[1] == '0')
						doing_cid = 1;
					if(cbuf[0] == '\n') {
						if(doing_cid) {
							parse_cid(buffer);
							memset(buffer, 0, sizeof(buffer));
							doing_cid = 0;
							cou = 0;
						} else {
							evalrc(buffer);
							memset(buffer, 0, sizeof(buffer));
							cbuf[0] = 0;
							cou = 0;
						}
					}	else {
						cbuf[0] = 0;
						cou++;
					}
				}
				if(FD_ISSET(fileno(stdin), &fds_read) != 0)
				{
					switch(getc(stdin)) {
						case 'q':
							trap_sig(SIGTERM);
							break;
						case 's':
							parse_cid("802701083039303532303130070F574952454C4553532043414C4C2020020A30303332303532363239AF\n");
							break;
						case 'l':
							parse_cid("802701083132323130383234070F5354414E444953482048454154494E020A343031333937333337325C\n");
							break;
						case 'o':
							parse_cid("80190108313232313135303508014F020A3430313437343737343063\n");
							break;
					}
				}
				if(FD_ISSET(s, &fds_read) != 0) {
					recvfrom(s, netbuf, 255, 0, (struct sockaddr*)&from, &length);
/*					strcpy(hipad, inet_ntoa(from.sin_addr));
			if(!((hipad[0] == '1' && hipad[1] == '0' && hipad[2]
							== '.') ||
				(hipad[0] == '1' && hipad[1] == '9' &&
				 hipad[2] == '2') || (hipad[0] == '1' &&
					 hipad[1] == '2' && hipad[3] == '7'))) {
				memset(hipad, 0, sizeof(hipad));
				fprintf(logfh, "Unauthorized access attempt"
						" from %s.\n",
						inet_ntoa(from.sin_addr));
				break;
			}*/
#ifdef DEBUG
					printf("client sent %s\n", netbuf);
#endif
				memset(hipad, 0, sizeof(hipad));
					if((netbuf[0] == 'A' && netbuf[1] ==
								'T')) {
					if(netbuf[strlen(netbuf)] == '\n')
						netbuf[strlen(netbuf)] = 0;
					fprintf(modem, "%s\r\n", netbuf);
					fflush(modem);
					now = time(NULL);
					ct = localtime(&now);
					fprintf(logfh, 
	"%02d:%02d:%02d: Remote (%s) sent %s, sending to modem...\n",
		ct->tm_hour, ct->tm_min, ct->tm_sec,
       		inet_ntoa(from.sin_addr), netbuf);
					fflush(logfh);
					} else {
						if(strcmp(netbuf, "AHU\n") ==
								0) {
					now = time(NULL);
					ct = localtime(&now);
	fprintf(logfh, "%02d:%02d:%02d: Telemarketers Suck!\n",
		ct->tm_hour, ct->tm_min, ct->tm_sec);
					fflush(logfh);
					fprintf(modem, "ATH1\r\n");
					fflush(modem);
					sleep(2);
					fprintf(modem, "ATH\r\n");
					fflush(modem);
						}
#ifdef SPEAKER
					if(strcmp(netbuf, "RNG\n") == 0) {
						brring();
					}
#endif
					}
					memset(&from, 0, sizeof(from));
					memset(netbuf, 0, sizeof(netbuf));
				}
			}
		}
	}
	close(modemfd);
	return 0;
}
int evalrc(char* result)
{
	int rescode, i;
	for(i = 0; i <= strlen(result); i++) {
		if(result[i] == '\r' || result[i] == '\n') result[i] = '\0';
	}
	rescode = atoi(result);
	switch(rescode) {
		case 0:
			/* OK */
			return 0;
			break;
		case 2:
			now = time(NULL);
			ct = localtime(&now);
			if(logrings) {
				fprintf(logfh, "%02d:%02d:%02d: Brrrring!!!!!!\n",
						ct->tm_hour, ct->tm_min, ct->tm_sec);
				fflush(logfh);
			}
			for(i = 0; i <= nhosts; i++) {
				send_dgram(hosts[i], "RING");
			}
#ifdef SPEAKER
			if(usespkr) brring();
#endif
			break;
		case 4:
			error = 1;
			return -1;
			break;
		default:
			return 0;
			break;
	}
	return 0;
}
int parse_cid(char* cidstring)
{
	char *p, *datep;
	int len = 0, i;
	char finalbuf[1024], msg[512];
	char printbuf[2048];
	unsigned char cch;
	char cbyte, cbyte2;
	char bytebuf[10];
	char date[7];
	time_t rnow;
	struct tm *ctm;
	char cidtime[7];
	char name[128];
	char phone[128];
	int cur = 0, sz, fbcou = 0;
	short int finl = 0;
	memset(msg, 0, sizeof msg);
	memset(bytebuf, 0, sizeof bytebuf);
	memset(date, 0, sizeof date);
	memset(finalbuf, 0, sizeof(finalbuf));
	memset(printbuf, 0, sizeof(printbuf) * sizeof(char));
	if(cidstring[strlen(cidstring)] == '\n')
		cidstring[strlen(cidstring)] = 0;
	sz = strlen(cidstring);
	finl = (sz / 2) - 2;
#ifdef DEBUG
	printf("Size of %s is %d\n", cidstring, sz);
#endif
	rnow = time(NULL);
	ctm = localtime(&rnow);
	for(cur = 0; cur <= sz; cur++) {
		cbyte = cidstring[cur++];
		cbyte2 = cidstring[cur];
		sprintf(bytebuf, "0x%c%c", cbyte, cbyte2);
		sscanf(bytebuf, "%X", &cch);
		if(cch == 0) continue;
		if(fbcou <= finl) {
			finalbuf[fbcou] = cch;
			fbcou++;
		} else break;
		memset(bytebuf, 0, sizeof(bytebuf));
		cbyte = 0;
		cbyte2 = 0;
	}
	sz = fbcou;
	p = finalbuf;
	len = sz;
	while(len && !(*p >= 0x30 && *p <= 0x39)) {
		p++;
		len--;
	}
	datep = p;
	date[0] = datep[0];
	date[1] = datep[1];
	date[2] = datep[2];
	date[3] = datep[3];
	date[4] = 0;
	datep += 4;
	cidtime[0] = datep[0];
	cidtime[1] = datep[1];
	cidtime[2] = datep[2];
	cidtime[3] = datep[3];
	cidtime[4] = 0;
	p += 8;
	len -= 8;
	if(longformat) {
		fprintf(logfh, "Date: %s (local %02d/%02d)\nTime: %s (local %02d:%02d)\n",
				date, ctm->tm_mon+1, ctm->tm_mday, cidtime,
				ctm->tm_hour, ctm->tm_min);
		fflush(logfh);
	} else {
		snprintf(printbuf, sizeof(printbuf), "%02d:%02d:%02d: L%02d/%02d %s:%s:",
				ctm->tm_hour, ctm->tm_min, ctm->tm_sec, ctm->tm_mon + 1,
				ctm->tm_mday, date, cidtime);
	}
	while(len && !isprint(*p)) {
		p++;
		len--;
	}
	i = 0;
	while(len && (i < sizeof(finalbuf)) && isprint(*p)) {
		name[i++] = *p++;
		len--;
	}
	name[i] = 0;
	if(name[0] == 'P' && !(isalpha(name[1]) || (name[1] >= '0' && name[1] <= '9'))) {
		strcpy(name, "PRIVATE");
	}
	if(name[0] == 'O' && !(isalpha(name[1]) || (name[1] >= '1' && name[1] <= '9'))) {
		strcpy(name, "UNAVAILABLE");
	}
	while(len && !isprint(*p)) {
		p++;
		len--;
	}
	i = 0;
	while(len && (i < sizeof(finalbuf)) && isprint(*p) && 
			(p[0] >= 0x20 && p[0] <= 0x7e)) {
		if(!((p[0] >= '0' && p[0] <= '9') || p[0] == 'O' || p[0] == 'P'))
		{
			p++;
			len--;
		       continue;
		} else {
			phone[i++] = *p++;
			len--;
		       }
	}
	phone[i] = 0;
	if(phone[0] == 'P')
	       strcpy(phone, "PRIVATE");
	if(phone[0] == 'O')
		strcpy(phone, "UNAVAILABLE");
	if(longformat) {
		fprintf(logfh, "Name: %s\nPhone Number: %s\n*********\n", name, phone);
		fflush(logfh);
	} else {
		snprintf(printbuf + strlen(printbuf), sizeof(printbuf) - strlen(printbuf) - 1, "%s:%s\n", name, phone);
	}
	if(!longformat) {
		fputs(printbuf, logfh);
		fflush(logfh);
	}
	sprintf(msg, "%s:%s:0:%s:%s", date, cidtime, name, phone);
#ifdef DEBUG
	printf("msg will be %s\n", msg);
#endif
	for(i = 0; i <= nhosts; i++) {
		send_dgram(hosts[i], msg);
	}
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
void load_addrs(char* fl)
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
