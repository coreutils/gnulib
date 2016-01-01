# GNU Makefile for gnulib central.
# Copyright (C) 2006, 2009-2016 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# This Makefile requires the use of GNU make.  Some targets require
# that you have tools like git, makeinfo and cppi installed.

# Required for the use of <(...) below.
SHELL=/bin/bash

# Produce some files that are not stored in the repository.
all:

# Produce the documentation in readable form.
info html dvi pdf:
	cd doc && $(MAKE) $@ && $(MAKE) mostlyclean

# Collect the names of rules starting with 'sc_'.
syntax-check-rules := $(sort $(shell sed -n 's/^\(sc_[a-zA-Z0-9_-]*\):.*/\1/p'\
			Makefile))

# Perform some platform independent checks on the gnulib code.
check: $(syntax-check-rules)

sc_prefer_ac_check_funcs_once:
	@if test -d .git; then						\
	  git grep -w -l AC_CHECK_FUNCS modules				\
	    && { echo use AC_CHECK_FUNCS_ONCE, not AC_CHECK_FUNCS	\
		   in modules/ 1>&2; exit 1; } || :			\
	else :; fi

sc_prohibit_leading_TABs:
	@if test -d .git; then						\
	  git grep -l '^ *	' lib m4 tests				\
	    | grep -Ev '^lib/reg|Makefile|test-update-copyright'	\
	    | grep .							\
	    && { printf '*** %s\n' 'indent with spaces, not TABs;'	\
		 1>&2; exit 1; } || :					\
	else :; fi

sc_prohibit_augmenting_PATH_via_TESTS_ENVIRONMENT:
	@if test -d .git; then						\
	  url=http://thread.gmane.org/gmane.comp.lib.gnulib.bugs/22874;	\
	  git grep '^[	 ]*TESTS_ENVIRONMENT += PATH=' modules		\
	    && { printf '%s\n' 'Do not augment PATH via TESTS_ENVIRONMENT;' \
		 "  see <$$url>" 1>&2; exit 1; } || :			\
	else :; fi

# It's easy to forget the noise-suppressing "@" at the beginning
# of each sc_ rule.  Check for it both in maint.mk and in this file.
sc_prohibit_sc_omitted_at:
	@if test -d .git; then						\
	  git grep -n -A1 '^sc_[[:alnum:]_-]*:' top/maint.mk Makefile	\
            | grep -vE ':sc_|[0-9][-]	@|--$$'				\
            | sed 's/-\([0-9][0-9]*\)-/:\1:/'				\
	    | grep .							\
	    && { printf '*** %s\n' 'oops; missing "@"'			\
		 1>&2; exit 1; } || :					\
	else :; fi

# Run all maint.mk syntax-check tests on gnulib's sources.
sc_maint:
	@rm -f maint.mk; ln -s top/maint.mk maint.mk
	$(MAKE) -s srcdir=. gnulib_dir=. _build-aux=build-aux \
            -f cfg.mk -f maint.mk syntax-check
	rm -f maint.mk

# Files in m4/ that (exceptionally) may use AC_LIBOBJ.
# Do not include their ".m4" suffix.
allow_AC_LIBOBJ =	\
  close			\
  dprintf		\
  dup2			\
  faccessat		\
  fchdir		\
  fclose		\
  fcntl			\
  fprintf-posix		\
  open			\
  printf-posix-rpl	\
  snprintf		\
  sprintf-posix		\
  stdio_h		\
  vasnprintf		\
  vasprintf		\
  vdprintf		\
  vfprintf-posix	\
  vprintf-posix		\
  vsnprintf		\
  vsprintf-posix

allow_AC_LIBOBJ_or := $(shell echo $(allow_AC_LIBOBJ) | tr -s ' ' '|')

sc_prohibit_AC_LIBOBJ_in_m4:
	@url=http://article.gmane.org/gmane.comp.lib.gnulib.bugs/26995;	\
	if test -d .git; then						\
	  git ls-files m4						\
	     | grep -Ev '^m4/($(allow_AC_LIBOBJ_or))\.m4$$'		\
	     | xargs grep '^ *AC_LIBOBJ('				\
	    && { printf '%s\n' 'Do not use AC_LIBOBJ in m4/*.m4;'	\
		 "see <$$url>"; exit 1; } || :;				\
	else :; fi

sc_pragma_columns:
	@if test -d .git; then						\
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
	@./gnulib-tool --extract-filelist \
	    $$(cd ./modules; grep -ilrE '(meyering|blake)' .) \
	  | sort -u \
	  | grep '\.c$$' \
	  | grep -vE '/(stdio-(read|write)|getloadavg)\.c$$' \
	  | xargs cppi -c

# Ensure that the list of symbols checked for by the
# sc_prohibit_intprops_without_use rule match those in the actual file.
# Extract the symbols from the .h file and compare with the list of
# symbols extracted from the rule in maint.mk.
sc_check_sym_list:
	@i=lib/intprops.h; \
	diff -u <(perl -lne '/^# *define ([A-Z]\w+)\(/ and print $$1' $$i|fmt) \
	  <(sed -n /^_intprops_name/,/^_intprops_syms_re/p top/maint.mk \
	    |sed '/^_/d;s/^  //;s/	*\\$$//')

# Ensure that the copyright statements in files and in the module descriptions
# are consistent.
sc_check_copyright:
	@./check-copyright

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
