AC_INIT([senmqd],[0.1])
AC_PREREQ([2.64])
AC_CONFIG_SRCDIR([./src/main.cc])
AC_CONFIG_HEADERS([./src/config.h])
AC_GNU_SOURCE
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([1.11  foreign  -Wall no-define subdir-objects \
                  color-tests \
                  silent-rules \
                  parallel-tests])

LT_INIT
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX

PKG_CHECK_MODULES([ZMQSTUFF],[libzmq >= 4.0.4])

AC_OUTPUT([
Makefile
])



