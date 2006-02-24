# $Id: Makefile,v 1.2 2006-02-24 00:19:55 karl Exp $
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
