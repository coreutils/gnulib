# This file is in the public domain.
#
# Makefile configuration - processed by automake.

# General automake options.
AUTOMAKE_OPTIONS = foreign no-dependencies subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I gnulib-m4

# The list of subdirectories containing Makefiles.
SUBDIRS = . tests

AM_CPPFLAGS = -Ilib -I$(srcdir)/lib

# Additional files to be distributed.
EXTRA_DIST = autogen.sh autoclean.sh gnulib-m4/gnulib-cache.m4

BUILT_SOURCES =
SUFFIXES =
MOSTLYCLEANFILES =
MOSTLYCLEANDIRS =
CLEANFILES =
DISTCLEANFILES =
MAINTAINERCLEANFILES =

noinst_LIBRARIES =

include lib/gnulib.mk
include src/local.mk
