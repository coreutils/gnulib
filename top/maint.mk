# -*-Makefile-*-
# This Makefile fragment is intended to be useful by any GNU-like project.
# This file originate from coreutils, CPPI, Bison, and Autoconf.

# Copyright (C) 2001-2009 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

ME := maint.mk
MYSELF := $(srcdir)/$(ME)

# List of all C-like source code files that will be tested for
# stylistic "errors".  You may want to define this to something
# more complex in cfg.mk.
C_SOURCES ?= $(shell find . -name '*.[chly]')

# Add some more files to check, typically set in cfg.mk.
C_SOURCES += $(C_SOURCES_ADD)

# Do not save the original name or timestamp in the .tar.gz file.
# Use --rsyncable if available.
gzip_rsyncable := \
  $(shell gzip --help 2>/dev/null|grep rsyncable >/dev/null && echo --rsyncable)
GZIP_ENV = '--no-name --best $(gzip_rsyncable)'

# Prevent programs like 'sort' from considering distinct strings to be equal.
# Doing it here saves us from having to set LC_ALL elsewhere in this file.
export LC_ALL = C

# There are many rules below that prohibit constructs in this package.
# If the offending construct can be matched with a grep-E-style regexp,
# use this macro.  The shell variables "re" and "msg" must be defined.
define _prohibit_regexp
  dummy=; : so we do not need a semicolon before each use;		\
  test "x$$re" != x || { echo '$(ME): re not defined' 1>&2; exit 1; };	\
  test "x$$msg" != x || { echo '$(ME): msg not defined' 1>&2; exit 1; };\
  grep $(_ignore_case) -nE "$$re" $(C_SOURCES) &&			\
    { echo '$(ME): '"$$msg" 1>&2; exit 1; } || :
endef

# Casting arguments to free is never necessary.
sc_cast_of_argument_to_free:
	@re='\<free *\( *\(' msg='don'\''t cast free argument'		\
	  $(_prohibit_regexp)

sc_cast_of_x_alloc_return_value:
	@re='\*\) *x(m|c|re)alloc\>'					\
	msg='don'\''t cast x*alloc return value'			\
	  $(_prohibit_regexp)

sc_cast_of_alloca_return_value:
	@re='\*\) *alloca\>' msg='don'\''t cast alloca return value'	\
	  $(_prohibit_regexp)

sc_space_tab:
	@re='[ ]	' msg='found SPACE-TAB sequence; remove the SPACE' \
	  $(_prohibit_regexp)

# Don't use *scanf or the old ato* functions in `real' code.
# They provide no error checking mechanism.
# Instead, use strto* functions.
sc_prohibit_atoi_atof:
	@re='\<([fs]?scanf|ato([filq]|ll)) *\('				\
	msg='do not use *scan''f, ato''f, ato''i, ato''l, ato''ll or ato''q' \
	  $(_prohibit_regexp)

# Using EXIT_SUCCESS as the first argument to error is misleading,
# since when that parameter is 0, error does not exit.  Use `0' instead.
sc_error_exit_success:
	@grep -nF 'error (EXIT_SUCCESS,' $(C_SOURCES) &&		\
	  { echo '$(ME): found error (EXIT_SUCCESS' 1>&2;		\
	    exit 1; } || :

# Stylistic, use #ifdef instead of #if
sc_no_if_have_config_h:
	@grep -n '^# *if HAVE_CONFIG_H' $(C_SOURCES) &&			\
	  { echo '$(ME): found use of #if HAVE_CONFIG_H; use #ifdef'	\
		1>&2; exit 1; } || :

# To use this "command" macro, you must first define two shell variables:
# h: the header, enclosed in <> or ""
# re: a regular expression that matches IFF something provided by $h is used.
define _header_without_use
  h_esc=`echo "$$h"|sed 's/\./\\./g'`;					\
  if $(C_SOURCES) | grep -l '\.c$$' > /dev/null; then			\
    files=$$(grep -l '^# *include '"$$h_esc"				\
	     $$($(C_SOURCES) | grep '\.c$$')) &&			\
    grep -LE "$$re" $$files | grep . &&					\
      { echo "$(ME): the above files include $$h but don't use it"	\
	1>&2; exit 1; } || :;						\
  else :;								\
  fi
endef

# Prohibit the inclusion of assert.h without an actual use of assert.
sc_prohibit_assert_without_use:
	@h='<assert.h>' re='\<assert *\(' $(_header_without_use)

sc_obsolete_symbols:
	@re='\<(HAVE''_FCNTL_H|O''_NDELAY)\>'				\
	msg='do not use HAVE''_FCNTL_H or O'_NDELAY			\
	  $(_prohibit_regexp)

# Each nonempty ChangeLog line must start with a year number, or a TAB.
sc_changelog:
	@grep -n '^[^12	]' $$(find . -name ChangeLog) &&	\
	  { echo '$(ME): found unexpected prefix in a ChangeLog' 1>&2;	\
	    exit 1; } || :

# Collect the names of rules starting with `sc_'.
syntax-check-rules := \
  $(shell sed -n 's/^\(sc_[a-zA-Z0-9_-]*\):.*/\1/p' $(MYSELF))
.PHONY: $(syntax-check-rules)

syntax-check: $(syntax-check-rules)

# Code Coverage

init-coverage:
	$(MAKE) $(AM_MAKEFLAGS) clean
	lcov --directory . --zerocounters

COVERAGE_CCOPTS ?= "-g --coverage"
COVERAGE_OUT ?= doc/coverage

build-coverage:
	$(MAKE) $(AM_MAKEFLAGS) CFLAGS=$(COVERAGE_CCOPTS) CXXFLAGS=$(COVERAGE_CCOPTS)
	$(MAKE) $(AM_MAKEFLAGS) CFLAGS=$(COVERAGE_CCOPTS) CXXFLAGS=$(COVERAGE_CCOPTS) check
	mkdir -p $(COVERAGE_OUT)
	lcov --directory . --output-file $(COVERAGE_OUT)/$(PACKAGE).info \
		--capture

gen-coverage:
	genhtml --output-directory $(COVERAGE_OUT) \
		$(COVERAGE_OUT)/$(PACKAGE).info \
		--highlight --frames --legend \
		--title "$(PACKAGE_NAME)"

coverage: init-coverage build-coverage gen-coverage

# Update gettext files.
PACKAGE ?= $(shell basename $(PWD))
POURL = http://translationproject.org/latest/$(PACKAGE)/
PODIR ?= po
refresh-po:
	rm -f $(PODIR)/*.po && \
	echo "$(ME): getting translations into po (please ignore the robots.txt ERROR 404)..." && \
	wget --no-verbose --directory-prefix $(PODIR) --no-directories --recursive --level 1 --accept .po --accept .po.1 $(POURL) && \
	echo 'en@boldquot' > $(PODIR)/LINGUAS && \
	echo 'en@quot' >> $(PODIR)/LINGUAS && \
	ls $(PODIR)/*.po | sed 's/\.po//' | sed 's,$(PODIR)/,,' | sort >> $(PODIR)/LINGUAS

INDENT_SOURCES ?= $(C_SOURCES)
.PHONY: indent
indent:
	indent $(INDENT_SOURCES)
