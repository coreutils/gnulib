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

'''An easy access to pygnulib classes.'''

#===============================================================================
# Define global imports
#===============================================================================
__all__ = list()

try:
    # Constants
    from . import constants

    # Main classes
    from .GLConfig import GLConfig
    from .GLError import GLError
    from .GLInfo import GLInfo

    # File system
    from .GLFileSystem import GLFileSystem
    from .GLFileSystem import GLFileAssistant

    # Module system
    from .GLModuleSystem import GLModule
    from .GLModuleSystem import GLModuleSystem
    from .GLModuleSystem import GLModuleTable

    # Different modes
    from .GLImport import GLImport
    from .GLEmiter import GLEmiter
    from .GLTestDir import GLTestDir
    from .GLTestDir import GLMegaTestDir

    # Other modules
    from .GLMakefileTable import GLMakefileTable
except ValueError as error:
    # Constants
    import constants

    # Main classes
    from GLConfig import GLConfig
    from GLError import GLError
    from GLInfo import GLInfo

    # File system
    from GLFileSystem import GLFileSystem
    from GLFileSystem import GLFileAssistant

    # Module system
    from GLModuleSystem import GLModule
    from GLModuleSystem import GLModuleSystem
    from GLModuleSystem import GLModuleTable

    # Different modes
    from GLImport import GLImport
    from GLEmiter import GLEmiter
    from GLTestDir import GLTestDir
    from GLTestDir import GLMegaTestDir

    # Other modules
    from GLMakefileTable import GLMakefileTable

# Append modules to namespace.
__all__ += ['GLConfig', 'GLError', 'GLInfo']
__all__ += ['GLFileSystem', 'GLFileAssistant']
__all__ += ['GLModule', 'GLModuleSystem', 'GLModuleTable']
__all__ += ['GLImport', 'GLEmiter', 'GLTestDir']
__all__ += ['GLMakefileTable']

#===============================================================================
# Define module information
#===============================================================================
__author__ = constants.__author__
__license__ = constants.__license__
__copyright__ = constants.__copyright__
