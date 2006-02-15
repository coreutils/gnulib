# -*-Makefile-*-
# This Makefile fragment is intended to be useful by any GNU-like project.
# This file originate from coreutils, CPPI, Bison, and Autoconf.

## Copyright (C) 2001-2006 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

ME := maint.mk

# List of all C-like source code files that will be tested for
# stylistic "errors".  You may want to define this to something 
# more complex in Makefile.cfg.
C_SOURCES ?= $(shell find . -name '*.[chly]')

# Add some more files to check, typically set in Makefile.cfg.
C_SOURCES += $(C_SOURCES_ADD)

# Do not save the original name or timestamp in the .tar.gz file.
# Use --rsyncable if available.
gzip_rsyncable := \
  $(shell gzip --help|grep rsyncable >/dev/null && echo --rsyncable)
GZIP_ENV = '--no-name --best $(gzip_rsyncable)'

# Prevent programs like 'sort' from considering distinct strings to be equal.
# Doing it here saves us from having to set LC_ALL elsewhere in this file.
export LC_ALL = C

# Casting arguments to free is never necessary.
sc_cast_of_argument_to_free:
	@grep -nE '\<free *\( *\(' $(C_SOURCES) &&		\
	  { echo '$(ME): don'\''t cast free argument' 1>&2;		\
	    exit 1; } || :

# Collect the names of rules starting with `sc_'.
syntax-check-rules := $(shell sed -n 's/^\(sc_[a-zA-Z0-9_-]*\):.*/\1/p' $(ME))
.PHONY: $(syntax-check-rules)

syntax-check: $(syntax-check-rules)
