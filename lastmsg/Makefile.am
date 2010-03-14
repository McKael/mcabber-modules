
if INSTALL_MODULE_LASTMSG

pkglib_LTLIBRARIES = liblastmsg.la
liblastmsg_la_SOURCES = lastmsg.c
liblastmsg_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
