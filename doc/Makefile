# Makefile for gnulib doc.  -*- coding: utf-8 -*-
# Copyright (C) 2004, 2006-2025 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

lang_env = env LANG= LC_MESSAGES= LC_ALL= LANGUAGE=
makeinfo_prog = makeinfo
# ASCII_DASHES_AND_QUOTES=0 is needed for makeinfo versions ≥ 7.1.
# CHECK_NORMAL_MENU_STRUCTURE=1 is needed for makeinfo versions ≥ 6.8.
MAKEINFO = $(lang_env) $(makeinfo_prog) \
 -c ASCII_DASHES_AND_QUOTES=0 \
 -c CHECK_NORMAL_MENU_STRUCTURE=1

manual_opts = --no-split --reference-limit=2000
TEXI2HTML = $(MAKEINFO) $(manual_opts) --html

# Needed so that tex finds a recent enough texinfo.tex.
tex_opts = -I ../build-aux

undocumented-modules.texi: force
	grep -h '^@mindex ' `find . -name '*.texi'` | sed -e 's/@mindex //' | LC_ALL=C sort -u > documented-modules.tmp
	(cd .. && ./gnulib-tool --list) | LC_ALL=C sort -u > all-modules.tmp
	LC_ALL=C join -v1 all-modules.tmp documented-modules.tmp > undocumented-modules.tmp
# For testing whether a module is deprecated, "./gnulib-tool --extract-status <module>" is a bit slow, so use grep instead.
	modules=`cat undocumented-modules.tmp`; (cd ../modules && grep -l '^deprecated$$' $$modules) > deprecated-undocumented-modules.tmp
	LC_ALL=C join -v1 undocumented-modules.tmp deprecated-undocumented-modules.tmp > undocumented-modules-2.tmp
	LC_ALL=C join -v1 undocumented-modules-2.tmp internal-modules > undocumented-modules-3.tmp
	sed -e 's/^/@umindex /' < undocumented-modules-3.tmp > undocumented-modules.texi
	rm -f documented-modules.tmp all-modules.tmp undocumented-modules.tmp deprecated-undocumented-modules.tmp undocumented-modules-2.tmp undocumented-modules-3.tmp

%.info: %.texi undocumented-modules.texi
	$(MAKEINFO) $(manual_opts) $<

%.html: %.texi undocumented-modules.texi
	$(TEXI2HTML) -o $@ $<

%.dvi: %.texi undocumented-modules.texi
	texi2dvi $(tex_opts) $< || { if texi2dvi $(tex_opts) $< 2>/dev/null | grep 'TeX capacity exceeded, sorry \[save size=' > /dev/null; then echo '*** You must increase the save_size in your texmf.cnf file, often located under /etc/texmf/.'; fi; exit 1; }

%.pdf: %.texi undocumented-modules.texi
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
standards.info standards.html standards.dvi standards.pdf: standards.texi make-stds.texi fdl.texi
maintain.info maintain.html maintain.dvi maintain.pdf: maintain.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cn *.cp *.cps *.fn *.ky *.log *.m *.pg *.toc *.tmp *.tp *.vr *.vrs
	rm -f updated-stamp

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf

force:
