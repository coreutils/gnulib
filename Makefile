# GNU Makefile for gnulib central.
# Copyright (C) 2006, 2009-2024 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# in any medium, are permitted without royalty provided the copyright
# notice and this notice are preserved.

# This Makefile requires the use of GNU make.  Some targets require
# that you have tools like git, makeinfo and cppi installed.

# Required for the use of <(...) below.
SHELL=bash

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
	  url=https://lists.gnu.org/r/bug-gnulib/2010-09/msg00064.html; \
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
  printf-posix		\
  snprintf		\
  sprintf-posix		\
  stdio_h		\
  termcap		\
  terminfo		\
  vasnprintf		\
  vasprintf		\
  vdprintf		\
  vfprintf-posix	\
  vprintf-posix		\
  vsnprintf		\
  vsprintf-posix

allow_AC_LIBOBJ_or := $(shell echo $(allow_AC_LIBOBJ) | tr -s ' ' '|')

sc_prohibit_AC_LIBOBJ_in_m4:
	@url=https://lists.gnu.org/r/bug-gnulib/2011-06/msg00051.html; \
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


# List of C macros defined through AH_VERBATIM in m4/extern-inline.m4:
config_h_MACROS1 = \
  _GL_INLINE \
  _GL_EXTERN_INLINE \
  _GL_INLINE_HEADER_BEGIN \
  _GL_INLINE_HEADER_END
# List of C macros defined through AH_VERBATIM in m4/gnulib-common.m4:
config_h_MACROS2 = \
  _GL_GNUC_PREREQ \
  _Noreturn \
  _GL_ATTRIBUTE_ALLOC_SIZE \
  _GL_ATTRIBUTE_ALWAYS_INLINE \
  _GL_ATTRIBUTE_ARTIFICIAL \
  _GL_ATTRIBUTE_COLD \
  _GL_ATTRIBUTE_CONST \
  _GL_ATTRIBUTE_DEALLOC \
  _GL_ATTRIBUTE_DEPRECATED \
  _GL_ATTRIBUTE_ERROR \
  _GL_ATTRIBUTE_WARNING \
  _GL_ATTRIBUTE_EXTERNALLY_VISIBLE \
  _GL_ATTRIBUTE_FALLTHROUGH \
  _GL_ATTRIBUTE_FORMAT \
  _GL_ATTRIBUTE_LEAF \
  _GL_ATTRIBUTE_MALLOC \
  _GL_ATTRIBUTE_MAY_ALIAS \
  _GL_ATTRIBUTE_MAYBE_UNUSED \
  _GL_UNUSED \
  _GL_ATTRIBUTE_NODISCARD \
  _GL_ATTRIBUTE_NOINLINE \
  _GL_ATTRIBUTE_NONNULL \
  _GL_ATTRIBUTE_NONSTRING \
  _GL_ATTRIBUTE_NOTHROW \
  _GL_ATTRIBUTE_PACKED \
  _GL_ATTRIBUTE_PURE \
  _GL_ATTRIBUTE_RETURNS_NONNULL \
  _GL_ATTRIBUTE_SENTINEL \
  _GL_ATTRIBUTE_UNUSED \
  _GL_UNUSED_LABEL \
  _GL_BEGIN_C_LINKAGE \
  _GL_END_C_LINKAGE \
  _GL_ASYNC_SAFE \
  _GL_CMP
# List of C macros defined through AH_VERBATIM in m4/nullptr.m4:
config_h_MACROS3 = \
  nullptr
# List of C macros defined through AH_VERBATIM in m4/posixcheck.m4:
config_h_MACROS4 = \
  GNULIB_POSIXCHECK
# List of C macros defined through AH_VERBATIM in m4/sh-filename.m4:
config_h_MACROS5 = \
  BOURNE_SHELL
# List of C macros defined through AH_VERBATIM in m4/stdalign.m4:
config_h_MACROS6 = \
  alignof \
  alignas
# List of C macros defined through AH_VERBATIM in m4/stdarg.m4:
config_h_MACROS7 = \
  va_copy
# List of C macros defined through AH_VERBATIM in m4/threads_h.m4:
config_h_MACROS8 = \
  _Thread_local
# List of C macros defined through AH_VERBATIM (only the most important ones):
config_h_MACROS = \
  $(config_h_MACROS1) $(config_h_MACROS2) $(config_h_MACROS3) \
  $(config_h_MACROS4) $(config_h_MACROS5) $(config_h_MACROS6) \
  $(config_h_MACROS7) $(config_h_MACROS8)

