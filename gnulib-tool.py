#! /bin/sh
#
# Copyright (C) 2002-2025 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# This program invokes the Python rewrite of gnulib-tool.
# It is meant to behave identically to gnulib-tool, just faster.

progname=$0

# func_exit STATUS
# exits with a given status.
# This function needs to be used, rather than 'exit', when a 'trap' handler is
# in effect that refers to $?.
func_exit ()
{
  (exit $1); exit $1
}

# func_fatal_error message
# outputs to stderr a fatal error message, and terminates the program.
# Input:
# - progname                 name of this program
func_fatal_error ()
{
  echo "$progname: *** $1" 1>&2
  echo "$progname: *** Stop." 1>&2
  func_exit 1
}

# func_readlink SYMLINK
# outputs the target of the given symlink.
if (type readlink) > /dev/null 2>&1; then
  func_readlink ()
  {
    # Use the readlink program from GNU coreutils.
    readlink "$1"
  }
else
  func_readlink ()
  {
    # Use two sed invocations. A single sed -n -e 's,^.* -> \(.*\)$,\1,p'
    # would do the wrong thing if the link target contains " -> ".
    LC_ALL=C ls -l "$1" | sed -e 's, -> ,#%%#,' | sed -n -e 's,^.*#%%#\(.*\)$,\1,p'
  }
fi

# func_gnulib_dir
# locates the directory where the gnulib repository lives
# Input:
# - progname                 name of this program
# Sets variables
# - self_abspathname         absolute pathname of gnulib-tool
# - gnulib_dir               absolute pathname of gnulib repository
func_gnulib_dir ()
{
  case "$progname" in
    /* | ?:*) self_abspathname="$progname" ;;
    */*) self_abspathname=`pwd`/"$progname" ;;
    *)
      # Look in $PATH.
      # Iterate through the elements of $PATH.
      # We use IFS=: instead of
      #   for d in `echo ":$PATH:" | sed -e 's/:::*/:.:/g' | sed -e 's/:/ /g'`
      # because the latter does not work when some PATH element contains spaces.
      # We use a canonicalized $pathx instead of $PATH, because empty PATH
      # elements are by definition equivalent to '.', however field splitting
      # according to IFS=: loses empty fields in many shells:
      #   - /bin/sh on Solaris loses all empty fields (at the beginning, at
      #     the end, and in the middle),
      #   - GNU bash, /bin/sh on AIX and HP-UX, and /bin/ksh on AIX, HP-UX,
      #     Solaris lose empty fields at the end.
      # The 'case' statement is an optimization, to avoid evaluating the
      # explicit canonicalization command when $PATH contains no empty fields.
      self_abspathname=
      if test "$PATH_SEPARATOR" = ";"; then
        # On Windows, programs are searched in "." before $PATH.
        pathx=".;$PATH"
      else
        # On Unix, we have to convert empty PATH elements to ".".
        pathx="$PATH"
        case :$PATH: in
          *::*)
            pathx=`echo ":$PATH:" | sed -e 's/:::*/:.:/g' -e 's/^://' -e 's/:\$//'`
            ;;
        esac
      fi
      saved_IFS="$IFS"
      IFS="$PATH_SEPARATOR"
      for d in $pathx; do
        IFS="$saved_IFS"
        test -z "$d" && d=.
        if test -x "$d/$progname" && test ! -d "$d/$progname"; then
          self_abspathname="$d/$progname"
          break
        fi
      done
      IFS="$saved_IFS"
      if test -z "$self_abspathname"; then
        func_fatal_error "could not locate the gnulib-tool program - how did you invoke it?"
      fi
      ;;
  esac
  while test -h "$self_abspathname"; do
    # Resolve symbolic link.
    linkval=`func_readlink "$self_abspathname"`
    test -n "$linkval" || break
    case "$linkval" in
      /* | ?:* ) self_abspathname="$linkval" ;;
      * ) self_abspathname=`echo "$self_abspathname" | sed -e 's,/[^/]*$,,'`/"$linkval" ;;
    esac
  done
  gnulib_dir=`echo "$self_abspathname" | sed -e 's,/[^/]*$,,'`
}

# If $progname contains '/' and is not a symlink, it suffices for $prog to be
# the same as $progname with except with basename '.gnulib-tool.py'; this
# speeds startup and might avoid problems in oddball development environments.
# Otherwise, $prog is the absolute name of the .gnulib-tool.py file.
if case $progname in
     */*) test -h "$0" ;;
   esac
then
  func_gnulib_dir
  prog=$gnulib_dir/.gnulib-tool.py
else
  prog=${progname%/*}/.gnulib-tool.py
fi

# Check the Python version.
if (python3 -c 'import sys; sys.exit(not sys.version_info >= (3,7))') 2>/dev/null; then
  :
else
  if (python3 --version) >/dev/null 2>/dev/null; then
    case `python3 --version 2>&1` in
      Python\ 3.[0-6] | Python\ 3.[0-6].*)
        func_fatal_error "python3 is too old (minimum required version is 3.7); try setting GNULIB_TOOL_IMPL=sh" ;;
      Python\ 3.*)
        ;;
      *)
        func_fatal_error "python3 version is unsupported" ;;
    esac
  else
    func_fatal_error "python3 not found; try setting GNULIB_TOOL_IMPL=sh"
  fi
fi

# Tell Python to store the compiled bytecode outside the gnulib directory.
if test -z "$PYTHONPYCACHEPREFIX"; then
  PYTHONPYCACHEPREFIX="${TMPDIR-/tmp}/gnulib-python-cache-${USER-$LOGNAME}"
  export PYTHONPYCACHEPREFIX
fi

profiler_args=
# For profiling, cf. <https://docs.python.org/3/library/profile.html>.
#profiler_args="-m cProfile -s tottime"
exec python3 $profiler_args -- "$prog" "$@"
