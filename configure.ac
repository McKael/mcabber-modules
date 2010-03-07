#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([mcabber-modules],[0.10.0-dev],[mcabber@lilotux.net])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config-modules.h])

AC_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

if test "x$GCC" = "xyes"; then
    dnl Get gcc version
    AC_MSG_CHECKING([gcc version])
        gccver=$($CC -dumpversion)
        gccvermajor=$(echo $gccver | cut -d . -f1)
        gccverminor=$(echo $gccver | cut -d . -f2)
        gccvernum=$(expr $gccvermajor "*" 100 + $gccverminor)
    AC_MSG_RESULT($gccver)

    CFLAGS="$CFLAGS -Wall"
    if test x"$gccvermajor" = x"4" -a x"$gccverminor" = x"2"; then
        DBGCFLAGS="-fgnu89-inline"
    fi
fi

AC_SYS_LARGEFILE

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h locale.h netdb.h netinet/in.h stddef.h \
                  stdlib.h string.h strings.h sys/socket.h sys/time.h \
                  syslog.h termios.h wchar.h wctype.h localcharset.h])
AC_CHECK_HEADERS([unistd.h], , AC_MSG_ERROR([Missing header file]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm arc4random bzero gethostbyname gethostname inet_ntoa \
                isascii memmove memset modf select setlocale socket strcasecmp \
                strchr strdup strncasecmp strrchr strstr strcasestr vsnprintf \
                iswblank])


# Check for glib
AM_PATH_GLIB_2_0(2.14.0,
                 [AC_DEFINE([HAVE_GLIB_REGEX], 1,
                            [Define if GLib has regex support])],
                 [AM_PATH_GLIB_2_0(2.0.0, , AC_MSG_ERROR([glib is required]),
                                  [g_list_append], ["$gmodule_module"])],
                 [g_regex_new "$gmodule_module"])

# Export $libexecdir to the source tree
AC_DEFINE_DIR(PKGLIB_DIR, "${libdir}/${PACKAGE}", [Modules directory])

AC_ARG_ENABLE(debug,
    [AC_HELP_STRING(--enable-debug, add development compilation options)],
    debug=$enableval, debug="")
if test x"${debug}" = x"yes"; then
    AC_DEFINE_UNQUOTED([ENABLE_DEBUG],[1],[Devel compilation options])
    if test "x$GCC" = "xyes"; then
        if test "$gccvernum" -ge "400"; then
            CFLAGS="$CFLAGS -Wextra"
        else
            CFLAGS="$CFLAGS -W"
        fi
        CFLAGS="$CFLAGS -Wno-unused-parameter -pedantic -std=gnu99 $DBGCFLAGS -O0"
    fi # gcc
    CFLAGS="$CFLAGS -g"
else
    CFLAGS="-O2 $CFLAGS"
fi

# We need _GNU_SOURCE for strptime() and strcasestr()
CFLAGS="$CFLAGS -D_GNU_SOURCE"

AC_CONFIG_FILES([comment/Makefile
                 extsay/Makefile
                 Makefile])
AC_OUTPUT
