# Makefile for gnulib doc.  -*- coding: utf-8 -*-
# Copyright (C) 2004, 2006-2024 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

lang_env = env LANG= LC_MESSAGES= LC_ALL= LANGUAGE=
makeinfo_prog = makeinfo
# The customization variable CHECK_NORMAL_MENU_STRUCTURE is necessary with
# makeinfo versions ≥ 6.8.
MAKEINFO = $(lang_env) $(makeinfo_prog) -c CHECK_NORMAL_MENU_STRUCTURE=1

manual_opts = --no-split --reference-limit=2000
TEXI2HTML = $(MAKEINFO) $(manual_opts) --html

# Needed so that tex finds a recent enough texinfo.tex.
tex_opts = -I ../build-aux

%.info: %.texi
	$(MAKEINFO) $(manual_opts) $<

%.html: %.texi
	$(TEXI2HTML) -o $@ $<

%.dvi: %.texi
	texi2dvi $(tex_opts) $< || { if texi2dvi $(tex_opts) $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file, often located under /etc/texmf/.'; fi; exit 1; }

%.pdf: %.texi
	texi2pdf $(tex_opts) $< || { if texi2pdf $(tex_opts) $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file, often located under /etc/texmf/.'; fi; exit 1; }

all: info html dvi
info: $(doc).info
html: $(doc).html
dvi: $(doc).dvi
pdf: $(doc).pdf

INSTALL_PRELUDE = echo '@firstparagraphindent insert'

OMIT_TRAILING_EMPTY_LINES = /^$$/H; /^$$/d; x; s/\n//p; s/\n*//; x

# INSTALL file with UTF-8 encoding.
INSTALL: install.texi
	$(INSTALL_PRELUDE) | cat - $< > tmp.texi
	$(MAKEINFO) --plaintext --no-validate tmp.texi \
	  | sed -e '$(OMIT_TRAILING_EMPTY_LINES)' > $@
	rm -f tmp.texi

# INSTALL file with ASCII encoding.
INSTALL.ISO: INSTALL
	sed \
          -e 's/©/(C)/g' \
          -e 's/–/-/g' \
          -e 's/‘/'\''/g' \
          -e 's/’/'\''/g' \
          -e 's/“/"/g' \
          -e 's/”/"/g' \
          $< > $@

# INSTALL file with UTF-8 encoding.
# This file is obsolescent, as INSTALL now uses UTF-8.
INSTALL.UTF-8: INSTALL
	cp $< $@

GNULIB_TEXI_FILES = $(filter-out maintain.texi make-stds.texi standards.texi,$(wildcard *.texi)) $(wildcard posix-headers/*.texi) $(wildcard posix-functions/*.texi) $(wildcard glibc-headers/*.texi) $(wildcard glibc-functions/*.texi)
NEWEST_GNULIB_TEXI_FILE = $(shell ls -t $(GNULIB_TEXI_FILES) | sed 1q)

# Date of last update. Requires GNU date.
updated-stamp: $(GNULIB_TEXI_FILES)
	date -u -r $(NEWEST_GNULIB_TEXI_FILE) +"@set UPDATED %F %T" > $@

# Dependencies. Search for @include to find them all.
gnulib.info gnulib.html gnulib.dvi gnulib.pdf: $(GNULIB_TEXI_FILES) updated-stamp
standards.info standards.html standards.dvi: standards.texi make-stds.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cn *.cp *.cps *.fn *.ky *.log *.pg *.toc *.tp *.vr *.vrs
	rm -f updated-stamp

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf
