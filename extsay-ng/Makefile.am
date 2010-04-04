
if INSTALL_MODULE_EXTSAYNG

pkglib_LTLIBRARIES = libextsay.la
libextsay_la_SOURCES = extsay.c
libextsay_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
