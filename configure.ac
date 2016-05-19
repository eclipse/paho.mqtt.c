#*******************************************************************************
#  Copyright (c) 2016
# 
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution. 
# 
#  The Eclipse Public License is available at 
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at 
#    http://www.eclipse.org/org/documents/edl-v10.php.
# 
#  Contributors:
#     Guilherme Maciel Ferreira - initial implementation
#*******************************************************************************/

###############################################################################
# Autotools setup
###############################################################################

AC_PREREQ([2.65])
AC_INIT([paho.mqtt.c], [1.1.0])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG_C

AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign subdir-objects])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AM_SILENT_RULES([yes])

LT_INIT([shared static pic-only])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_SED

###############################################################################
# Input dependencies
###############################################################################

# dl
AC_SEARCH_LIBS([dlopen], [dl], , AC_MSG_ERROR([missing dl library]))

# Pthread
AC_CHECK_LIB([pthread], [pthread_create], , AC_MSG_ERROR([missing pthread library]))
AC_CHECK_HEADER([pthread.h], , AC_MSG_ERROR([missing pthread.h header]))

# Socket
AC_CHECK_HEADER([sys/socket.h], , AC_MSG_ERROR([missing sys/socket.h header]))

AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([sys/param.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([sys/timeb.h])
AC_CHECK_HEADERS([syslog.h])
AC_HEADER_STDBOOL

AC_TYPE_SIZE_T

AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime])
AC_CHECK_FUNCS([ftime])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([rmdir])
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([setenv])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strstr])

###############################################################################
# Input options
###############################################################################

# Build synchronous library
AC_ARG_ENABLE(
	[sync],
	AS_HELP_STRING(
		[--enable-sync=@<:@yes/no@:>@],
		[build synchronous library @<:@default=yes@:>@]
	),
	,
	enable_sync=yes
)

AM_CONDITIONAL([PAHO_BUILD_SYNC], [test "$enable_sync" = yes])


# Build asynchronous library
AC_ARG_ENABLE(
	[async],
	AS_HELP_STRING(
		[--enable-async=@<:@yes/no@:>@],
		[build asynchronous library @<:@default=yes@:>@]
	),
	,
	enable_async=yes
)

AM_CONDITIONAL([PAHO_BUILD_ASYNC], [test "$enable_async" = yes])


AS_IF([test "x$enable_sync" != "xyes" && test "x$enable_async" != "xyes"], [
	AC_MSG_ERROR([you must build synchronous, asynchronous or both libraries])
])

# Build samples
AC_ARG_ENABLE(
	[samples],
	AS_HELP_STRING(
		[--enable-samples=@<:@yes/no@:>@],
		[build sample programs @<:@default=no@:>@]
	),
	,
	enable_samples=no
)

AS_IF([test "x$enable_samples" = "xyes"], [
	AC_CONFIG_FILES([src/samples/Makefile])
])

AM_CONDITIONAL([PAHO_BUILD_SAMPLES], [test "$enable_samples" = yes])


# Build test
AC_ARG_ENABLE(
	[test],
	AS_HELP_STRING(
		[--enable-test=@<:@yes/no@:>@],
		[build test programs @<:@default=no@:>@]
	),
	,
	enable_test=no
)

AS_IF([test "x$enable_test" = "xyes"], [
	AC_CONFIG_FILES([test/Makefile])
])

AM_CONDITIONAL([PAHO_BUILD_TEST], [test "$enable_test" = yes])


# Build documentation
AC_ARG_ENABLE(
	[doc],
	AS_HELP_STRING(
		[--enable-doc=@<:@yes/no@:>@],
		[build documentation @<:@default=no@:>@]
	),
	,
	enable_doc=no
)

