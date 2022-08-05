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


#===============================================================================
# Define module information
#===============================================================================
__author__ = constants.__author__
__license__ = constants.__license__
__copyright__ = constants.__copyright__


#===============================================================================
# Define global constants
#===============================================================================


#===============================================================================
# Define GLError class
#===============================================================================
class GLError(Exception):
    '''Exception handler for pygnulib classes.'''

    def __init__(self, errno, errinfo=None):
        '''Each error has following parameters:
        errno: code of error; used to catch error type
          1: file does not exist in GLFileSystem: <file>
          2: cannot patch file inside GLFileSystem: <file>
          3: configure file does not exist: <configure.ac>
          4: minimum supported autoconf version is 2.59, not <version>
          5: <gnulib-comp.m4> is expected to contain gl_M4_BASE([<m4base>])
          6: missing sourcebase argument
          7: missing docbase argument
          8: missing testsbase argument
          9: missing libname argument
         10: conddeps are not supported with inctests
         11: incompatible licenses on modules: <modules>
         12: cannot process empty filelist
         13: cannot create the given directory: <directory>
         14: cannot delete the given file: <file>
         15: cannot create the given file: <file>
         16: cannot transform the given file: <file>
         17: cannot update the given file: <file>
         18: module lacks a license: <module>
         19: could not create destination directory: <directory>
        errinfo: additional information'''
        self.errno = errno
        self.errinfo = errinfo
        self.args = (self.errno, self.errinfo)

    def __repr__(self):
        errno = self.errno
        errinfo = self.errinfo
        if self.message == None:
            message = None
            if errno == 1:
                message = "file does not exist in GLFileSystem: %s" % repr(errinfo)
            elif errno == 2:
                message = "cannot patch file inside GLFileSystem: %s" % repr(errinfo)
            elif errno == 3:
                message = "configure file does not exist: %s" % repr(errinfo)
            elif errno == 4:
                message = "minimum supported autoconf version is 2.59, not %s" % repr(errinfo)
            elif errno == 5:
                message = "%s is expected to contain gl_M4_BASE([%s])" % (repr(os.path.join(errinfo, 'gnulib-comp.m4')), repr(errinfo))
            elif errno == 6:
                message = "missing sourcebase argument; cache file doesn't contain it, so you might have to set this argument"
            elif errno == 7:
                message = "missing docbase argument; you might have to create GLImport instance with mode 0 and docbase argument"
            elif errno == 8:
                message = "missing testsbase argument; cache file doesn't contain it, so you might have to set this argument"
            elif errno == 9:
                message = "missing libname argument; cache file doesn't contain it, so you might have to set this argument"
            elif errno == 10:
                message = "conddeps are not supported with inctests"
            elif errno == 11:
                message = "incompatible licenses on modules: %s" % repr(errinfo)
            elif errno == 12:
                message = "cannot process empty filelist"
            elif errno == 13:
                message = "cannot create the given directory: %s" % repr(errinfo)
            elif errno == 14:
                message = "cannot remove the given file: %s" % repr(errinfo)
            elif errno == 15:
                message = "cannot create the given file: %s" % repr(errinfo)
            elif errno == 16:
                message = "cannot transform the given file: %s" % repr(errinfo)
            elif errno == 17:
                message = "cannot update/replace the given file: %s" % repr(errinfo)
            elif errno == 18:
                message = "module lacks a license: %s" % repr(errinfo)
            elif errno == 19:
                message = "error when running subprocess: %s" % repr(errinfo)
            self.message = '[Errno %d] %s' % (errno, message)
        return self.message
