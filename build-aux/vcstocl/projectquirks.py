# Project quirks base class.
# Copyright (C) 2020-2022 Free Software Foundation, Inc.
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


class ProjectQuirks:
    ''' Base class for project quirks

    The following members can be overridden in the subclass:

    - MACRO_QUIRKS
      A list of dictionary entries with indexes as `orig` and `sub` where `orig`
      is a Python regular expression pattern to match and `sub` is the
      substitution.  These substitutions are used to work around C/C++ macros
      that are known to break parsing of C programs.

    - C_MACROS
      This is a list of macro definitions that are extensively used and are
      known to break parsing due to some characteristic, mainly the lack of a
      semicolon at the end.

    - IGNORE_LIST
      A list of files to ignore in the changesets, either because they are not
      needed (such as the ChangeLog) or because they are not parseable.  For
      example, glibc has a header file that is only assembly code, which breaks
      the C parser.

    - repo
      Specify the project repo source control.  The default value is `git`.

    '''
    MACRO_QUIRKS = []
    C_MACROS = []
    repo = 'git'
    IGNORE_LIST = ['ChangeLog']
