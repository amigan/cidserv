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
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef USE_XOSD
#define DEF_XOSD_TIMEOUT 5
#define DEF_XOSD_COLOUR "green"
#define DEF_XOSD_VOFFSET 30
#define DEF_XOSD_FONT "-misc-fixed-medium-r-semicondensed-*-13-*-*-*-c-*-koi8-r"
#include <xosd.h>
extern int XOSD_TIMEOUT, XOSD_VOFFSET;
extern char *XOSD_COLOUR, *XOSD_FONT;
#endif
short int tsec = 10;
typedef struct cis
{
  char *name;
  char *number;
  char *date;
  char *time;
} cidinfo;
void sendtoser(char* txt);

XtAppContext app_context;
Widget toplevel;		/* The overall window.       */
Widget form;			/* The layout of the window. */
Widget quit_button;		/* The button in the window. */
Widget telemar;
Widget mlabel;
XtIntervalId timer;
XtPointer test;
short int tflag = 0;

void htime(XtPointer client_data, XtIntervalId *timer) 
{
 XEvent bogus_event;
 Display** displays;
 int numdr;
#ifdef DEBUG
	printf("here\n");
#endif
	if(--tsec == 0) {
//  XtDestroyApplicationContext (app_context);
#ifdef DEBUG
	printf("alive\n");
#endif
	/* This is EVIL */
  tflag = 1;
 XtGetDisplays(app_context, &displays, &numdr);
 bogus_event.type = 0;        /* XAnyEvent type, ignored. */
bogus_event.xany.display = displays[0];
bogus_event.xany.window = 0;
XPutBackEvent(displays[0], &bogus_event);
  XtAppSetExitFlag (app_context); }
	else {
  timer = (XtIntervalId)XtAppAddTimeOut(app_context, 
		  (unsigned long)1000, 
		  (XtTimerCallbackProc)htime, 
		  test);
	}
}
/* The callback function for the quit button. */
void
quit_proc (Widget w, XtPointer client_data, XtPointer call_data)
{
  XtDestroyApplicationContext (app_context);
  XtAppSetExitFlag (app_context);
}
void telecall (Widget w, XtPointer client_data, XtPointer call_data)
{
	sendtoser("AHU\n");
}
int
wind (int argc, char **argv, char *labtext)
{
  /* Create the window and its contents' appearance. */
  toplevel = XtOpenApplication (&app_context, "xcid", NULL, 0, &argc,
				argv, NULL,
				applicationShellWidgetClass, NULL, 0);
  XtVaSetValues(toplevel, XtNtitle, "XCid - Incoming Call", NULL);
  form = XtVaCreateManagedWidget ("form", formWidgetClass, toplevel, NULL);
  mlabel =
    XtVaCreateManagedWidget ("mlabel", labelWidgetClass, form, XtNwidth, 200,
			     XtNheight, 200, NULL);
  telemar =
    XtVaCreateManagedWidget ("telemar", commandWidgetClass, form, XtNlabel,
			     "Telemarketerize", XtNfromVert, mlabel, NULL);

  quit_button = XtVaCreateManagedWidget ("quit_button", commandWidgetClass,
					 form, XtNlabel, "Close", XtNfromHoriz,
					 telemar, XtNfromVert, mlabel, NULL);
  timer = XtAppAddTimeOut(app_context, 1000, htime, (XtPointer) 0);
  /* Create window's behavior. */
  XtAddCallback (quit_button, XtNcallback, quit_proc, NULL);
  XtAddCallback(telemar, XtNcallback, telecall, NULL);
  /* Display the window. */
  XtRealizeWidget (toplevel);
  XtVaSetValues (mlabel, XtNlabel, labtext, NULL);

  /* Infinite loop, waiting for events to happen. */
  XtAppMainLoop (app_context);
	if(tflag) {
#ifdef DEBUG
	printf("about...\n");
#endif
  XtDestroyApplicationContext (app_context);
	tflag = 0;
	tsec = 10;
#ifdef DEBUG
	printf("done...\n");
#endif
	}
  return 0;
}

void
parseinfo (buffer, cifo)
     char *buffer;
     cidinfo *cifo;
{
  char *bfrp;
  char *origbuf;
  origbuf = buffer;
  bfrp = origbuf;
  /* 9005:2010:0:WIRELESS CALL   :401213123123 */
  cifo->date = strsep (&bfrp, ":");
  cifo->time = strsep (&bfrp, ":");
  if(cifo->time == NULL) return;
  strsep (&bfrp, ":");
  cifo->name = strsep (&bfrp, ":");
  cifo->number = bfrp;
}

void
telluser (buf)
     char *buf;
{
  cidinfo cid;
#ifdef USE_XOSD
  xosd *osd;
#endif 
#ifndef USE_XOSD
  char *tav[] = { "xcid", NULL };
  char *ltx;
  size_t lent;
#endif
  bzero(&cid, sizeof cid);
  parseinfo (buf, &cid);
  if(cid.name == NULL) return;
#ifdef DEBUG
  printf ("Name: %s\nNum: %s\nDate: %s\nTime: %s\n",
	  cid.name, cid.number, cid.date, cid.time);
#endif
  if(cid.name == NULL || cid.number == NULL) {return;}
#ifdef USE_XOSD
  /* lent += sizeof(" -- \nDate:  -- Time:\n       ");
  ltx = malloc(lent);
  memset(ltx, 0, lent);
  snprintf(ltx, lent, "%s -- %s\nDate: %s -- Time: %s",
		  cid.name, cid.number, cid.date, cid.time);*/
  osd = xosd_create(2);
  xosd_set_font(osd, XOSD_FONT);
  xosd_set_colour(osd, XOSD_COLOUR);
  xosd_set_timeout(osd, XOSD_TIMEOUT);
  xosd_set_pos(osd, XOSD_top);
  xosd_set_align(osd, XOSD_right);
  xosd_set_vertical_offset(osd, XOSD_VOFFSET);
  xosd_set_shadow_offset(osd, 1);
  xosd_display(osd, 0, XOSD_printf, "%s -- %s", cid.name, cid.number);
  xosd_display(osd, 1, XOSD_printf, "Date: %s -- Time: %s", cid.date,
		  cid.time);
  xosd_wait_until_no_display(osd);
  xosd_destroy(osd);
#else
  lent +=	sizeof ("Name: \nNumber: \nDate: \nTime: \n           ");
  lent =
    sizeof (char) * (strlen (cid.name) + strlen (cid.number) +
		     strlen (cid.date) + strlen (cid.time));
  ltx = (char *) malloc (lent);
  memset (ltx, 0, lent);
  snprintf (ltx, lent, "Name: %s\nNumber: %s\nDate: %s\nTime: %s\n",
	    cid.name, cid.number, cid.date, cid.time);
  wind (0, tav, ltx);
  free (ltx);
#endif
}