AS_IF([test "x$enable_doc" = "xyes"], [
	AC_CHECK_PROGS([DOXYGEN], [doxygen])

	AS_IF([test -z "$DOXYGEN"], [
		AC_MSG_ERROR([Doxygen not found - disabling documentation build])
	], [
		AC_CONFIG_FILES([doc/DoxyfileV3ClientAPI])
		AC_CONFIG_FILES([doc/DoxyfileV3AsyncAPI])
		AC_CONFIG_FILES([doc/DoxyfileV3ClientInternal])
	])

])

AM_CONDITIONAL([PAHO_BUILD_DOC], [test "$enable_doc" = yes && test -n "$DOXYGEN"])


# Debug
AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING(
		[--enable-debug=@<:@yes/no@:>@],
		[build libraries and binaries with debugging symbols @<:@default=no@:>@]
	),
	,
	enable_debug=no
)

AS_IF([test "x$enable_debug" = "xyes"], [
	CFLAGS+=" -g"
])


# Peak warnings
AC_ARG_ENABLE(
	[peak_warnings],
	AS_HELP_STRING(
		[--enable-peak-warnings=@<:@yes/no@:>@],
		[enable peak warnings level @<:@default=no@:>@]
	),
	,
	enable_peak_warnings=no
)

AS_IF([test "x$enable_peak_warnings" = "xyes"], [
	CFLAGS+=" -Wabi"
	CFLAGS+=" -Wall"
	CFLAGS+=" -Wcast-qual"
	CFLAGS+=" -Wconversion"
	CFLAGS+=" -Wextra"
	CFLAGS+=" -Wfloat-equal"
	CFLAGS+=" -Winline"
	CFLAGS+=" -Wmissing-declarations"
	CFLAGS+=" -Wmissing-format-attribute"
	CFLAGS+=" -Wno-deprecated-declarations"
	CFLAGS+=" -Wpacked"
	CFLAGS+=" -Wredundant-decls"
	CFLAGS+=" -Wshadow"
	CFLAGS+=" -Wstack-protector -fstack-protector"
	CFLAGS+=" -Wswitch-default"
	CFLAGS+=" -fno-common"
])


# Add OpenSSL support to build secure PAHO libraries
AC_ARG_WITH(
	[ssl],
	AS_HELP_STRING(
		[--with-ssl=@<:@yes/no@:>@],
		[with OpenSSL support @<:@default=no@:>@]
	),
	,
	with_ssl=no
)

AS_IF([test "x$with_ssl" = "xyes"], [
	AC_CHECK_LIB([ssl], [SSL_connect], , AC_MSG_ERROR([missing ssl library]))
	AC_CHECK_HEADER([openssl/ssl.h], , AC_MSG_ERROR([missing openssl/ssl.h header]))

	AC_CHECK_LIB([crypto], [CRYPTO_num_locks], , AC_MSG_ERROR([missing crypto library]))
	AC_CHECK_HEADER([openssl/crypto.h], , AC_MSG_ERROR([missing openssl/crypto.h header]))
])

AM_CONDITIONAL([PAHO_WITH_SSL], [test "$with_ssl" = yes])


###############################################################################
# Output files
###############################################################################

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([doc/Makefile])

AC_OUTPUT

###############################################################################
# Output message
###############################################################################

echo ""
echo "Paho MQTT C has been configured with the following options:"
echo "            Build as shared library : $enable_shared"
echo "            Build as static library : $enable_static"
echo "          Build synchronous library : $enable_sync"
echo "         Build asynchronous library : $enable_async"
echo "                      Build samples : $enable_samples"
echo "                         Build test : $enable_test"
echo "                Build documentation : $enable_doc"
echo "                   Enable debugging : $enable_debug"
echo "          Enable peak warning level : $enable_peak_warnings"
echo "               With OpenSSL library : $with_ssl"
echo "                      With CPPFLAGS : $CPPFLAGS"
echo "                        With CFLAGS : $CFLAGS"
echo "                       With LDFLAGS : $LDFLAGS"
echo "                     With libraries : $LIBS"
