#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
int parse_cid(char* cidstring)
{
	char *p, *datep;
	int len = 0, i;
	char finalbuf[256], msg[512];
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
	memset(msg, 0, sizeof msg);
	memset(bytebuf, 0, sizeof bytebuf);
	memset(date, 0, sizeof date);
	memset(finalbuf, 0, sizeof(finalbuf));
	sz = strlen(cidstring);
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
		printf("fc = %d\n", fbcou);
		if(fbcou < 41)
		{       
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
	fprintf(stdout, "Date: %s (local %02d/%02d)\nTime: %s (local %02d:%02d)\n",
			date, ctm->tm_mon+1, ctm->tm_mday, cidtime,
			ctm->tm_hour, ctm->tm_min);
	while(len && !isprint(*p)) {
		p++;
		len--;
	}
	i = 0; /* bounds checking */
	while(len && p - finalbuf < sizeof(finalbuf) /* && (i < sizeof(finalbuf) - 1) */&& isprint(*p)) {
		printf("%p, %p, %d, %c\n", p, finalbuf, p - finalbuf, *p);
		name[i++] = *p++;
		len--;
		printf("next one will be %c\n", *p);
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
	while(len && (p - finalbuf < sizeof(finalbuf) - 1) && isprint(*p) && 
			(p[0] >= 0x20 && p[0] <= 0x7e)) {
		if(!((p[0] >= '0' && p[0] <= '9') || p[0] == 'O' || p[0] == 'P'))
		{
			++p;
			continue;
		}	else {
			phone[i++] = *p++;
			len--;
		       }
	}
	phone[i] = 0;
	if(phone[0] == 'P')
	       strcpy(phone, "PRIVATE");
	if(phone[0] == 'O')
		strcpy(phone, "UNAVAILABLE");
	fprintf(stdout, "Name: %s\nPhone Number: %s\n*********\n", name, phone);
	fflush(stdout);
	sprintf(msg, "%s:%s:0:%s:%s", date, cidtime, name, phone);
#ifdef DEBUG
	printf("msg will be %s\n", msg);
#endif
	return 0;
}
int main(int argc, char* argv[])
{
	char* avb;
	avb = strdup(argv[1]);
	parse_cid(avb);
	free(avb);
	return 0;
}
