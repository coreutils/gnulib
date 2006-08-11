# $Id: Makefile,v 1.4 2006-08-11 06:39:41 rwild Exp $
# Makefile for gnulib doc.  
# Copyright (C) 2004, 2006 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

TEXI2HTML = $(MAKEINFO) --no-split --html
%.html: %.texi
	$(TEXI2HTML) -o $@ $< 

all: info html dvi
info: $(doc).info
html: $(doc).html
dvi: $(doc).dvi

standards.info standards.html standards.dvi: standards.texi make-stds.texi
