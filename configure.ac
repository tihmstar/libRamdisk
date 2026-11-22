AC_PREREQ([2.69])
AC_INIT([libRamdisk], m4_esyscmd([git rev-list --count HEAD | tr -d '\n']), [tihmstar@gmail.com])

AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST
LT_INIT([disable-static])

AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_DEFINE([VERSION_COMMIT_COUNT], "m4_esyscmd([git rev-list --count HEAD | tr -d '\n'])", [Git commit count])
AC_DEFINE([VERSION_COMMIT_SHA], "m4_esyscmd([git rev-parse HEAD | tr -d '\n'])", [Git commit sha])
AC_SUBST([VERSION_COMMIT_COUNT], ["m4_esyscmd([git rev-list --count HEAD | tr -d '\n'])"])
AC_SUBST([VERSION_COMMIT_SHA], ["m4_esyscmd([git rev-parse HEAD | tr -d '\n'])"])

# Checks for programs.
AC_PROG_CXX([clang++])
AC_PROG_CC([clang++])

# Check for operating system
AC_MSG_CHECKING([whether we need platform-specific build settings])
case $host_os in
  darwin* )
        AC_MSG_RESULT([${host_os}])
        CXXFLAGS+=" -stdlib=libc++ "
        ;;
  *mingw32*|*cygwin*|*msys*)
        AC_MSG_RESULT([${host_os}])
        win32=true
        LDFLAGS+=" -lws2_32"
        ;;
esac

CXXFLAGS+=" -fPIC -std=c++23 -O3"
CFLAGS+=" -fPIC -std=c23"

# Checks for libraries.
LIBGENERAL_REQUIRES_STR="libgeneral >= 92"

PKG_CHECK_MODULES(libgeneral, $LIBGENERAL_REQUIRES_STR)

AC_SUBST([libgeneral_requires], [$LIBGENERAL_REQUIRES_STR])

PRIVATE_LIBS="-lcommon -ldmg -lhfs -lz"
AC_SUBST([private_libs], [$PRIVATE_LIBS])

AC_ARG_ENABLE([debug],
            [AS_HELP_STRING([--enable-debug],
            [enable debug build(default is no)])],
            [debug_build=true],
            [debug_build=false])

if test "$debug_build" = true; then
  echo "*** Note: debug build requested ***"
  CFLAGS+=" -g -O0 -DDEBUG=1"
  CXXFLAGS+=" -g -O0 -DDEBUG=1"
fi

LT_INIT

AC_CHECK_FUNCS([mkdtemp])

LDFLAGS+=" -no-undefined $PRIVATE_LIBS"

GLOBAL_CFLAGS+=" -fvisibility=hidden"
case "$GLOBAL_CFLAGS" in
	*-fvisibility=hidden*)
		AC_DEFINE([HAVE_FVISIBILITY], [1], [Define if compiled with -fvisibility=hidden])
esac

AC_SUBST(GLOBAL_CFLAGS)
AC_SUBST(GLOBAL_CXXFLAGS)
AC_SUBST(GLOBAL_LDFLAGS)

AC_CONFIG_FILES([Makefile
                include/Makefile
                libramdisk.pc
                libRamdisk/Makefile])
AC_OUTPUT

echo "
Configuration for $PACKAGE-$VERSION:
-------------------------------------------

  install prefix ..........: $prefix
  debug build  ............: $debug_build"
echo "  compiler ................: ${CC}

  Now type 'make' to build $PACKAGE-$VERSION,
  and then 'make install' for installation.
"