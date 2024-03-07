# Copyright (C) 2002-2024 Free Software Foundation, Inc.
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

#===============================================================================
# Define global imports
#===============================================================================
import os
from . import constants
from .GLConfig import GLConfig


#===============================================================================
# Define module information
#===============================================================================
__author__ = constants.__author__
__license__ = constants.__license__
__copyright__ = constants.__copyright__


#===============================================================================
# Define global constants
#===============================================================================
TESTS = constants.TESTS
joinpath = constants.joinpath


#===============================================================================
# Define GLMakefileTable class
#===============================================================================
class GLMakefileTable(object):
    '''This class is used to edit Makefile and store edits as table.
    When user creates  Makefile, he may need to use this class.'''

    def __init__(self, config):
        '''GLMakefileTable.__init__(config) -> GLMakefileTable

        Create GLMakefileTable instance.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.table = list()

    def __getitem__(self, y):
        '''x.__getitem__(y) = x[y]'''
        if type(y) is not int:
            raise TypeError('indices must be integers, not %s'
                            % type(y).__name__)
        result = self.table[y]
        return dict(result)

    def editor(self, dir, var, val, dotfirst=False):
        '''GLMakefileTable.editor(dir, var, val, dotfirst)

        This method is used to remember that ${dir}Makefile.am needs to be edited
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
        self.table += [dictionary]

    def parent(self, gentests, source_makefile_am, tests_makefile_am):
        '''GLMakefileTable.parent(gentests)

        Add a special row to Makefile.am table with the first parent directory
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
               and (joinpath(self.config['destdir'], dir1, 'Makefile.am')
                    or joinpath(dir1, 'Makefile.am') == joinpath(sourcebase, source_makefile_am)
                    or (gentests and joinpath(dir1, 'Makefile.am') == joinpath(testsbase, tests_makefile_am)))):
            dir2 = joinpath(os.path.basename(dir1), dir2)
            dir1 = os.path.dirname(dir1)
        self.editor(dir1, 'EXTRA_DIST', joinpath(dir2, 'gnulib-cache.m4'))

    def count(self):
        '''GLMakefileTable.count() -> int

        Count number of edits which were applied.'''
        return len(self.table)
