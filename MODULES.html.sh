#!/bin/sh
#
# Copyright (C) 2002, 2003 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# Usage: MODULES.html.sh > MODULES.html

PATH=`dirname $0`:$PATH; export PATH

sed_lt='s,<,\&lt;,g'
sed_gt='s,>,\&gt;,g'
sed_escape_dot='s,\.,\\.,g'
sed_escape_slash='s,/,\\/,g'
trnl='\012'

indent=""
missed_modules=`gnulib-tool --list`
missed_files=`ls -d lib/* m4/* | sed -e /CVS/d -e /README/d -e /ChangeLog/d -e /Makefile/d -e /TODO/d -e '/~$/d'`

# func_echo line
# outputs line with indentation.
func_echo ()
{
  echo "${indent}$*"
}

# func_indent
# increases the indentation.
func_indent ()
{
  indent="$indent  "
}

# func_unindent
# decreases the indentation.
func_unindent ()
{
  indent=`echo "$indent" | sed -e 's/  $//'`
}

# func_begin tag [attribute...]
# opens a HTML tag.
func_begin ()
{
  func_echo "<$*>"
  func_indent
}

# func_end tag
# closes a HTML tag.
func_end ()
{
  func_unindent
  func_echo "</$1>"
}

# func_wrap tag [attribute...]
# encloses $element in a HTML tag, without line breaks.
func_wrap ()
{
  element="<$*>${element}</$1>"
}

# func_begin_table
func_begin_table ()
{
  func_begin TABLE
  func_begin TR
  func_echo "<TH ALIGN=LEFT>lib/"
  func_echo "<TH ALIGN=LEFT>lib/"
  func_echo "<TH ALIGN=LEFT>m4/"
  func_echo "<TH ALIGN=LEFT>&nbsp;"
  func_end TR
  func_begin TR
  func_echo "<TH ALIGN=LEFT>Header"
  func_echo "<TH ALIGN=LEFT>Implementation"
  func_echo "<TH ALIGN=LEFT>Autoconf macro"
  func_echo "<TH ALIGN=LEFT>Depends on"
  func_end TR
}

