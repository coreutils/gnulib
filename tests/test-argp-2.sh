#! /bin/sh
# Test suite for argp.
# Copyright (C) 2006 Free Software Foundation, Inc.
# This file is part of the GNUlib Library.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

TMP=argp.$$

unset ARGP_HELP_FMT
ERR=0

func_compare() {
# If argp was compiled without base_name, it will display full program name
  sed '1{
         s,: [^ ]*/test-argp,: test-argp,
        }' | cmp - $TMP
}  

####
# Test --usage output
cat > $TMP <<EOT
Usage: test-argp [-tvO?V] [-f FILE] [-o[ARG]] [--test] [--file=FILE]
            [--input=FILE] [--verbose] [--optional[=ARG]] [--option] [--help]
            [--usage] [--version] ARGS...
EOT

./test-argp --usage | func_compare || ERR=1

####
# Test working usage-indent format

cat > $TMP <<EOT
Usage: test-argp [-tvO?V] [-f FILE] [-o[ARG]] [--test] [--file=FILE]
[--input=FILE] [--verbose] [--optional[=ARG]] [--option] [--help] [--usage]
[--version] ARGS...
EOT

ARGP_HELP_FMT='usage-indent=0' ./test-argp --usage | func_compare || ERR=1

####
# Test --help output
cat >$TMP <<EOT
Usage: test-argp [OPTION...] ARGS...
documentation string

 Main options
  -t, --test

 Option Group 1
  -f, --file=FILE, --input=FILE   Option with a mandatory argument
  -v, --verbose              Simple option without arguments

 Option Group 2
  -o, --optional[=ARG]       Option with an optional argument. ARG is one of
                             the following:
  -O, --option               An option

  many                       many units
  one                        one unit
  two                        two units

  -?, --help                 give this help list
      --usage                give a short usage message
  -V, --version              print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <>.
EOT

./test-argp --help | func_compare || ERR=1

####
# Test ambiguous option handling

./test-argp --optio 2>/dev/null && ERR=1

rm $TMP

exit $ERR
