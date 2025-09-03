AM_CPPFLAGS = $(FORTE_CCFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/libgtklp $(GTK2_CFLAGS) $(GTK_CFLAGS) $(CUPS_CFLAGS) -DLOCALEDIR=\"$(datadir)/locale\" $(GTKVER) -DMAX_PRT=$(MAXPRT) $(OLDDIALOG)

bin_PROGRAMS = gtklp
gtklp_SOURCES = file.c file.h general.c general.h gtklp.c gtklp.h \
	gtklp_functions.c gtklp_functions.h gtklptab.c gtklptab.h hpgl2.c \
	hpgl2.h image.c image.h output.c output.h ppd.c ppd.h special.c \
	special.h text.c text.h \
	black.xpm conflict.xpm duplex_long.xpm duplex_none.xpm \
	duplex_short.xpm logo.xpm logo2.xpm logo2_k.xpm logo_k.xpm rarrow.xpm \
	white.xpm

gtklp_LDADD = $(top_builddir)/libgtklp/libgtklp.a $(GTK2_LIBS) $(GTK_LIBS) $(CUPS_LIBS) $(FORTE_LDFLAGS) $(XLIBS)
gtklp_LDFLAGS = $(CUPS_LDFLAGS)
