# Copyright (C) 2002-2022 Free Software Foundation, Inc.
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

    def editor(self, dir, var, val):
        '''GLMakefileTable.editor(dir, var, val)

        This method is used to remember that ${dir}Makefile.am needs to be edited
        to that ${var} mentions ${val}.'''
        if type(dir) is not str:
            raise TypeError('dir must be a string, not %s' % (type(dir).__name__))
        if type(var) is not str:
            raise TypeError('var must be a string, not %s' % (type(var).__name__))
        if type(val) is not str:
            raise TypeError('val must be a string, not %s' % (type(val).__name__))
        dictionary = {'dir': dir, 'var': var, 'val': val}
        self.table += [dictionary]

    def parent(self):
        '''GLMakefileTable.parent()

        Add a special row to Makefile.am table with the first parent directory
        which contains or will contain Makefile.am file.
        GLConfig: sourcebase, m4base, testsbase, incl_test_categories,
        excl_test_categories, makefile_name.'''
        m4base = self.config['m4base']
        sourcebase = self.config['sourcebase']
        testsbase = self.config['testsbase']
        makefile_name = self.config['makefile_name']
        inctests = self.config.checkInclTestCategory(TESTS['tests'])
        dir1 = '%s%s' % (m4base, os.path.sep)
        mfd = 'Makefile.am'
        if not makefile_name:
            mfx = 'Makefile.am'
        else:  # if makefile_name
            mfx = makefile_name
        dir2 = ''
        while (dir1
               and (joinpath(self.config['destdir'], dir1, mfd)
                    or joinpath(dir1, mfd) == joinpath(sourcebase, mfx)
                    or (inctests and joinpath(dir1, mfd) == joinpath(testsbase, mfx)))):
            dir2 = joinpath(os.path.basename(dir1), dir2)
            dir1 = os.path.dirname(dir1)
        self.editor(dir1, 'EXTRA_DIST', joinpath(dir2, 'gnulib-cache.m4'))

    def count(self):
        '''GLMakefileTable.count() -> int

        Count number of edits which were applied.'''
        return len(self.table)
