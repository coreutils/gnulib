# GNU Makefile for gnulib central.
# Copyright (C) 2006, 2009-2011 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# This Makefile requires the use of GNU make.  Some targets require
# that you have tools like git, makeinfo and cppi installed.

# Produce some files that are not stored in the repository.
all:

# Produce the documentation in readable form.
info html dvi pdf:
	cd doc && $(MAKE) $@ && $(MAKE) mostlyclean

# Collect the names of rules starting with `sc_'.
syntax-check-rules := $(sort $(shell sed -n 's/^\(sc_[a-zA-Z0-9_-]*\):.*/\1/p'\
			Makefile))

# Perform some platform independent checks on the gnulib code.
check: $(syntax-check-rules)

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

# Verify that certain (for now, only Jim Meyering and Eric Blake's)
# *.c files are consistently cpp indented.
sc_cpp_indent_check:
	./gnulib-tool --extract-filelist \
            $$(cd ./modules; grep -ilrE '(meyering|blake)' .) \
          | sort -u \
          | grep '\.c$$' \
          | grep -v '/getloadavg\.c$$' \
          | xargs cppi -c

# Regenerate some files that are stored in the repository.
regen: MODULES.html

# MODULES.html is periodically being generated and copied to the web pages at
# :ext:USER@cvs.savannah.gnu.org:/web/gnulib/gnulib/
# where it then appears at <http://www.gnu.org/software/gnulib/MODULES.html>.
MODULES.html: MODULES.html.sh
	./MODULES.html.sh > MODULES.html

# Run this rule once per year (usually early in January)
# to update all FSF copyright year lists here.
# We exclude the files listed in srclist.txt (maintained elsewhere)
# as well as those in tests/unictype (generated).
# Also exclude any file that includes the "GENERATED AUTOMATICALLY" comment,
# being careful not to exclude code that merely generates the comment.
# Also exclude doc/INSTALL*, since they too are generated.
update-copyright:
	exempt=$$(mktemp);						\
	grep -v '^#' config/srclist.txt|grep -v '^$$'			\
	  | while read src dst; do					\
	      test -f "$$dst" && { echo "$$dst"; continue; };		\
	      test -d "$$dst" || continue;				\
	      echo "$$dst"/$$(basename "$$src");			\
	    done > $$exempt;						\
	git ls-files tests/unictype >> $$exempt;			\
	git ls-files doc/INSTALL* >> $$exempt;				\
	git ls-files | grep -vFf $$exempt				\
	  | xargs grep -L '^/\*.*GENERATED AUTOMATICALLY'		\
	  | UPDATE_COPYRIGHT_MAX_LINE_LENGTH=79				\
	    UPDATE_COPYRIGHT_USE_INTERVALS=1				\
	      xargs build-aux/update-copyright
