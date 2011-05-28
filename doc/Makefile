# Makefile for gnulib doc.
# Copyright (C) 2004, 2006-2011 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

lang_env = env LANG= LC_MESSAGES= LC_ALL= LANGUAGE=
makeinfo_prog = makeinfo
MAKEINFO = $(lang_env) $(makeinfo_prog)

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

# INSTALL file with old ASCII quotation marks (looks ugly on modern machines).
INSTALL: install.texi
	$(INSTALL_PRELUDE) | cat - $< > tmp.texi
	$(MAKEINFO) --plaintext --no-validate tmp.texi > $@
	rm -f tmp.texi

# INSTALL file with ISO 646 or ISO 8859 quotation marks.
INSTALL.ISO: install.texi
	$(INSTALL_PRELUDE) | cat - $< > tmpi.texi
	$(MAKEINFO) --plaintext --no-validate tmpi.texi \
	  | sed -e "s/\`\([^']*\)'/'\1'/g" > $@
	rm -f tmpi.texi

# INSTALL file wit ISO 10646 (Unicode) quotation marks.
INSTALL.UTF-8: install.texi
	$(INSTALL_PRELUDE) | cat - $< > tmpu.texi
	$(MAKEINFO) --plaintext --no-validate tmpu.texi \
	  | sed -e "s/\`\([^']*\)'/‘\1’/g" > $@
	rm -f tmpu.texi

GNULIB_TEXI_FILES = $(filter-out maintain.texi make-stds.texi standards.texi,$(wildcard *.texi)) $(wildcard posix-headers/*.texi) $(wildcard posix-functions/*.texi) $(wildcard glibc-headers/*.texi) $(wildcard glibc-functions/*.texi)

# Date of last update. Requires GNU date.
updated-stamp: $(GNULIB_TEXI_FILES)
	date -u -r `ls -t $^ | sed 1q` +"@set UPDATED %F %T" > $@

# Dependencies. Search for @include to find them all.
gnulib.info gnulib.html gnulib.dvi gnulib.pdf: $(GNULIB_TEXI_FILES) updated-stamp
standards.info standards.html standards.dvi: standards.texi make-stds.texi fdl.texi

mostlyclean:
	rm -f *.aux *.cn *.cp *.cps *.fn *.ky *.log *.pg *.toc *.tp *.vr *.vrs
	rm -f updated-stamp

clean: mostlyclean
	rm -f *.info *.html *.dvi *.ps *.pdf
