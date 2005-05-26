/*
 * XCid - For use with CIDServ
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
/* $Amigan: cidserv/xcid/src/xcid.c,v 1.4 2005/05/26 23:40:37 dcp1990 Exp $ */
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
char* servaddr;
int start_netloop (void);
static const char rcsid[] = "$Amigan: cidserv/xcid/src/xcid.c,v 1.4 2005/05/26 23:40:37 dcp1990 Exp $";
#ifdef USE_XOSD
#define DEF_XOSD_TIMEOUT 5
#define DEF_XOSD_COLOUR "green"
#define DEF_XOSD_VOFFSET 30
#define DEF_XOSD_FONT "-misc-fixed-medium-r-semicondensed-*-13-*-*-*-c-*-koi8-r"
int XOSD_VOFFSET = DEF_XOSD_VOFFSET, XOSD_TIMEOUT = DEF_XOSD_TIMEOUT;
char *XOSD_FONT = DEF_XOSD_FONT;
char *XOSD_COLOUR = DEF_XOSD_COLOUR;
#endif
void usage(void)
{
	fprintf(stderr, "Usage: xcid [-c colourname] [-f fontspec] [-o voffset"
			"] [-t timeoutsecs]\n");
}
int
main (int argc, char *argv[])
{
/*	if(argc < 2) {
		fprintf(stderr, "Usage: %s server-ip\n", argv[0]);
		exit(1);
	}
	servaddr = strdup(argv[1]); */
#ifdef USE_XOSD
	int ch;
	while ((ch = getopt(argc, argv, "f:t:o:c:")) != -1) {
		switch(ch) {
			case 'f':
				XOSD_FONT = strdup(optarg);
				break;
			case 't':
				XOSD_TIMEOUT = atoi(optarg);
				if(XOSD_TIMEOUT < 1) XOSD_TIMEOUT = 
					DEF_XOSD_TIMEOUT;
				break;
			case 'o':
				XOSD_VOFFSET = atoi(optarg);
				break;
			case 'c':
				XOSD_COLOUR = strdup(optarg);
				break;
			case '?':
			default:
				usage();
				exit(-1);
		}
	}
	argc -= optind;
	argv += optind;
#endif
  start_netloop ();
#ifdef USE_XOSD
  free(XOSD_COLOUR);
  free(XOSD_FONT);
#endif
  return 0;
}
