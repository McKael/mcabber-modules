
if INSTALL_MODULE_COMMENT

pkglib_LTLIBRARIES = libcomment.la
libcomment_la_SOURCES = comment.c
libcomment_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
