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
/* $Amigan: cidserv/xcid/src/xcid.c,v 1.5 2005/11/10 21:17:54 dcp1990 Exp $ */
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
char* servaddr;
int start_netloop (void);
static const char rcsid[] = "$Amigan: cidserv/xcid/src/xcid.c,v 1.5 2005/11/10 21:17:54 dcp1990 Exp $";
#ifdef USE_XOSD
#include <xosd.h>
#define DEF_XOSD_TIMEOUT 5
#define DEF_XOSD_COLOUR "green"
#define DEF_XOSD_VOFFSET 30
#define DEF_XOSD_FONT "-misc-fixed-medium-r-semicondensed-*-13-*-*-*-c-*-koi8-r"
int XOSD_VOFFSET = DEF_XOSD_VOFFSET, XOSD_TIMEOUT = DEF_XOSD_TIMEOUT;
char *XOSD_FONT = DEF_XOSD_FONT;
char *XOSD_COLOUR = DEF_XOSD_COLOUR;
static int test = 0;
int vert = 1, horiz = 1; /* 1 is top or right, 0 is bottom or left */
#endif
void usage(void)
{
	fprintf(stderr, "Usage: xcid [-c colourname] [-f fontspec] [-o voffset"
			"] [-t timeoutsecs] [-a <b|t><l|r>] [-s]\n");
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
  	xosd *osd;
	while ((ch = getopt(argc, argv, "f:t:o:c:sa:")) != -1) {
		switch(ch) {
			case 'f':
				XOSD_FONT = strdup(optarg);
				break;
			case 'a': /* align... tr == top right bl ==
				     bottomleft */
				if(strlen(optarg) != 2) {
					fprintf(stderr, "-a error\n");
					exit(-1);
				}
				if(*optarg == 'b')
					vert = 0;
				if(optarg[1] == 'l')
					horiz = 0;
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
			case 's':
				test = 1;
				break;
			case '?':
			default:
				usage();
				exit(-1);
		}
	}
	argc -= optind;
	argv += optind;

	if(test) {
osd = xosd_create(2);
  xosd_set_font(osd, XOSD_FONT);
  xosd_set_colour(osd, XOSD_COLOUR);
  xosd_set_timeout(osd, XOSD_TIMEOUT);
  xosd_set_pos(osd, vert ? XOSD_top : XOSD_bottom);
  xosd_set_align(osd, horiz ? XOSD_right : XOSD_left);
  xosd_set_vertical_offset(osd, XOSD_VOFFSET);
  xosd_set_shadow_offset(osd, 1);
  xosd_display(osd, 0, XOSD_printf, "Welcome to xcid!");
  xosd_wait_until_no_display(osd);
  xosd_destroy(osd);
}

#endif
  start_netloop ();
#ifdef USE_XOSD
  free(XOSD_COLOUR);
  free(XOSD_FONT);
#endif
  return 0;
}
