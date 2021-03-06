#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(CROPSQUARE, 0.1.0, support@toonloop.com)
#AC_DEFINE([PACKAGE_URL],["http://www.toonloop.com"])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4]) # check for m4 macros in directory m4
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE()

# Check for C and C++ compilers
AC_PROG_CXX
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
PKG_PROG_PKG_CONFIG

AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS([inet_ntoa memset socket])

AC_CHECK_LIB([pthread],[pthread_exit],[AC_SUBST([LIBTHREAD_CXXFLAGS],["-lpthread"])])
AC_CHECK_LIB([m],[sin],[AC_SUBST([LIBM_CXXFLAGS],["-lm"])])

# OpenCV
PKG_CHECK_MODULES([OPENCV], [opencv >= 0.22])
if test "x${have_opencv}" = "xfalse" ; then
    AC_MSG_ERROR([Missing opencv])
fi

AC_SUBST([OPENCV_LIBS])
AC_SUBST([OPENCV_CFLAGS])

# BOOST
AX_BOOST_BASE([1.35])

AX_BOOST_PROGRAM_OPTIONS
if test "x${ax_cv_boost_program_options}" = "xno" ; then
	AC_MSG_ERROR([missing package: libboost-program-options-dev])
fi

AX_BOOST_FILESYSTEM
if test "x${ax_cv_boost_filesystem}" = "xno" ; then
	AC_MSG_ERROR([missing package: libboost-filesystem-dev])
fi

AX_BOOST_SYSTEM
if test "x${ax_cv_boost_system}" = "xno" ; then
	AC_MSG_ERROR([missing package: libboost-system-dev])
fi

# GNU help2man creates man pages from --help output; in many cases, this
# is sufficient, and obviates the need to maintain man pages separately.
# However, this means invoking executables, which we generally cannot do
# when cross-compiling, so we test to avoid that (the variable
# "cross_compiling" is set by AC_PROG_CC).
if test $cross_compiling = no; then
    AM_MISSING_PROG(HELP2MAN, help2man)
else
    HELP2MAN=:
fi

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    man/Makefile
    ])
AC_OUTPUT

