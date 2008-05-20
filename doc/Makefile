# $Id: Makefile,v 1.8 2007-07-06 22:56:38 haible Exp $
# Makefile for gnulib doc.
# Copyright (C) 2004, 2006-2008 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

TEXI2HTML = $(MAKEINFO) --no-split --reference-limit=2000 --html
%.html: %.texi
	$(TEXI2HTML) -o $@ $<

%.info: %.texi
	LANG= LC_MESSAGES= LC_ALL= LANGUAGE= makeinfo --no-split --reference-limit=2000 $<

%.pdf: %.texi
	texi2pdf $< || { if texi2pdf $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file.'; fi; exit 1; }

all: info html dvi
info: $(doc).info
html: $(doc).html
dvi: $(doc).dvi
pdf: $(doc).pdf

# Dependencies. Search for @include to find them all.
gnulib.info gnulib.html gnulib.dvi gnulib.pdf: gnulib.texi gnulib-intro.texi gnulib-tool.texi $(wildcard headers/*.texi) $(wildcard functions/*.texi) quote.texi error.texi gcd.texi relocatable-maint.texi regexprops-generic.texi fdl.texi
standards.info standards.html standards.dvi: standards.texi make-stds.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cp *.cps *.fn *.ky *.log *.pg *.toc *.tp *.vr *.vrs

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf
