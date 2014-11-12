#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([app-installers], [1.0.0], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/options.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])


# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_CC_C_O
AM_PROG_AR

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/mount.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset mkdir realpath rmdir strcasecmp strdup strerror strncasecmp strrchr])

PKG_CHECK_MODULES([EXTDEPS], [pkgmgr pkgmgr-parser pkgmgr-info pkgmgr-installer libxml-2.0])

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
