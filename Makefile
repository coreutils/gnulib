# Makefile for gnulib central.
# Copyright (C) 2006 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# Produce some files that are not stored in CVS.
all:

# Perform some platform independent checks on the gnulib code.
check:

# Regenerate some files that are stored in CVS; you should commit them
# afterwards.
regen: MODULES.html

MODULES.html: MODULES.html.sh
	./MODULES.html.sh > MODULES.html
