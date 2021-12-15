# This file is in the public domain.
#
# Makefile configuration - processed by automake.

# General automake options.
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I m4 -I gnulib-m4

# The list of subdirectories containing Makefiles.
SUBDIRS = m4 lib tests

AM_CPPFLAGS = -Ilib -I$(srcdir)/lib

# The list of programs that are built.
bin_PROGRAMS = hello

# The source files of the 'hello' program.
hello_SOURCES = hello.c
hello_LDADD = lib/libgnu.a

# Additional files to be distributed.
EXTRA_DIST = autogen.sh autoclean.sh gnulib-m4/gnulib-cache.m4
