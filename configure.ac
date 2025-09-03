AC_INIT
AC_GNU_SOURCE
AC_CONFIG_SRCDIR([gtklp/gtklp.c])                                                                                                
AM_INIT_AUTOMAKE(gtklp, 1.3.4)                                                                                         

AC_CONFIG_HEADERS([config.h])                                                                                            


AC_CONFIG_MACRO_DIR([m4])
AM_GNU_GETTEXT([external])

dnl Extra params
CUPSCONFIGPATH=""
AC_ARG_WITH(cups-config,[  --with-cups-config=CUPS-CONFIG   set full path to cups-config], CUPSCONFIGPATH=$withval)
AC_ARG_ENABLE(ssl, [  --enable-ssl            turn on SSL/TLS support ,enabled by default])
AC_ARG_ENABLE(forte, [  --enable-forte          dont use -Wall, but use -lX11, to compile with suns forte, disabled by default])
AC_ARG_ENABLE(oldfile, [  --enable-oldfile        dont use the new file selection dialog])
AC_ARG_WITH(maxprinters, [  --with-maxprinters=n    set the maximum number of supported printers, defaults to 150], MAX_PRT=$withval)

dnl Checks for programs
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_LIBTOOL


dnl Checks for libraries
XLIBS="-lX11"
AC_SUBST(XLIBS)


dnl Checks for header files

dnl internationalization macros
AM_GNU_GETTEXT


# Forte Compiler ############################################################
FORTE_LDFLAGS=""
FORTE_CCFLAGS="-Wall"
if test x$enable_forte = xyes; then
	FORTE_LDFLAGS="-lX11"
	FORTE_CCFLAGS=""
fi
AC_SUBST(FORTE_LDFLAGS)
AC_SUBST(FORTE_CCFLAGS)

# old file dialog ###########################################################
OLDDIALOG=""
if test x$enable_oldfile = xyes; then
	OLDDIALOG="-DOLDDIALOG"
fi
AC_SUBST(OLDDIALOG)

# gtk #######################################################################
PKG_CHECK_MODULES(GTK2, gtk+-2.0)
AC_SUBST(GTK2_CFLAGS)
AC_SUBST(GTK2_LIBS)
GTKVER="-DHAVE_GTK2"

AC_SUBST(GTKVER)

# cups ######################################################################
FOUNDCUPSCONFIG=0
if test "${CUPSCONFIGPATH}" != ""; then
	if test "${CUPSCONFIGPATH}" = "no"; then
		AC_MSG_RESULT([ ************************************************* ])
		AC_MSG_RESULT([ * Not to use cups-config is unsafe. Be warned ! * ])
		AC_MSG_RESULT([ ************************************************* ])
	else
		AC_MSG_CHECKING([for cups-config])
		if test -x "${CUPSCONFIGPATH}"; then
			FOUNDCUPSCONFIG=1
			CUPS_CONFIG="${CUPSCONFIGPATH}"
			AC_MSG_RESULT([${CUPSCONFIGPATH}])
		else
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([${CUPSCONFIGPATH} not found or not executable !])
		fi
	fi
else
	AC_PATH_PROG(CUPS_CONFIG, cups-config, no)
	if test x${CUPS_CONFIG} != xno; then
		FOUNDCUPSCONFIG=1
	else
		AC_MSG_ERROR([cups-config not found !])
	fi
fi

if test ${FOUNDCUPSCONFIG} != 0; then
	CUPS_CFLAGS="`${CUPS_CONFIG} --cflags`"
	CUPS_LIBS="`${CUPS_CONFIG} --libs`"
	CUPS_LDFLAGS="`${CUPS_CONFIG} --ldflags`"
	if test x$enable_ssl != xno; then
		CUPS_CFLAGS="${CUPS_CFLAGS} -DHAVE_LIBSSL"
	fi
	AC_SUBST(CUPS_CFLAGS)
	AC_SUBST(CUPS_LIBS)
	AC_SUBST(CUPS_LDFLAGS)
else
	CUPS_CFLAGS=""
	CUPS_LIBS="-lcups -lssl -lcrypto -lnsl"
	CUPS_LDFLAGS=""
	if test x$enable_ssl != xno; then
		CUPS_CFLAGS="${CUPS_CFLAGS} -DHAVE_LIBSSL"
	fi
	AC_SUBST(CUPS_CFLAGS)
	AC_SUBST(CUPS_LIBS)
	AC_SUBST(CUPS_FLAGS)
fi

# MAX_PRT #################################################################
MAXPRT="150"
if test "${MAX_PRT}" != ""; then
	if test "${MAX_PRT}" != "no"; then
		MAXPRT=${MAX_PRT}
	fi
fi

AC_MSG_RESULT([ *** Setting maximum number of printers to ${MAXPRT} *** ])
AC_SUBST(MAXPRT)

############################################################################


AC_CONFIG_FILES([Makefile po/Makefile.in po/Makefile libgtklp/Makefile gtklp/Makefile gtklpq/Makefile man/Makefile m4/Makefile   ])
AC_OUTPUT


  AC_MSG_RESULT([     *********************************** ])

if test x$enable_forte = xyes; then
	AC_MSG_RESULT([     forte support:       yes])
else
	AC_MSG_RESULT([     forte support:       no])
fi

if test x$enable_gtk1 = xyes; then
	AC_MSG_RESULT([     use gtk1:            yes])
else
	AC_MSG_RESULT([     use gtk1:            no])
fi

if test "${OLDDIALOG}" != ""; then
	AC_MSG_RESULT([     use old file dialog: yes])
else
	AC_MSG_RESULT([     use old file dialog: no])
fi

if test "${CUPSCONFIGPATH}" != ""; then
	AC_MSG_RESULT([     cups config:         ${CUPSCONFIGPATH}])
else
	AC_MSG_RESULT([     cups config:         <default>])
fi

if test "${MAX_PRT}" != ""; then
	AC_MSG_RESULT([     Max Printer:         ${MAX_PRT}])
else
	AC_MSG_RESULT([     Max Printer:         <default>])
fi

  AC_MSG_RESULT([     *********************************** ])

AC_MSG_RESULT([ Ready. ])
AC_MSG_RESULT([ Now use "make" to built the package and "make install" afterwards.])
AC_MSG_RESULT([ If compilation fails, try "make clean" before "make"])
AC_MSG_RESULT([ or fiddle around with "--enable-gtk1" and "--enable-oldfile"])


