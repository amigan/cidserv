#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
int main()
{
	int s, c;
	int b, len;
	fd_set fds;
	struct sockaddr_in sin;
	FILE* client;
	struct sockaddr_in from;
	char cbuf[64];
	FD_ZERO(&fds);
	memset(cbuf,0,sizeof(cbuf));
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
	while(1) {
		b = sizeof sin;
		FD_ZERO(&fds);
		FD_SET(s, &fds);
		switch(select(s+1, &fds, NULL, NULL, NULL)) {
			case -1:
				perror("select");
				return 3;
				break;
			case 0:
				break;
			default:
			{
				len = sizeof from;
				if(FD_ISSET(s, &fds) != 0) {
					printf("here\n");
					recvfrom(s, cbuf, 63, 0, (struct sockaddr*)&from, &len);
					printf("%s", cbuf);
					memset(cbuf,0,sizeof(cbuf));
				}
			}
		}
	}
	return 0;
}
