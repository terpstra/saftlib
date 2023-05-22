dnl Set these variables before each release:
m4_define(MAJOR,3)    dnl Increment if removed/changed properties/signals/methods since previous release
m4_define(MINOR,0)    dnl Increment if added properties/signals/methods; reset to 0 if MAJOR changed
m4_define(REVISION,0) dnl Increment on each release; reset to 0 if MAJOR/MINOR changed
m4_define(SONAME,10)   dnl Whenever MAJOR is incremented, add MINOR+1 to this variable


AC_INIT([saftlib], [MAJOR.MINOR.REVISION])
LT_INIT
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])

PKG_CHECK_MODULES([EB],      [etherbone >= 2.1.0])
PKG_CHECK_MODULES([SIGCPP],  [sigc++-2.0])
AC_CHECK_PROG( SAFTBUSGEN_CHECK,saftbus-gen,yes)
AM_CONDITIONAL([NO_SAFTBUSGEN], [test x"$SAFTBUSGEN_CHECK" != x"yes"])

#####################################
# the following is needed to extract build info (relevant for src/build.cpp src/build.hpp version.h)
#####################################
AC_CACHE_CHECK([for operating system], [ac_cv_OS], [os=`cat /etc/os-release | grep NAME | head -n 1 | cut -d"=" -f2`; os="${os%\"}"; ac_cv_OS="${os#\"}" ])
AC_DEFINE_UNQUOTED([OPERATING_SYSTEM], ["$ac_cv_OS"], [The operating system of the build system])
AC_DEFINE_UNQUOTED([HOSTNAME], ["${HOSTNAME:-unknown}"], [The hostname of the build system])
AC_DEFINE_UNQUOTED([USERNAME], ["${USERNAME:-${LOGNAME:-${USER:-unknown}}}"], [The username of the person compiling saftlib])
AC_PATH_PROGS([GIT], [git])
AM_CONDITIONAL([GIT_TREE], [test x$GIT != x -a -e .git])
#####################################
# end of build info stuff
#####################################


AC_SUBST([SAFTD_MAJOR], MAJOR)
AC_SUBST([SAFTD_MINOR], MINOR)
AC_SUBST([SAFTD_REVISION], REVISION)
AC_SUBST([SAFTD_API], [$((SONAME+MINOR))]) dnl API incremented on every change
AC_SUBST([SAFTD_SONAME], SONAME)

AC_CONFIG_FILES([Makefile saftlib3.pc saftbus.service])
AC_OUTPUT