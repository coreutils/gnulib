# GNU Makefile for gnulib central.
# Copyright (C) 2006, 2009-2025 Free Software Foundation, Inc.
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

# ==============================================================================
# Documentation

# Produce the documentation in readable form.
info html dvi pdf:
	cd doc && $(MAKE) $@ && $(MAKE) mostlyclean

# ==============================================================================
# Various checks

# Collect the names of rules starting with 'sc_'.
syntax-check-rules := $(sort $(shell sed -n 's/^\(sc_[a-zA-Z0-9_-]*\):.*/\1/p'\
			Makefile))

# Perform some platform independent checks on the gnulib code.
check: $(syntax-check-rules)

sc_prefer_ac_check_funcs_once:
	if test -d .git; then						\
	  git grep -l -w AC_CHECK_FUNCS modules				\
	    | grep -Ev '$(exclude_file_name_regexp--sc_prefer_ac_check_funcs_once)' \
	    | grep .							\
	    && { echo use AC_CHECK_FUNCS_ONCE, not AC_CHECK_FUNCS	\
		   in modules/ 1>&2; exit 1; } || :			\
	else :; fi
exclude_file_name_regexp--sc_prefer_ac_check_funcs_once = \
  ^modules/(jit/cache)

sc_prohibit_leading_TABs:
	if test -d .git; then						\
	  git grep -l '^ *	' lib m4 tests				\
	    | grep -Ev '$(exclude_file_name_regexp--sc_prohibit_leading_TABs)' \
	    | grep .							\
	    && { printf '*** %s\n' 'indent with spaces, not TABs;'	\
		 1>&2; exit 1; } || :					\
	else :; fi
exclude_file_name_regexp--sc_prohibit_leading_TABs = \
  ^(lib/(cdefs\.h|getopt|ieee754|malloc/|mini-|mktime\.c|qsort\.c|reg|strverscmp\.c)|m4/(largefile|std-gnu)|m4/libgcrypt|tests/from-glibc/|tests/test-update-copyright|Makefile|.*/Makefile)

# Cf. <https://github.com/cpredef/predef/blob/master/Compilers.md>
sc_prohibit_misspelled_compiler_predefs:
	if test -d .git; then						\
	  git ls-files m4 lib tests					\
	    | xargs grep -Ew '($(misspelled_compiler_predefs_or))'		\
	    && { printf '*** %s\n' 'misspelled predefs' 1>&2; exit 1; }	\
	    || :							\
	else :; fi
misspelled_compiler_predefs_or = $(shell echo $(misspelled_compiler_predefs) | tr -s ' ' '|')
misspelled_compiler_predefs =\
  __clang_major		\
  __clang_minor		\
  __GNUC		\
  __GNUC_MAJOR		\
  __GNUC_MAJOR__	\
  __GNUC_MINOR		\

# Cf. <https://github.com/cpredef/predef/blob/master/Architectures.md>
# Only the users of module 'host-cpu-c-abi' are allowed to use __${arch}__
# for all architectures.
sc_prohibit_misspelled_cpu_predefs:
	if test -d .git; then						\
	  git ls-files m4 lib tests					\
	    | grep -Ev '^(m4/host-cpu-c-abi\.m4|lib/sigsegv\.(in\.h|c)|tests/test-sigsegv.*|tests/jit/test-cache\.c)$$' \
	    | xargs grep -Ew '($(misspelled_cpu_predefs_or))'		\
	    && { printf '*** %s\n' 'misspelled predefs' 1>&2; exit 1; }	\
	    || :							\
	else :; fi
misspelled_cpu_predefs_or = $(shell echo $(misspelled_cpu_predefs) | tr -s ' ' '|')
misspelled_cpu_predefs =\
  __alpha__		\
  __amd64		\
  __x86_64		\
  __arm			\
  __arm64		\
  __arm64__		\
  __aarch64		\
  __hppa__		\
  __loongarch64__	\
  __m68k		\
  __riscv__		\
  __riscv64		\
  __riscv64__		\
  __sparc__		\
  __sparc64		\
  __sparc64__		\