# func_module module
func_module ()
{
  func_begin TR

  sed_remove_trailing_empty_line='${
/^$/d
}'

  element=`gnulib-tool --extract-include-directive $1 | sed -e "$sed_lt" -e "$sed_gt" -e "$sed_remove_trailing_empty_line" | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
  test -n "$element" || element='---'
  func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

  sed_choose_unconditional_nonstandard_include='s,^#include "\(.*\)"$,\1,p'
  includefile=`gnulib-tool --extract-include-directive $1 | sed -n -e "$sed_choose_unconditional_nonstandard_include" | sed -e "$sed_escape_dot" | tr -d "$trnl"`
  sed_choose_lib_files='s,^lib/\(.*\)$,\1,p'
  element=`gnulib-tool --extract-filelist $1 | sed -e '/^$/d' | sed -n -e "$sed_choose_lib_files" | sed -e '/^'"${includefile}"'$/d' | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
  test -n "$element" || element='---'
  func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

  sed_choose_m4_files='s,^m4/\(.*\)$,\1,p'
  element=`(gnulib-tool --extract-filelist $1 | sed -e "$sed_remove_trailing_empty_line" | sed -n -e "$sed_choose_m4_files" | sed -e '/^onceonly/d'; gnulib-tool --extract-autoconf-snippet $1 | sed -e "$sed_remove_trailing_empty_line") | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
  test -n "$element" || element='---'
  func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

  element=`gnulib-tool --extract-dependencies $1 | sed -e "$sed_remove_trailing_empty_line" | sed -e 's/$/<BR>/' | tr -d "$trnl" | sed -e 's/<BR>$//'`
  test -n "$element" || element='---'
  func_echo "<TD ALIGN=LEFT VALIGN=TOP>$element"

  func_end TR

  missed_modules=`echo "$missed_modules" | sed -e '/^'"$1"'$/d'`

  files=`gnulib-tool --extract-filelist $1`
  sed_removal_prefix='s,^,/^,'
  sed_removal_suffix='s,$,\$/d,'
  sed_remove_files=`echo '{'; for file in $files; do echo $file | sed -e "$sed_escape_dot" -e "$sed_escape_slash" | sed -e "$sed_removal_prefix" -e "$sed_removal_suffix"; done; echo '}'`
  missed_files=`echo "$missed_files" | sed -e "$sed_remove_files"`
}

# func_end_table
func_end_table ()
{
  func_end TABLE
}


echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">'
func_begin HTML

func_begin HEAD

element="Gnulib Module List"
func_wrap TITLE
func_echo "$element"

modules=`gnulib-tool --list`
modules=`for m in $modules; do printf "%s, " $m; done | sed -e 's/, $//'`
func_echo '<META NAME="keywords" CONTENT="'"${modules}"'">'

func_end HEAD

func_begin BODY

func_end BODY

element="Gnulib Module List"
func_wrap H1
func_echo "$element"

func_echo 'This is a list of the modules which make up gnulib, with dependencies.'

element="Support for systems lacking ANSI C 89"
func_wrap H2
func_echo "$element"

func_begin_table
func_module c-bs-a
func_module assert
func_module atexit
func_module strtod
func_module strtol
func_module strtoul
func_module memchr
func_module memcmp
func_module memcpy
func_module memmove
func_module memset
func_module strcspn
func_module strpbrk
func_module strstr
func_module strerror
func_module mktime
func_end_table

func_echo 'These modules are not listed among dependencies below, for simplicity.'

element="Enhancements for ANSI C 89 functions"
func_wrap H2
func_echo "$element"

element="Memory management functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module malloc
func_module realloc
func_end_table

element="Date and time <time.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module strftime
func_end_table

element="Extra functions based on ANSI C 89"
func_wrap H2
func_echo "$element"

element="Memory management functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module xalloc
func_module alloca
func_module bumpalloc
func_end_table

element="Integer arithmetic functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module gcd
func_end_table

element="Environment variables <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module putenv
func_module setenv
func_end_table

element="Character handling <ctype.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

element="String handling <string.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module bcopy
func_module memrchr
func_module stpcpy
func_module strcase
func_module strdup
func_module strnlen
func_module strndup
func_end_table

element="Numeric conversion functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module xstrtod
func_module xstrtol
func_end_table

element="Date and time <time.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module getdate
func_end_table

element="Input/Output <stdio.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module unlocked-io
func_end_table

element="Command-line arguments"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module argmatch
func_module version-etc
func_module long-options
func_end_table

element="Cryptographic computations"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module md5
func_module sha
func_end_table

element="Misc"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module diacrit
func_module getline
func_module linebuffer
func_module obstack
func_module hash-pjw
func_module hash
func_module readtokens
func_module strverscmp
func_end_table

element="Support for systems lacking ISO C 99"
func_wrap H2
func_echo "$element"

element="Boolean type and values <stdbool.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module stdbool
func_end_table

element="Numeric conversion functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module strtoll
func_module strtoull
func_end_table

element="Functions for greatest-width integer types <inttypes.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module strtoimax
func_module strtoumax
func_end_table

element="Enhancements for ISO C 99 functions"
func_wrap H2
func_echo "$element"

func_begin_table
func_end_table

element="Extra functions based on ISO C 99"
func_wrap H2
func_echo "$element"

element="Numeric conversion functions <stdlib.h>"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module xstrtoimax
func_module xstrtoumax
func_end_table

element="Support for systems lacking POSIX:2001"
func_wrap H2
func_echo "$element"

func_begin_table
func_module chown
func_module dup2
func_module ftruncate
func_module getgroups
func_module gettimeofday
func_module mkdir
func_module mkstemp
func_module stat
func_module lstat
func_module timespec
func_module nanosleep
func_module regex
func_module rename
func_module rmdir
func_module utime
func_end_table

element="Enhancements for POSIX:2001 functions"
func_wrap H2
func_echo "$element"

func_begin_table
func_module dirname
func_module getopt
func_module unistd-safer
func_module fnmatch
func_module fnmatch-posix
func_module fnmatch-gnu
func_module exclude
func_end_table

element="Extra functions based on POSIX:2001"
func_wrap H2
func_echo "$element"

element="Numeric conversion functions"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module human
func_end_table

element="File system functions"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module acl
func_module backupfile
func_module fsusage
func_module dirfd
func_module euidaccess
func_module file-type
func_module fileblocks
func_module filemode
func_module isdir
func_module lchown
func_module makepath
func_module modechange
func_module mountlist
func_module path-concat
func_module pathmax
func_module same
func_module save-cwd
func_module savedir
func_module xgetcwd
func_module xreadlink
func_end_table

element="File descriptor based Input/Output"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module safe-read
func_module safe-write
func_module full-read
func_module full-write
func_end_table

element="File stream based Input/Output"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module fpending
func_module closeout
func_module stdio-safer
func_module getpass
func_end_table

element="Users and groups"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module getugroups
func_module group-member
func_module idcache
func_module userspec
func_end_table

element="Date and time"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module gettime
func_module settime
func_module posixtm
func_end_table

element="Networking functions"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module gethostname
func_module xgethostname
func_module canon-host
func_end_table

element="Internationalization functions"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module gettext
func_module iconv
func_module localcharset
func_module hard-locale
func_module mbswidth
func_module memcasecmp
func_module memcoll
func_module xmemcoll
func_module unicodeio
func_module rpmatch
func_module yesno
func_end_table

element="Executing programs"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

element="Java"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

element="Misc"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H3
func_echo "$element"

func_begin_table
func_module exitfail
func_module c-stack
func_module error
func_module fatal
func_module getloadavg
func_module getpagesize
func_module getusershell
func_module physmem
func_module posixver
func_module quotearg
func_module quote
func_module readutmp
func_module sig2str
func_end_table

if test -n "$missed_modules"; then

  element="Unclassified modules - please update MODULES.html.sh"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_wrap H2
  func_echo "$element"

  func_begin_table
  for module in $missed_modules; do
    func_module $module
  done
  func_end_table

fi

if test -n "$missed_files"; then

  element="Lone files - please create new modules containing them"
  element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
  func_wrap H2
  func_echo "$element"

  func_echo '<PRE>'
  echo "$missed_files"
  echo '</PRE>'

fi

element="Future developments"
element=`printf "%s" "$element" | sed -e "$sed_lt" -e "$sed_gt"`
func_wrap H2
func_echo "$element"

func_echo 'Ideally a module could consist of:'
func_begin UL
func_echo '<LI>A header file: lib/<VAR>module</VAR>.h'
func_echo '<LI>One or more implementation files: lib/<VAR>module</VAR>.c et al.'
func_echo '<LI>One or more autoconf macro files: m4/<VAR>module</VAR>.m4 et al.'
func_echo '<LI>A configure.ac fragment, Makefile.am fragment, dependency list: modules/<VAR>module</VAR>'
func_echo '<LI>Some documentation'
func_echo '<LI>A POT file and some PO files'
func_echo '<LI>A testsuite'
func_end UL

func_echo '<HR>'
func_echo 'Generated from <CODE>MODULES.html.sh</CODE> on '`LC_ALL=C date +"%e %B %Y"`.

func_end HTML
