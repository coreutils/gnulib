# Makefile for gnulib doc.
# Copyright (C) 2004, 2006-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

MAKEINFO = env LANG= LC_MESSAGES= LC_ALL= LANGUAGE= makeinfo
TEXI2HTML = $(MAKEINFO) --no-split --reference-limit=2000 --html

%.info: %.texi
	LANG= LC_MESSAGES= LC_ALL= LANGUAGE= makeinfo --no-split --reference-limit=2000 $<

%.html: %.texi
	$(TEXI2HTML) -o $@ $<

%.dvi: %.texi
	texi2dvi $< || { if texi2dvi $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file, often located under /etc/texmf/.'; fi; exit 1; }

%.pdf: %.texi
	texi2pdf $< || { if texi2pdf $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file, often located under /etc/texmf/.'; fi; exit 1; }

all: info html dvi
info: $(doc).info
html: $(doc).html
dvi: $(doc).dvi
pdf: $(doc).pdf

# INSTALL file with old ASCII quotation marks (looks ugly on modern machines).
INSTALL: install.texi
	$(MAKEINFO) --plaintext --no-validate $< > $@

# INSTALL file with ISO 646 or ISO 8859 quotation marks.
INSTALL.ISO: install.texi
	$(MAKEINFO) --plaintext --no-validate $< | sed -e "s/\`\([^']*\)'/'\1'/g" > $@

# INSTALL file wit ISO 10646 (Unicode) quotation marks.
INSTALL.UTF-8: install.texi
	$(MAKEINFO) --plaintext --no-validate $< | sed -e "s/\`\([^']*\)'/‘\1’/g" > $@

GNULIB_TEXI_FILES = $(filter-out maintain.texi make-stds.texi standards.texi,$(wildcard *.texi)) $(wildcard posix-headers/*.texi) $(wildcard posix-functions/*.texi) $(wildcard glibc-headers/*.texi) $(wildcard glibc-functions/*.texi)

# Date of last update. Requires GNU date.
updated-stamp: $(GNULIB_TEXI_FILES)
	date -u -r `ls -t $^ | sed 1q` +"@set UPDATED %F %T" > $@

# Dependencies. Search for @include to find them all.
gnulib.info gnulib.html gnulib.dvi gnulib.pdf: $(GNULIB_TEXI_FILES) updated-stamp
standards.info standards.html standards.dvi: standards.texi make-stds.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cp *.cps *.fn *.ky *.log *.pg *.toc *.tp *.vr *.vrs
	rm -f updated-stamp

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf
