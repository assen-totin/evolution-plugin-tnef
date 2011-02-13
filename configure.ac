#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.66])
AC_INIT([tnef-plugin], [0.0.3], [assen@online.bg])
AC_CONFIG_SRCDIR([config.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Check for packages
PKG_CHECK_MODULES([EVOLUTION_MAIL], [evolution-mail = 2.32.0])
PKG_CHECK_MODULES([GTKHTML_EDITOR_3_14], [gtkhtml-editor-3.14 >= 3.32])

EVOLUTION_FULL_VERSION=`pkg-config --modversion evolution-mail`
EVOLUTION_SHORT_VERSION=`echo -n "$EVOLUTION_FULL_VERSION" | awk '{split ($0, a, "."); printf "%s.%s",a[[1]],a[[2]]}'`

AC_DEFINE_UNQUOTED([BASE_VERSION], ["$EVOLUTION_SHORT_VERSION"])
AC_DEFINE_UNQUOTED([DATASERVER_VERSION], ["$EVOLUTION_FULL_VERSION"])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION], ["$EVOLUTION_FULL_VERSION"])
AC_DEFINE_UNQUOTED([VERSION], ["$EVOLUTION_FULL_VERSION"])
AC_DEFINE_UNQUOTED([PACKAGE_STRING], ["$EVOLUTION_FULL_VERSION"])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["evolution-$EVOLUTION_SHORT_VERSION"])
AC_DEFINE_UNQUOTED([INTERFACE_VERSION], ["$EVOLUTION_SHORT_VERSION"])
AC_DEFINE_UNQUOTED([KILL_PROCESS_CMD], ["`which killall`"])

# Create eplug file from template
echo "Preparing eplug file..."
EVOLUTION_PRIVLIBDIR=`pkg-config --variable=privlibdir evolution-mail`
EVOLUTION_PLUGIN_DIR=$EVOLUTION_PRIVLIBDIR/plugins
AC_SUBST([EVOLUTION_PLUGIN_DIR])
AC_CONFIG_FILES([org-gnome-tnef-plugin.eplug])

# Create spec file from template
echo "Preparing spec file..."
TNEF_PLUGIN_DIRECTORY=`pwd | awk '{split ($0, a, "/"); c=0; for (b in a) {if (b>c) {c=b}} printf "%s",a[[c]]}'`
TNEF_PLUGIN_VERSION=`echo -n "$TNEF_PLUGIN_DIRECTORY" | awk '{split ($0, a, "-"); printf "%s",a[[3]]}'`
AC_SUBST([TNEF_PLUGIN_VERSION])
AC_CONFIG_FILES([tnef-plugin.spec])
TNEF_PLUGIN_SPEC=tnef-plugin-$TNEF_PLUGIN_VERSION.spec

# Makefile
MAKEFILE_LIBTOOL=`which libtool`
MAKEFILE_CC=`which gcc`
AC_SUBST([MAKEFILE_LIBTOOL])
AC_SUBST([MAKEFILE_CC])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

# Rename spec file to include version number
mv -f tnef-plugin.spec $TNEF_PLUGIN_SPEC

