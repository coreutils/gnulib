# Makefile for gnulib central.
# Copyright (C) 2006, 2009 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# Produce some files that are not stored in the repository.
all:

# Produce the documentation in readable form.
info:
	cd doc && $(MAKE) info
html:
	cd doc && $(MAKE) html
dvi:
	cd doc && $(MAKE) dvi && $(MAKE) mostlyclean
pdf:
	cd doc && $(MAKE) pdf && $(MAKE) mostlyclean

# Perform some platform independent checks on the gnulib code.
check:

# Regenerate some files that are stored in the repository.
regen: MODULES.html

# MODULES.html is periodically being generated and copied to the web pages at
# :ext:USER@cvs.savannah.gnu.org:/web/gnulib/gnulib/
# where it then appears at <http://www.gnu.org/software/gnulib/MODULES.html>.
MODULES.html: MODULES.html.sh
	./MODULES.html.sh > MODULES.html
