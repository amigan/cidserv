#include <string.h>
#include <stdio.h>
int main()
{
	char buf[512];
	char* p;
	char* datep;
	int len = 0;
	int i;
	char cch;
	char finalbuf[512];
	char cbyte;
	char cbyte2;
	char bytebuf[10];
	char date[7];
	char time[7];
	char name[128];
	char phone[128];
	int waitingsec = 0;
	int cur = 0;
	int sz;
	int fbcou = 0;
	memset(finalbuf, 0, sizeof(finalbuf));
	memset(buf, 0, sizeof(buf));
	scanf("%s", buf);
	sz = strlen(buf);
	printf("Size of %s is %d\n", buf, sz);
	for(cur = 0; cur <= sz; cur++) {
		cbyte = buf[cur++];
		cbyte2 = buf[cur];
		sprintf(bytebuf, "0x%c%c", cbyte, cbyte2);
		sscanf(bytebuf, "%X", &cch);
		if(cch == 0) continue;
		finalbuf[fbcou] = cch;
		fbcou++;
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
	date[2] = '/';
	date[3] = datep[2];
	date[4] = datep[3];
	date[5] = 0;
	datep += 4;
	time[0] = datep[0];
	time[1] = datep[1];
	time[2] = ':';
	time[3] = datep[2];
	time[4] = datep[3];
	time[5] = 0;
	p += 8;
	len -= 8;
	printf("Date: %s\nTime: %s\n", date, time);
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
	while(len && !isprint(*p)) {
		p++;
		len--;
	}
	i = 0;
	while(len && (i < sizeof(finalbuf)) && isprint(*p)) {
		phone[i++] = *p++;
		len--;
	}
	phone[i] = 0;
	printf("Name: %s\nPhone Number: %s\n", name, phone);
	return 0;
}