# Cf. <https://github.com/cpredef/predef/blob/master/OperatingSystems.md>
sc_prohibit_misspelled_os_predefs:
	if test -d .git; then						\
	  git ls-files m4 lib tests					\
	    | xargs grep -Ew '($(misspelled_os_predefs_or))'		\
	    && { printf '*** %s\n' 'misspelled predefs' 1>&2; exit 1; }	\
	    || :							\
	else :; fi
misspelled_os_predefs_or = $(shell echo $(misspelled_os_predefs) | tr -s ' ' '|')
misspelled_os_predefs =	\
  __AIX__		\
  __FreeBSD		\
  __NetBSD		\
  __OpenBSD		\
  __CYGWIN		\
  __CYGWIN32__		\
  __MINGW__		\
  __WIN32__		\
  __WIN64__		\
  __GNU__		\
  __HAIKU		\
  __hpux__		\
  __IRIX__		\
  __sgi__		\
  __linux		\
  __minix__		\
  __sun__		\
  __unix		\
  __unix__		\
  __ZOS__		\
  __zOS__		\

sc_prohibit_augmenting_PATH_via_TESTS_ENVIRONMENT:
	if test -d .git; then						\
	  url=https://lists.gnu.org/r/bug-gnulib/2010-09/msg00064.html; \
	  git grep '^[	 ]*TESTS_ENVIRONMENT += PATH=' modules		\
	    && { printf '%s\n' 'Do not augment PATH via TESTS_ENVIRONMENT;' \
		 "  see <$$url>" 1>&2; exit 1; } || :			\
	else :; fi

# It's easy to forget the noise-suppressing "@" at the beginning
# of each sc_ rule.  Check for it in maint.mk.
sc_prohibit_sc_omitted_at:
	if test -d .git; then						\
	  git grep -n -A1 '^sc_[[:alnum:]_-]*:' top/maint.mk		\
	    | grep -vE ':sc_|[0-9][-]	@|--$$'				\
	    | sed 's/-\([0-9][0-9]*\)-/:\1:/'				\
	    | grep .							\
	    && { printf '*** %s\n' 'oops; missing "@"'			\
		 1>&2; exit 1; } || :					\
	else :; fi

## Unmaintained. When removing this rule, also remove cfg.mk.
## # Run all maint.mk syntax-check tests on gnulib's sources.
## sc_maint:
## 	@rm -f maint.mk; ln -s top/maint.mk maint.mk
## 	$(MAKE) -s srcdir=. gnulib_dir=. _build-aux=build-aux \
## 	        -f cfg.mk -f maint.mk syntax-check
## 	rm -f maint.mk

sc_prohibit_AC_LIBOBJ_in_m4:
	url=https://lists.gnu.org/r/bug-gnulib/2011-06/msg00051.html; \
	if test -d .git; then						\
	  git ls-files m4						\
	     | grep -Ev '^m4/($(allow_AC_LIBOBJ_or))\.m4$$'		\
	     | xargs grep '^ *AC_LIBOBJ('				\
	    && { printf '%s\n' 'Do not use AC_LIBOBJ in m4/*.m4;'	\
		 "see <$$url>"; exit 1; } || :;				\
	else :; fi
