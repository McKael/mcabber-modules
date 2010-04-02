
if INSTALL_MODULE_EXTSAYNG

pkglib_LTLIBRARIES = libextsayng.la
libextsayng_la_SOURCES = extsayng.c
libextsayng_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
