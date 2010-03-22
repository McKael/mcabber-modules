
if INSTALL_MODULE_XTTITLE

pkglib_LTLIBRARIES = libxttitle.la
libxttitle_la_SOURCES = xttitle.c
libxttitle_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