allow_AC_LIBOBJ_or = $(shell echo $(allow_AC_LIBOBJ) | tr -s ' ' '|')
# Files in m4/ that (exceptionally) may use AC_LIBOBJ, without their ".m4"
# suffix.
allow_AC_LIBOBJ =	\
  dprintf		\
  fprintf-posix		\
  printf-posix		\
  snprintf		\
  sprintf-posix		\
  termcap		\
  terminfo		\
  vasnprintf		\
  vasprintf		\
  vdprintf		\
  vfprintf-posix	\
  vprintf-posix		\
  vsnprintf		\
  vsprintf-posix

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
  _GL_ATTRIBUTE_DEALLOC_FREE \
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
  _GL_ATTRIBUTE_NONNULL_IF_NONZERO \
  _GL_ATTRIBUTE_NONSTRING \
  _GL_ATTRIBUTE_NOTHROW \
  _GL_ATTRIBUTE_PACKED \
  _GL_ATTRIBUTE_PURE \
  _GL_ATTRIBUTE_REPRODUCIBLE \
  _GL_ATTRIBUTE_RETURNS_NONNULL \
  _GL_ATTRIBUTE_SENTINEL \
  _GL_ATTRIBUTE_UNSEQUENCED \
  _GL_ATTRIBUTE_UNUSED \
  _GL_UNUSED_LABEL \
  _GL_ATTRIBUTE_CAPABILITY_TYPE \
  _GL_ATTRIBUTE_ACQUIRE_CAPABILITY \
  _GL_ATTRIBUTE_RELEASE_CAPABILITY \
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
	for file in `grep -l -F -w -f <(for macro in $(config_h_MACROS); do echo $$macro; done) lib/*.h lib/*/*.h \
	             | grep -vE '$(exclude_file_name_regexp--sc_check_config_h_reminder)'`; do \
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
exclude_file_name_regexp--sc_check_config_h_reminder = \
  ^lib/(noreturn\.h|(uninorm|unistr)\.in\.h)


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

# ==============================================================================
# Regenerating some files

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

# ==============================================================================
# Updating copyright notices

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

# ==============================================================================
# Maintaining localizations

# Creates an up-to-date POT file (in the po/ directory).
gnulib.pot:
	cd po && make

# Creates a snapshot tarball for the Translation Project. Once created,
# 1. upload it to alpha.gnu.org via
#    $ build-aux/gnupload --to alpha.gnu.org:gnulib gnulib-????????.tar.gz
# 2. notify <coordinator@translationproject.org>
gnulib-tp-snapshot: gnulib.pot
	version=`date -u +"%Y%m%d"`; \
	dir=gnulib-$$version; \
	mkdir $$dir \
	&& for file in `find lib -type f` `find po -type f` COPYING; do \
	     case $$file in \
	       *.orig | *.rej | *~ | '.#'* | '#'*'#' ) ;; \
	       *) \
	         mkdir -p $$dir/`dirname $$file` || exit 1; \
	         ln $$file $$dir/$$file || exit 1; \
	     esac; \
	   done \
	&& { echo 'This tarball contains the GNU gnulib sources relevant for translators.'; \
	     echo 'It is only meant for use by the translators and the translation coordinator.'; \
	     echo 'If you are a developer, use a git checkout of the GNU gnulib project instead.'; \
	   } > $$dir/README \
	&& tar --owner=root --group=root -cf $$dir.tar $$dir \
	&& gzip -9 --force $$dir.tar \
	&& rm -rf $$dir \
	&& ls -l $$dir.tar.gz

# Creates a tarball with the gnulib localizations. Once created,
# 1. upload it to ftp.gnu.org via
#    $ build-aux/gnupload --to ftp.gnu.org:gnulib gnulib-l10n-????????.tar.gz
# 2. notify your preferred distros so that they pick it up.
gnulib-l10n-release: gnulib.pot
	cp doc/COPYING.LESSERv2 gnulib-l10n/COPYING
	mkdir -p gnulib-l10n/po \
	&& cp po/Makevars gnulib-l10n/po/Makevars \
	&& cp po/gnulib.pot gnulib-l10n/po/gnulib.pot
	cd gnulib-l10n \
	&& ./autogen.sh \
	&& (cd po \
	    && rm -f *.po \
	    && wget --mirror --level=1 -nd -nv -A.po https://translationproject.org/latest/gnulib/ \
	    && touch POTFILES.in \
	    && ls -1 *.po | LC_ALL=C sort | sed -e 's/\.po$$//' > LINGUAS \
	    && for file in *.po; do msgmerge --update --lang=$${file%.po} --previous $$file gnulib.pot || exit 1; done \
	    && for file in *.po; do msgfmt -c -o $${file%.po}.gmo $$file || exit 1; done \
	   ) \
	&& ./configure \
	&& make distcheck \
	&& mv gnulib-l10n-????????.tar.gz .. \
	&& make distclean \
	&& ./autoclean.sh
	ls -l gnulib-l10n-????????.tar.gz

# ==============================================================================
