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

#===============================================================================
# Define global imports
#===============================================================================
import os
from .constants import joinpath
from .GLConfig import GLConfig

#===============================================================================
# Define GLMakefileTable class
#===============================================================================
class GLMakefileTable:
    '''This class is used to edit Makefile and store edits as table.
    When user creates Makefile.am, he may need to use this class.
    The internal representation consists of a list of edits.
    Each edit is a dictionary with keys 'dir', 'var', 'val', 'dotfirst'.
    An edit may be removed; this is done by removing its 'var' key but
    keeping it in the list. Removed edits must be ignored.'''

    config: GLConfig
    table: list[dict[str, str | bool]]

    def __init__(self, config: GLConfig) -> None:
        '''Create GLMakefileTable instance.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.table = []

    def __getitem__(self, y: int) -> dict[str, str | bool]:
        '''x.__getitem__(y) = x[y]'''
        if type(y) is not int:
            raise TypeError('indices must be integers, not %s'
                            % type(y).__name__)
        result = self.table[y]
        # Do *not* clone the result here. We want GLEmiter to be able to make
        # side effects on the result.
        return result

    def editor(self, dir: str, var: str, val: str, dotfirst: bool = False) -> None:
        '''This method is used to remember that ${dir}Makefile.am needs to be edited
        to that ${var} mentions ${val}.
        If ${dotfirst} is non-empty, this mention needs to be present after '.'.
        This is a special hack for the SUBDIRS variable, cf.
        <https://www.gnu.org/software/automake/manual/html_node/Subdirectories.html>.'''
        if type(dir) is not str:
            raise TypeError('dir must be a string, not %s' % (type(dir).__name__))
        if type(var) is not str:
            raise TypeError('var must be a string, not %s' % (type(var).__name__))
        if type(val) is not str:
            raise TypeError('val must be a string, not %s' % (type(val).__name__))
        if type(dotfirst) is not bool:
            raise TypeError('dotfirst must be a bool, not %s' % (type(dotfirst).__name__))
        dictionary = {'dir': dir, 'var': var, 'val': val, 'dotfirst': dotfirst}
        self.table.append(dictionary)

    def parent(self, gentests: bool, source_makefile_am: str, tests_makefile_am: str) -> None:
        '''Add a special row to Makefile.am table with the first parent directory
        which contains or will contain Makefile.am file.
        GLConfig: sourcebase, m4base, testsbase, incl_test_categories,
        excl_test_categories, makefile_name.
        gentests is a bool that is True if any files are to be placed in $testsbase.
        source_makefile_am is the name of the source Makefile.am.
        tests_makefile_am is the name of the tests Makefile.am.'''
        if type(gentests) is not bool:
            raise TypeError('gentests must be a bool, not %s' % (type(gentests).__name__))
        if type(source_makefile_am) is not str:
            raise TypeError('source_makefile_am must be a str, not %s' % (type(source_makefile_am).__name__))
        if type(tests_makefile_am) is not str:
            raise TypeError('tests_makefile_am must be a str, not %s' % (type(tests_makefile_am).__name__))
        m4base = self.config['m4base']
        sourcebase = self.config['sourcebase']
        testsbase = self.config['testsbase']
        dir1 = '%s%s' % (m4base, os.path.sep)
        dir2 = ''
        while (dir1
               and not (os.path.isfile(joinpath(self.config['destdir'], dir1, 'Makefile.am'))
                        or joinpath(dir1, 'Makefile.am') == joinpath(sourcebase, source_makefile_am)
                        or (gentests and joinpath(dir1, 'Makefile.am') == joinpath(testsbase, tests_makefile_am)))):
            dir2 = joinpath(os.path.basename(dir1), dir2)
            dir1 = os.path.dirname(dir1)
        self.editor(dir1, 'EXTRA_DIST', joinpath(dir2, 'gnulib-cache.m4'))

    def count(self) -> int:
        '''Count number of edits which are stored, including the removed ones.'''
        return len(self.table)
