# $Id: Makefile,v 1.1 2004-09-19 13:17:06 karl Exp $
# Makefile for gnulib doc.  
# Copyright (C) 2004 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

doc = gnulib

TEXI2HTML = $(MAKEINFO) --no-split --html
%.html: %.texi
	$(TEXI2HTML) -o $@ $< 

all: $(doc).info $(doc).html $(doc).dvi 
