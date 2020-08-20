# Project quirks base class.
# Copyright (C) 2020 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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
    # This is a list of regex substitutions for C/C++ macros that are known to
    # break parsing of the C programs.  Each member of this list is a dict with
    # the key 'orig' having the regex and 'sub' having the substitution of the
    # regex.
    MACRO_QUIRKS = []

    # This is a list of macro definitions that are extensively used and are
    # known to break parsing due to some characteristic, mainly the lack of a
    # semicolon at the end.
    C_MACROS = []

    # The repo type, defaults to git.
    repo = 'git'

    # List of files to ignore either because they are not needed (such as the
    # ChangeLog) or because they are non-parseable.  For example, glibc has a
    # header file that is only assembly code, which breaks the C parser.
    IGNORE_LIST = ['ChangeLog']
