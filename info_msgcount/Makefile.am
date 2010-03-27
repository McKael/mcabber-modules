
if INSTALL_MODULE_INFO_MSGCOUNT

pkglib_LTLIBRARIES = libinfo_msgcount.la
libinfo_msgcount_la_SOURCES = info_msgcount.c
libinfo_msgcount_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
