# $Id: Makefile,v 1.6 2007-04-27 19:58:36 haible Exp $
# Makefile for gnulib doc.  
# Copyright (C) 2004, 2006-2007 Free Software Foundation, Inc.
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

# Dependencies. Search for @include to find them all.
gnulib.info gnulib.html gnulib.dvi: gnulib.texi gnulib-intro.texi gnulib-tool.texi $(wildcard headers/*.texi) quote.texi error.texi ctime.texi gcd.texi inet_ntoa.texi relocatable-maint.texi regexprops-generic.texi fdl.texi
standards.info standards.html standards.dvi: standards.texi make-stds.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cp *.cps *.fn *.ky *.log *.pg *.toc *.tp *.vr *.vrs

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf
