
if INSTALL_MODULE_CLOCK

pkglib_LTLIBRARIES = libclock.la
libclock_la_SOURCES = clock.c
libclock_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS) $(MCABBER_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(MCABBER_CFLAGS)

endif
