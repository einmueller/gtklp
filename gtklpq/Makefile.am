AM_CPPFLAGS = $(FORTE_CCFLAGS) $(GTK2_CFLAGS) $(GTK_CFLAGS) $(CUPS_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/libgtklp -DLOCALEDIR=\"$(datadir)/locale\" $(GTKVER) -DMAX_PRT=$(MAXPRT)

bin_PROGRAMS = gtklpq
gtklpq_SOURCES = gtklpq.c gtklpq.h gtklpq_functions.c gtklpq_functions.h \
	jobtags.h printer.c printer.h \
	gtklpq_exit.xpm gtklpq_hold.xpm gtklpq_move.xpm gtklpq_prio.xpm \
	gtklpq_release.xpm gtklpq_start_printer.xpm gtklpq_stop_job.xpm \
	gtklpq_stop_jobs.xpm gtklpq_stop_printer.xpm

gtklpq_LDADD = $(top_builddir)/libgtklp/libgtklp.a $(FORTE_LDFLAGS) $(GTK2_LIBS) $(GTK_LIBS) $(CUPS_LIBS) $(XLIBS)
gtklpq_LDFLAGS= $(CUPS_LDFLAGS)
