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

from __future__ import annotations


class GLFileTable:
    '''The GLFileTable class stores file information for the duration of the
    operation being executed.'''

    all_files: list[str]
    old_files: list[tuple[str, str]]
    new_files: list[tuple[str, str]]
    added_files: list[str]
    removed_files: list[str]

    def __init__(self, all_files: list[str]) -> None:
        '''Create a GLFileTable with initialized fields.
        - all_files, a list of all files being operated on.'''
        self.all_files = all_files
        self.old_files = []
        self.new_files = []
        self.added_files = []
        self.removed_files = []
