#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main() {
	char msg[212];
	int s;
	struct sockaddr_in sin;
	strcpy(msg, "1230:1234:3:DAN PONTE:2123456789");
	s = socket(PF_INET, SOCK_DGRAM, 0);
	bzero(&sin, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("10.10.10.1");
	sin.sin_port = htons(3890);
	if (connect(s, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("connect");
		close(s);
		return 2;
	}
	write(s, msg, sizeof(msg));
	close(s);
	return 0;
}