# Ensure that .h files that use macros from config.h contain a reminder to
# include <config.h>.
sc_check_config_h_reminder:
	fail=0; \
	for file in `grep -l -F -w -f <(for macro in $(config_h_MACROS); do echo $$macro; done) lib/*.h lib/*/*.h`; do \
	  : "Filter out .h files that are not public header files of their respective module."; \
	  include_pattern='[<"]'`echo $$file | sed -e 's,^lib/,,' -e 's,[.]in[.]h,.h,' -e 's,_,[/_],g' -e 's,[.],[.],g'`'[>"]' ; \
	  if ./gnulib-tool --extract-include-directive `./gnulib-tool --find $$file` | grep "$$include_pattern" >/dev/null; then \
	    grep '# *error "Please include config[.]h first[.]"' $$file >/dev/null \
	      || { echo -n "File $$file lacks a config.h reminder. Needed for:"; \
	           for macro in $(config_h_MACROS); do \
	             if grep -F -w $$macro $$file >/dev/null; then echo -n " $$macro"; fi; \
	           done; \
	           if grep -F HAVE_RAW_DECL_ $$file >/dev/null; then echo -n " HAVE_RAW_DECL_*"; fi; \
	           echo; \
	           fail=1; \
	         }; \
	  fi; \
	done; \
	exit $$fail


# Ensure that .h files that invoke _GL_INLINE_HEADER_BEGIN also invoke
# _GL_INLINE_HEADER_END.  Otherwise, some GCC diagnostics remain turned off
# for the rest of the compilation unit.
sc_check_GL_INLINE_HEADER_use:
	fail=0; \
	for file in `grep -l -F -w _GL_INLINE_HEADER_BEGIN lib/*.h lib/*/*.h`; do \
	  grep -l -F -w _GL_INLINE_HEADER_END $$file >/dev/null \
	    || { echo "File $$file lacks an invocation of _GL_INLINE_HEADER_END."; \
	         fail=1; \
	       }; \
	done; \
	exit $$fail

# Ensure that the copyright statements in files and in the module descriptions
# are consistent.
sc_check_copyright:
	@./check-copyright

# Regenerate some files that are stored in the repository.
regen: build-aux/bootstrap MODULES.html

# build-aux/bootstrap needs to be regenerated from top/bootstrap*.
build-aux/bootstrap: top/gen-bootstrap.sed top/bootstrap top/bootstrap-funclib.sh
	sed -f top/gen-bootstrap.sed < top/bootstrap > build-aux/bootstrap-tmp
	chmod a+x build-aux/bootstrap-tmp
	mv build-aux/bootstrap-tmp build-aux/bootstrap

# MODULES.html is periodically being generated and copied to the web pages at
# :ext:USER@cvs.savannah.gnu.org:/web/gnulib/gnulib/
# where it then appears at <https://www.gnu.org/software/gnulib/MODULES.html>.
MODULES.html: MODULES.html.sh
	./MODULES.html.sh > MODULES.html

# A perl BEGIN block to set Y to the current year number and W to Y-1.
_year_and_prev = BEGIN{@t=localtime(time); $$y=$$t[5]+1900; $$w=$$y-1}

# Which TZ setting to use when updating copyright.
COPYRIGHT_TZ = UTC0

# Run this rule once per year (usually early in January)
# to update all FSF copyright year lists here.
# We exclude the files listed in srclist.txt (maintained elsewhere)
# as well as those in tests/unictype (generated).
# Also exclude any file that includes the "GENERATED AUTOMATICALLY" comment,
# being careful not to exclude code that merely generates the comment.
# Also exclude doc/INSTALL*, since they too are generated.
# Also adjust template-style files that must start with a single
# (the current) year number in some places.
# Also adjust version-etc.c and gendocs.sh.
update-copyright:
	export TZ='$(COPYRIGHT_TZ)';					\
	exempt=$$(mktemp);						\
	grep -v '^#' config/srclist.txt|grep -v '^$$'			\
	  | while read top src dst options; do				\
	      test -f "$$dst" && { echo "$$dst"; continue; };		\
	      test -d "$$dst" || continue;				\
	      echo "$$dst"/$$(basename "$$src");			\
	    done > $$exempt;						\
	git ls-files tests/unictype >> $$exempt;			\
	git ls-files doc/INSTALL* >> $$exempt;				\
	for file in $$(git ls-files); do				\
	  test ! -h $$file || echo $$file;				\
	done >> $$exempt;						\
	git ls-files | grep -vFf $$exempt				\
	  | xargs grep -L '^/\*.*GENERATED AUTOMATICALLY'		\
	  | UPDATE_COPYRIGHT_MAX_LINE_LENGTH=79				\
	    UPDATE_COPYRIGHT_USE_INTERVALS=1				\
	      xargs build-aux/update-copyright
	export TZ='$(COPYRIGHT_TZ)';					\
	perl -pi -e '$(_year_and_prev) s/(copyright.*)\b$$w\b/$$1$$y/i'	\
	  lib/version-etc.c doc/gnulib.texi build-aux/gendocs.sh
	export TZ='$(COPYRIGHT_TZ)';					\
	perl -pi -e '$(_year_and_prev) s/ $$w-$$y / $$y /g'		\
	  doc/gendocs_template* build-aux/gendocs.sh
	export TZ='$(COPYRIGHT_TZ)';					\
	perl -pi -e							\
	  '$(_year_and_prev) s/^(scriptversion=)$$w.*/$$1$$y-01-01.00/i' \
	  build-aux/gendocs.sh
