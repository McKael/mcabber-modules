
if INSTALL_MODULE_KILLPRESENCE

pkglib_LTLIBRARIES = libkillpresence.la
libkillpresence_la_SOURCES = killpresence.c
libkillpresence_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
