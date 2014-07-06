
if INSTALL_MODULE_SHOW_MDR

pkglib_LTLIBRARIES = libshow_mdr.la
libshow_mdr_la_SOURCES = show_mdr.c
libshow_mdr_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
