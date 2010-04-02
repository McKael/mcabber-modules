
if INSTALL_MODULE_EXTSAY_OLD

pkglib_LTLIBRARIES = libextsay_old.la
libextsay_old_la_SOURCES = extsay_old.c
libextsay_old_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
