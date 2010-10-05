# Makefile for gnulib central.
# Copyright (C) 2006, 2009, 2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# Produce some files that are not stored in the repository.
all:

# Produce the documentation in readable form.
info html dvi pdf:
	cd doc && $(MAKE) $@ && $(MAKE) mostlyclean

# Perform some platform independent checks on the gnulib code.
check: \
  sc_prohibit_augmenting_PATH_via_TESTS_ENVIRONMENT			\
  sc_pragma_columns							\
  sc_prefer_ac_check_funcs_once

sc_prefer_ac_check_funcs_once:
	if test -d .git; then						\
          git grep -w -l AC_CHECK_FUNCS modules				\
            && { echo use AC_CHECK_FUNCS_ONCE, not AC_CHECK_FUNCS	\
                   in modules/ 1>&2; exit 1; } || :			\
	else :; fi

sc_prohibit_augmenting_PATH_via_TESTS_ENVIRONMENT:
	if test -d .git; then						\
          url=http://thread.gmane.org/gmane.comp.lib.gnulib.bugs/22874;	\
	  git grep '^[	 ]*TESTS_ENVIRONMENT += PATH=' modules		\
	    && { printf '%s\n' 'Do not augment PATH via TESTS_ENVIRONMENT;' \
		 "  see <$$url>" 1>&2; exit 1; } || :			\
	else :; fi

sc_pragma_columns:
	if test -d .git; then						\
          git ls-files|grep '\.in\.h$$'					\
              | xargs grep -l '^@PRAGMA_SYSTEM_HEADER@'			\
              | xargs grep -L '^@PRAGMA_COLUMNS@'			\
              | grep .							\
	    && { printf '%s\n'						\
                   'the files listed above use @PRAGMA_SYSTEM_HEADER@'	\
                   'without also using @PRAGMA_COLUMNS@' 1>&2;		\
		 exit 1; } || :;					\
	else :; fi

# Regenerate some files that are stored in the repository.
regen: MODULES.html

# MODULES.html is periodically being generated and copied to the web pages at
# :ext:USER@cvs.savannah.gnu.org:/web/gnulib/gnulib/
# where it then appears at <http://www.gnu.org/software/gnulib/MODULES.html>.
MODULES.html: MODULES.html.sh
	./MODULES.html.sh > MODULES.html
