/*
 * XCid - network stuff
 * (C)2004, Dan Ponte
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Dan Ponte nor the names of his contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DAN PONTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL DAN PONTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3890
#define BUFFERL 256
struct sockaddr_in servsad;
char servaddr[256];
void telluser(char* buf);

void sendtoser(char* txt)
{
	int s;
	struct sockaddr_in sin;
	s = socket(PF_INET, SOCK_DGRAM, 0);
	bzero(&sin, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr = servsad.sin_addr;/*.s_addr = inet_addr(servaddr); */
#ifdef DEBUG
	printf("it is %s\n", inet_ntoa(sin.sin_addr));
#endif
	sin.sin_port = htons(1450);
	if(connect(s, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("Connect");
		exit(-1);
	}
	write(s, txt, strlen(txt) + 1);
	close(s);
	return;
}
int start_netloop(void)
{
	int sockfd, addr_len, nbytes;
	struct sockaddr_in ouraddr;
	struct sockaddr_in* bcasaddr;
	struct timeval tv;
	bcasaddr = &servsad;
	bzero(&servsad, sizeof servsad);
	fd_set fds_read;
	FD_ZERO(&fds_read);
	char buffer[BUFFERL];

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("sock");
		exit(-1);
	}
	ouraddr.sin_family = AF_INET;
	ouraddr.sin_port = htons(PORT);
	ouraddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(ouraddr.sin_zero), 0, 8);
	if(bind(sockfd, (struct sockaddr*)&ouraddr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(-2);
	}
	addr_len = sizeof(struct sockaddr);
	while(1)
	{
		FD_ZERO(&fds_read);
		FD_SET(sockfd, &fds_read);
		tv.tv_sec = 3; tv.tv_usec = 0;
		switch(select(sockfd + 1, &fds_read, NULL, NULL, NULL))
		{
			case -1:
				perror("select");
				exit(-1);
				break;
			default:
				if(FD_ISSET(sockfd, &fds_read) != 0)
				{
					
					if((nbytes = recvfrom(sockfd, buffer, BUFFERL - 1, 0, (struct sockaddr*)bcasaddr, &addr_len
				  			   )) == -1) {
						perror("recv");
						exit(-3);
					}
					buffer[nbytes] = 0;
#ifdef DEBUG
					printf("got %s\n", buffer);
#endif
					telluser(buffer);
					memset(buffer, 0, BUFFERL);
				}
		}
	}
	close(sockfd);
	return 0;
}
