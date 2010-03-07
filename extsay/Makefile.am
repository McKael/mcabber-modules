
if INSTALL_HEADERS
pkglib_LTLIBRARIES = libextsay.la
libextsay_la_SOURCES = extsay.c
libextsay_la_LDFLAGS = -module -avoid-version -shared

LDADD = $(GLIB_LIBS)
AM_CPPFLAGS = -I$(top_srcdir) $(GLIB_CFLAGS) $(LOUDMOUTH_CFLAGS) \
				$(GPGME_CFLAGS) $(LIBOTR_CFLAGS) \
				$(ENCHANT_CFLAGS)

endif
