
if INSTALL_MODULE_IGNORE_AUTH

pkglib_LTLIBRARIES = libignore_auth.la
libignore_auth_la_SOURCES = ignore_auth.c
libignore_auth_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
