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
import copy
from typing import Any
from .constants import (
    MODES,
    TESTS,
    joinpath,
    remove_trailing_slashes,
)
from .GLError import GLError
from pygnulib.enums import CopyAction


#===============================================================================
# Define GLConfig class
#===============================================================================
class GLConfig:
    '''This class is used to store intermediate settings for all pygnulib
    classes. It contains all necessary attributes to setup any other class.
    By default all attributes are set to empty string, empty list or zero.
    The most common value, however, is a None value.'''

    table: dict[str, Any]

    def __init__(self,
                 tempdir: str | None = None,
                 destdir: str | None = None,
                 localpath: list[str] | None = None,
                 auxdir: str | None = None,
                 sourcebase: str | None = None,
                 m4base: str | None = None,
                 pobase: str | None = None,
                 docbase: str | None = None,
                 testsbase: str | None = None,
                 modules: list[str] | None = None,
                 avoids: list[str] | None = None,
                 files: list[str] | None = None,
                 incobsolete: bool | None = None,
                 incl_test_categories: list[int] | tuple[int] | None = None,
                 excl_test_categories: list[int] | tuple[int] | None = None,
                 libname: str | None = None,
                 lgpl: str | bool | None = None,
                 gpl: str | None = None,
                 gnu_make: bool | None = None,
                 makefile_name: str | None = None,
                 tests_makefile_name: str | None = None,
                 automake_subdir: bool | None = None,
                 automake_subdir_tests: bool | None = None,
                 libtool: bool | None = None,
                 conddeps: bool | None  = None,
                 macro_prefix: str | None = None,
                 podomain: str | None = None,
                 witness_c_macro: str | None = None,
                 vc_files: bool | None = None,
                 copymode: CopyAction | None = None,
                 lcopymode: CopyAction | None = None,
                 configure_ac: str | None = None,
                 ac_version: float | int | None = None,
                 libtests: bool | None = None,
                 single_configure: bool | None = None,
                 verbose: int | None = None,
                 dryrun: bool | None = None,
                 errors: bool | None = None) -> None:
        '''Create new GLConfig instance.'''
        self.table = dict()
        self.table['tempdir'] = tempdir
        # Check and store the attributes.
        # Remove trailing slashes from the directory names. This is necessary
        # for m4base (to avoid an error in func_import) and optional for the
        # others.
        # destdir
        self.resetDestDir()
        if destdir != None:
            self.setDestDir(destdir)
        # localpath
        self.resetLocalPath()
        if localpath != None:
            self.setLocalPath(localpath)
        # auxdir
        self.resetAuxDir()
        if auxdir != None:
            self.setAuxDir(auxdir)
        # sourcebase
        self.resetSourceBase()
        if sourcebase != None:
            self.setSourceBase(sourcebase)
        # m4base
        self.resetM4Base()
        if m4base != None:
            self.setM4Base(m4base)
        # pobase
        self.resetPoBase()
        if pobase != None:
            self.setPoBase(pobase)
        # docbase
        self.resetDocBase()
        if docbase != None:
            self.setDocBase(docbase)
        # testsbase
        self.resetTestsBase()
        if testsbase != None:
            self.setTestsBase(testsbase)
        # modules
        self.resetModules()
        if modules != None:
            self.setModules(modules)
        # avoids
        self.resetAvoids()
        if avoids != None:
            self.setAvoids(avoids)
        # files
        self.resetFiles()
        if files != None:
            self.setFiles(files)
        # incobsolete
        self.resetIncObsolete()
        if incobsolete != None:
            self.setIncObsolete(incobsolete)
        # test categories to include
        self.resetInclTestCategories()
        if incl_test_categories != None:
            self.setInclTestCategories(incl_test_categories)
        # test categories to exclude
        self.resetExclTestCategories()
        if excl_test_categories != None:
            self.setExclTestCategories(excl_test_categories)
        # libname
        self.resetLibName()
        if libname != None:
            self.setLibName(libname)
        # lgpl
        self.resetLGPL()
        if lgpl != None:
            self.setLGPL(lgpl)
        # gpl
        self.resetGPL()
        if gpl != None:
            self.setGPL(gpl)
        # gnu-make
        self.resetGnuMake()
        if gnu_make != None:
            self.setGnuMake(gnu_make)
        # makefile_name
        self.resetMakefileName()
        if makefile_name != None:
            self.setMakefileName(makefile_name)
        # tests_makefile_name
        self.resetTestsMakefileName()
        if tests_makefile_name != None:
            self.setTestsMakefileName(tests_makefile_name)
        # automake-subdir
        self.resetAutomakeSubdir()
        if automake_subdir != None:
            self.setAutomakeSubdir(automake_subdir)
        # automake_subdir_tests
        self.resetAutomakeSubdirTests()
        if automake_subdir_tests != None:
            self.setAutomakeSubdirTests(automake_subdir_tests)
        # libtool
        self.resetLibtool()
        if libtool != None:
            self.setLibtool(libtool)
        # conddeps
        self.resetCondDeps()
        if conddeps != None:
            self.setCondDeps(conddeps)
        # macro_prefix
        self.resetMacroPrefix()
        if macro_prefix != None:
            self.setMacroPrefix(macro_prefix)
        # podomain
        self.resetPoDomain()
        if podomain != None:
            self.setPoDomain(podomain)
        # witness_c_macro
        self.resetWitnessCMacro()
        if witness_c_macro != None:
            self.setWitnessCMacro(witness_c_macro)
        # vc_files
        self.resetVCFiles()
        if vc_files != None:
            self.setVCFiles(vc_files)
        # copymode (--symlink and --hardlink)
        self.resetCopyMode()
        if copymode == None:
            # Default to copying.
            copymode = CopyAction.Copy
        self.setCopyMode(copymode)
        # lcopymode (--local-symlink and --local-hardlink)
        self.resetLCopyMode()
        if lcopymode == None:
            # Default to copying.
            lcopymode = CopyAction.Copy
        self.setLCopyMode(lcopymode)
        # configure_ac
        self.resetAutoconfFile()
        if configure_ac != None:
            self.setAutoconfFile(configure_ac)
        # ac_version
        self.resetAutoconfVersion()
        if ac_version != None:
            self.setAutoconfVersion(ac_version)
        # libtests
        self.resetLibtests()
        if libtests != None:
            self.setLibtests(libtests)
        # single_configure
        self.resetSingleConfigure()
        if single_configure != None:
            self.setSingleConfigure(single_configure)
        # verbose
        self.resetVerbosity()
        if verbose != None:
            self.setVerbosity(verbose)
        # dryrun
        self.resetDryRun()
        if dryrun != None:
            self.setDryRun(dryrun)
        # errors
        self.resetErrors()
        if errors != None:
            self.setErrors(errors)

    # Define special methods.
    def __repr__(self) -> str:
        '''x.__repr__() <==> repr(x)'''
        return '<pygnulib.GLConfig>'

    def __getitem__(self, y: str) -> Any:
        '''x.__getitem__(y) <==> x[y]'''
        if y in self.table:
            return self.table[y]
        else:  # if y not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(y))

    def dictionary(self) -> dict[str, Any]:
        '''Return the configuration as a dict object.'''
        return dict(self.table)

    def copy(self) -> GLConfig:
        '''Return the copy of the configuration.'''
        table = copy.deepcopy(self)
        return table

    def update(self, other_config: GLConfig) -> None:
        '''Merges all non-default values from other_config into this config.'''
        if type(other_config) is not GLConfig:
            raise TypeError('other_config must be a GLConfig, not %s'
                            % type(other_config).__name__)
        dictionary = other_config.table
        result = dict()
        for key in dictionary:
            src = self.table[key]
            dest = dictionary[key]
            # Merge src and dest into a single value.
            if src == dest:
                value = src
            else:
                if self.isdefault(key, src):
                    value = dest
                else:  # if not self.isdefault(key, src)
                    if self.isdefault(key, dest):
                        value = src
                    else:  # if not self.isdefault(key, dest)
                        if key in ['modules', 'avoids',
                                   'incl_test_categories', 'excl_test_categories']:
                            value = sorted(set(src + dest))
                        else:
                            value = dest
            result[key] = value
        self.table = result

    def update_key(self, other_config: GLConfig, key: str) -> None:
        '''Copies the value for key from other_config into this config.'''
        if key in self.table:
            if type(other_config) is not GLConfig:
                raise TypeError('other_config must be a GLConfig, not %s'
                                % type(other_config).__name__)
            dictionary = other_config.table
            self.table[key] = dictionary[key]
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def default(self, key: str) -> Any:
        '''Return default value for the given key.'''
        if key in self.table:
            if key == 'libname':
                return 'libgnu'
            elif key == 'macro_prefix':
                return 'gl'
            elif key == 'include_guard_prefix':
                return 'GL'
            elif key == 'ac_version':
                return 2.64
            elif key == 'verbosity':
                return 0
            elif key in ['localpath', 'modules', 'avoids',
                         'incl_test_categories', 'excl_test_categories']:
                return []
            elif key in ['incobsolete', 'gnu_make',
                         'automake_subdir', 'automake_subdir_tests',
                         'libtests', 'dryrun']:
                return False
            elif key in ['copymode', 'lcopymode']:
                return CopyAction.Copy
            elif key in ['lgpl', 'gpl', 'libtool', 'conddeps', 'vc_files']:
                return None
            elif key == 'errors':
                return True
            else:  # otherwise
                return ''
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def isdefault(self, key: str, value: Any) -> bool:
        '''Check whether the value for the given key is a default value.'''
        if key in self.table:
            default = self.default(key)
            return value == default
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def keys(self) -> list[str]:
        '''Return list of keys.'''
        return list(self.table.keys())

    def values(self) -> list[Any]:
        '''Return list of values.'''
        return list(self.table.values())

    # Define destdir methods.
    def getDestDir(self) -> str:
        '''Return the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        return self.table['destdir']

    def setDestDir(self, destdir: str) -> None:
        '''Specify the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        if type(destdir) is str:
            if destdir:
                self.table['destdir'] = os.path.normpath(destdir)
        else:  # if destdir has not str type
            raise TypeError('destdir must be a string, not %s'
                            % type(destdir).__name__)

    def resetDestDir(self) -> None:
        '''Reset the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        self.table['destdir'] = ''

    # Define localpath methods.
    def getLocalPath(self) -> list[str]:
        '''Return a list of local override directories where to look up files
        before looking in gnulib's directory. The first one has the highest
        priority.'''
        return self.table['localpath']

    def setLocalPath(self, localpath: list[str]) -> None:
        '''Specify a list of local override directories where to look up files
        before looking in gnulib's directory. The first one has the highest
        priority.'''
        if type(localpath) is list:
            for dir in localpath:
                if type(dir) is not str:
                    raise TypeError('localpath element must be a string, not %s' % type(dir).__name__)
        else:
            raise TypeError('localpath must be a list, not %s' % type(localpath).__name__)
        self.table['localpath'] = [ remove_trailing_slashes(dir)
                                    for dir in localpath ]

    def resetLocalPath(self) -> None:
        '''Reset a list of local override directories where to look up files
        before looking in gnulib's directory.'''
        self.table['localpath'] = []

    # Define auxdir methods.
    def getAuxDir(self) -> str:
        '''Return directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        if self.table['auxdir']:
            return self.table['auxdir']
        return 'build-aux'

    def setAuxDir(self, auxdir: str) -> None:
        '''Specify directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        if type(auxdir) is str:
            if auxdir:
                self.table['auxdir'] = remove_trailing_slashes(auxdir)
        else:  # if type of auxdir is not str
            raise TypeError('auxdir must be a string, not %s'
                            % type(auxdir).__name__)

    def resetAuxDir(self) -> None:
        '''Reset directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        self.table['auxdir'] = ''

    # Define sourcebase methods.
    def getSourceBase(self) -> str:
        '''Return directory relative to destdir where source code is placed.'''
        return self.table['sourcebase']

    def setSourceBase(self, sourcebase: str) -> None:
        '''Specify directory relative to destdir where source code is placed.'''
        if type(sourcebase) is str:
            if sourcebase:
                self.table['sourcebase'] = remove_trailing_slashes(sourcebase)
        else:  # if type of sourcebase is not str
            raise TypeError('sourcebase must be a string, not %s'
                            % type(sourcebase).__name__)

    def resetSourceBase(self) -> None:
        '''Return directory relative to destdir where source code is placed.'''
        self.table['sourcebase'] = ''

    # Define m4base methods.
    def getM4Base(self) -> str:
        '''Return directory relative to destdir where *.m4 macros are placed.'''
        return self.table['m4base']

    def setM4Base(self, m4base: str) -> None:
        '''Specify directory relative to destdir where *.m4 macros are placed.'''
        if type(m4base) is str:
            if m4base:
                self.table['m4base'] = remove_trailing_slashes(m4base)
        else:  # if type of m4base is not str
            raise TypeError('m4base must be a string, not %s'
                            % type(m4base).__name__)

    def resetM4Base(self) -> None:
        '''Reset directory relative to destdir where *.m4 macros are placed.'''
        self.table['m4base'] = ''

    # Define pobase methods.
    def getPoBase(self) -> str:
        '''Return directory relative to destdir where *.po files are placed.'''
        return self.table['pobase']

    def setPoBase(self, pobase: str) -> None:
        '''Specify directory relative to destdir where *.po files are placed.'''
        if type(pobase) is str:
            if pobase:
                self.table['pobase'] = remove_trailing_slashes(pobase)
        else:  # if type of pobase is not str
            raise TypeError('pobase must be a string, not %s'
                            % type(pobase).__name__)

    def resetPoBase(self) -> None:
        '''Reset directory relative to destdir where *.po files are placed.'''
        self.table['pobase'] = ''

    # Define docbase methods.
    def getDocBase(self) -> str:
        '''Return directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        return self.table['docbase']

    def setDocBase(self, docbase: str) -> None:
        '''Specify directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        if type(docbase) is str:
            if docbase:
                self.table['docbase'] = remove_trailing_slashes(docbase)
        else:  # if type of docbase is not str
            raise TypeError('docbase must be a string, not %s'
                            % type(docbase).__name__)

    def resetDocBase(self) -> None:
        '''Reset directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        self.table['docbase'] = ''

    # Define testsbase methods.
    def getTestsBase(self) -> str:
        '''Return directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        return self.table['testsbase']

    def setTestsBase(self, testsbase: str) -> None:
        '''Specify directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        if type(testsbase) is str:
            if testsbase:
                self.table['testsbase'] = remove_trailing_slashes(testsbase)
        else:  # if type of testsbase is not str
            raise TypeError('testsbase must be a string, not %s'
                            % type(testsbase).__name__)

    def resetTestsBase(self) -> None:
        '''Reset directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        self.table['testsbase'] = ''

    # Define modules methods.
    def addModule(self, module: str) -> None:
        '''Add the module to the modules list.'''
        if type(module) is str:
            if module not in self.table['modules']:
                self.table['modules'].append(module)
        else:  # if module has not str type
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)

    def removeModule(self, module: str) -> None:
        '''Remove the module from the modules list.'''
        if type(module) is str:
            if module in self.table['modules']:
                self.table['modules'].remove(module)
        else:  # if module has not str type
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)

    def getModules(self) -> list[str]:
        '''Return the modules list.'''
        return list(self.table['modules'])

    def setModules(self, modules: list[str] | tuple[str]) -> None:
        '''Set the modules list.'''
        if type(modules) is list or type(modules) is tuple:
            old_modules = self.table['modules']
            self.table['modules'] = []
            for module in modules:
                try:  # Try to add each module
                    self.addModule(module)
                except TypeError as exc:
                    self.table['modules'] = old_modules
                    raise TypeError('each module must be a string') from exc
                except GLError:
                    self.table['modules'] = old_modules
                    raise
        else:  # if type of modules is not list or tuple
            raise TypeError('modules must be a list or a tuple, not %s'
                            % type(modules).__name__)

    def resetModules(self) -> None:
        '''Reset the list of the modules.'''
        self.table['modules'] = []

    # Define avoids methods.
    def addAvoid(self, module: str) -> None:
        '''Avoid including the given module. Useful if you have code that provides
        equivalent functionality.'''
        if type(module) is str:
            self.table['avoids'].append(module)
        else:  # if module has not str type
            raise TypeError('avoid must be a string, not %s'
                            % type(module).__name__)

    def removeAvoid(self, module: str) -> None:
        '''Remove the given module from the list of avoided modules.'''
        if type(module) is str:
            if module in self.table['avoids']:
                self.table['avoids'].remove(module)
        else:  # if module has not str type
            raise TypeError('avoid must be a string, not %s'
                            % type(module).__name__)

    def getAvoids(self) -> list[str]:
        '''Return the list of the avoided modules.'''
        return list(self.table['avoids'])

    def setAvoids(self, modules: list[str] | tuple[str]) -> None:
        '''Specify the modules which will be avoided.'''
        if type(modules) is list or type(modules) is tuple:
            old_avoids = self.table['avoids']
            self.table['avoids'] = []
            for module in modules:
                try:  # Try to add each module
                    self.addAvoid(module)
                except TypeError as exc:
                    self.table['avoids'] = old_avoids
                    raise TypeError('each module must be a string') from exc
                except GLError:
                    self.table['avoids'] = old_avoids
                    raise
        else:  # if type of modules is not list or tuple
            raise TypeError('modules must be a list or a tuple, not %s'
                            % type(modules).__name__)

    def resetAvoids(self) -> None:
        '''Reset the list of the avoided modules.'''
        self.table['avoids'] = []

    # Define files methods.
    def addFile(self, file: str) -> None:
        '''Add file to the list of files.'''
        if type(file) is str:
            if file not in self.table['files']:
                self.table['files'].append(file)
        else:  # if file has not str type
            raise TypeError('file must be a string, not %s'
                            % type(file).__name__)

    def removeFile(self, file: str) -> None:
        '''Remove the given file from the list of files.'''
        if type(file) is str:
            if file in self.table['files']:
                self.table['files'].remove(file)
        else:  # if file has not str type
            raise TypeError('file must be a string, not %s'
                            % type(file).__name__)

    def getFiles(self) -> list[str]:
        '''Return the list of files.'''
        return list(self.table['files'])

    def setFiles(self, files: list[str] | tuple[str]) -> None:
        '''Specify the list of files.'''
        if type(files) is list or type(files) is tuple:
            old_files = self.table['files']
            self.table['files'] = []
            for file in files:
                try:  # Try to add each file
                    self.addFile(file)
                except TypeError as exc:
                    self.table['files'] = old_files
                    raise TypeError('each file must be a string') from exc
                except GLError:
                    self.table['files'] = old_files
                    raise
        else:  # if type of files is not list or tuple
            raise TypeError('files must be a list or a tuple, not %s'
                            % type(files).__name__)

    def resetFiles(self) -> None:
        '''Reset the list of files.'''
        self.table['files'] = []

    # Define incobsolete methods.
    def checkIncObsolete(self) -> bool:
        '''Check if user enabled inclusion of obsolete dependencies.'''
        return self.table['incobsolete']

    def setIncObsolete(self, value: bool) -> None:
        '''Enable / disable inclusion of obsolete dependencies.'''
        if type(value) is bool:
            self.table['incobsolete'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetIncObsolete(self) -> None:
        '''Reset inclusion of obsolete dependencies.'''
        self.table['incobsolete'] = False

    # Define incl_test_categories methods
    def checkInclTestCategory(self, category: int) -> bool:
        '''Tests whether the given test category is included.'''
        if category in TESTS.values():
            return category in self.table['incl_test_categories']
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def enableInclTestCategory(self, category: int) -> None:
        '''Enable the given test category.'''
        if category in TESTS.values():
            if category not in self.table['incl_test_categories']:
                self.table['incl_test_categories'].append(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def disableInclTestCategory(self, category: int) -> None:
        '''Disable the given test category.'''
        if category in TESTS.values():
            if category in self.table['incl_test_categories']:
                self.table['incl_test_categories'].remove(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def setInclTestCategory(self, category: int, enable: bool) -> None:
        '''Enable or disable the given test category.'''
        if enable:
            self.enableInclTestCategory(category)
        else:
            self.disableInclTestCategory(category)

    def getInclTestCategories(self) -> list[int]:
        '''Return the test categories that should be included.
        To get the list of all test categories, use the TESTS variable.'''
        return list(self.table['incl_test_categories'])

    def setInclTestCategories(self, categories: list[int] | tuple[int]) -> None:
        '''Specify the test categories that should be included.'''
        if type(categories) is list or type(categories) is tuple:
            old_categories = self.table['incl_test_categories']
            self.table['incl_test_categories'] = []
            for category in categories:
                try:  # Try to enable each category
                    self.enableInclTestCategory(category)
                except TypeError as exc:
                    self.table['incl_test_categories'] = old_categories
                    raise TypeError('each category must be one of TESTS integers') from exc
        else:  # if type of categories is not list or tuple
            raise TypeError('categories must be a list or a tuple, not %s'
                            % type(categories).__name__)

    def resetInclTestCategories(self) -> None:
        '''Reset test categories.'''
        self.table['incl_test_categories'] = []

    # Define excl_test_categories methods
    def checkExclTestCategory(self, category: int) -> bool:
        '''Tests whether the given test category is excluded.'''
        if category in TESTS.values():
            return category in self.table['excl_test_categories']
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def enableExclTestCategory(self, category: int) -> None:
        '''Enable the given test category.'''
        if category in TESTS.values():
            if category not in self.table['excl_test_categories']:
                self.table['excl_test_categories'].append(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def disableExclTestCategory(self, category: int) -> None:
        '''Disable the given test category.'''
        if category in TESTS.values():
            if category in self.table['excl_test_categories']:
                self.table['excl_test_categories'].remove(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def getExclTestCategories(self) -> list[int]:
        '''Return the test categories that should be excluded.
        To get the list of all test categories, use the TESTS variable.'''
        return list(self.table['excl_test_categories'])

    def setExclTestCategories(self, categories: list[int] | tuple[int]) -> None:
        '''Specify the test categories that should be excluded.'''
        if type(categories) is list or type(categories) is tuple:
            old_categories = self.table['excl_test_categories']
            self.table['excl_test_categories'] = []
            for category in categories:
                try:  # Try to enable each category
                    self.enableExclTestCategory(category)
                except TypeError as exc:
                    self.table['excl_test_categories'] = old_categories
                    raise TypeError('each category must be one of TESTS integers') from exc
        else:  # if type of categories is not list or tuple
            raise TypeError('categories must be a list or a tuple, not %s'
                            % type(categories).__name__)

    def resetExclTestCategories(self) -> None:
        '''Reset test categories.'''
        self.table['excl_test_categories'] = []

    # Define libname methods.
    def getLibName(self) -> str:
        '''Return the library name.'''
        return self.table['libname']

    def setLibName(self, libname: str) -> None:
        '''Specify the library name.'''
        if type(libname) is str:
            if libname:
                self.table['libname'] = libname
        else:  # if type of libname is not str
            raise TypeError('libname must be a string, not %s'
                            % type(libname).__name__)

    def resetLibName(self) -> None:
        '''Reset the library name to 'libgnu'.'''
        self.table['libname'] = 'libgnu'

    # Define libtool methods.
    def checkLibtool(self) -> bool | None:
        '''Check if user enabled libtool rules.'''
        return self.table['libtool']

    def setLibtool(self, value: bool) -> None:
        '''Enable / disable libtool rules.'''
        if type(value) is bool:
            self.table['libtool'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetLibtool(self) -> None:
        '''Reset libtool rules.'''
        self.table['libtool'] = None

    # Define conddeps methods.
    def checkCondDeps(self) -> bool | None:
        '''Check if user enabled conditional dependencies.'''
        return self.table['conddeps']

    def setCondDeps(self, value: bool) -> None:
        '''Enable / disable conditional dependencies (may save configure time and object code).'''
        if type(value) is bool:
            self.table['conddeps'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetCondDeps(self) -> None:
        '''Reset conditional dependencies (may save configure time and object code).'''
        self.table['conddeps'] = None

    # Define lgpl methods.
    def getLGPL(self) -> str | bool | None:
        '''Check for abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        return self.table['lgpl']

    def setLGPL(self, lgpl: str | bool | None) -> None:
        '''Abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        if lgpl in [None, True, '2', '3orGPLv2', '3']:
            self.table['lgpl'] = lgpl
        else:
            raise TypeError('invalid LGPL version: %s' % repr(lgpl))

    def resetLGPL(self) -> None:
        '''Disable abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        self.table['lgpl'] = None

    # Define gpl methods.
    def getGPL(self) -> str | None:
        '''Check for abort if modules aren't available under the GPL.
        Default value is None, which means that gpl is disabled.'''
        return self.table['gpl']

    def setGPL(self, gpl: str | bool | None) -> None:
        '''Abort if modules aren't available under the GPL.
        Default value is None, which means that gpl is disabled.'''
        if gpl in [None, '2', '3']:
            self.table['gpl'] = gpl
        else:
            raise TypeError('invalid GPL version: %s' % repr(gpl))

    def resetGPL(self) -> None:
        '''Disable abort if modules aren't available under the GPL.
        Default value is None, which means that gpl is disabled.'''
        self.table['gpl'] = None

    # Define gnu-make methods.
    def getGnuMake(self) -> bool:
        '''Return a boolean value describing whether the --gnu-make argument
        was used.'''
        return self.table['gnu_make']

    def setGnuMake(self, value: bool) -> None:
        '''Set the --gnu-make argument as if it were invoked using the
        command-line or disable it.'''
        if type(value) is bool:
            self.table['gnu_make'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetGnuMake(self) -> None:
        '''Reset the --gnu-make argument to its default. This feature must be
        explicitly enabled by programs who utilize GNU Make features instead
        of Autmake.'''
        self.table['gnu_make'] = False

    def getModuleIndicatorPrefix(self) -> str:
        '''Return module_indicator_prefix to use inside GLEmiter class.'''
        return self.getIncludeGuardPrefix()

    # Define macro_prefix and include_guard_prefix methods.
    # The include_guard_prefix is a replacement for ${gl_include_guard_prefix}.
    # It is determined from the macro_prefix.
    def getMacroPrefix(self) -> str:
        '''Return the prefix of the macros 'gl_EARLY' and 'gl_INIT'.
        Default macro_prefix is 'gl'.'''
        return self.table['macro_prefix']

    def getIncludeGuardPrefix(self) -> str:
        '''Return the replacement for ${gl_include_guard_prefix}.'''
        return self.table['include_guard_prefix']

    def setMacroPrefix(self, macro_prefix: str) -> None:
        '''Specify the prefix of the macros 'gl_EARLY' and 'gl_INIT'.
        Default macro_prefix is 'gl'.'''
        if type(macro_prefix) is str:
            if macro_prefix:
                self.table['macro_prefix'] = macro_prefix
        else:  # if type of macro_prefix is not str
            raise TypeError('macro_prefix must be a string, not %s'
                            % type(macro_prefix).__name__)
        if macro_prefix == 'gl':
            include_guard_prefix = 'GL'
        else:  # macro_prefix != 'gl'
            include_guard_prefix = 'GL_%s' % macro_prefix.upper()
        self.table['include_guard_prefix'] = include_guard_prefix

    def resetMacroPrefix(self) -> None:
        '''Reset the prefix of the macros 'gl_EARLY' and 'gl_INIT'.
        Default macro_prefix is 'gl'.'''
        self.table['macro_prefix'] = 'gl'
        self.table['include_guard_prefix'] = 'GL'

    # Define makefile_name methods.
    def getMakefileName(self) -> str:
        '''Return the name of makefile in automake syntax in the sourcebase directory.
        Default is 'Makefile.am'.'''
        return self.table['makefile_name']

    def setMakefileName(self, makefile_name: str) -> None:
        '''Specify the name of makefile in automake syntax in the sourcebase directory.
        Default is 'Makefile.am'.'''
        if type(makefile_name) is str:
            if makefile_name:
                self.table['makefile_name'] = makefile_name
        else:  # if type of makefile_name is not str
            raise TypeError('makefile_name must be a string, not %s'
                            % type(makefile_name).__name__)

    def resetMakefileName(self) -> None:
        '''Reset the name of makefile in automake syntax in the sourcebase directory.
        Default is 'Makefile.am'.'''
        self.table['makefile_name'] = ''

    # Define tests_makefile_name methods.
    def getTestsMakefileName(self) -> str:
        '''Return the name of makefile in automake syntax in the testsbase directory.
        Default is the the value of 'makefile_name'.'''
        return self.table['tests_makefile_name']

    def setTestsMakefileName(self, tests_makefile_name: str) -> None:
        '''Specify the name of makefile in automake syntax in the testsbase directory.
        Default is the value of 'makefile_name'.'''
        if type(tests_makefile_name) is str:
            if tests_makefile_name:
                self.table['tests_makefile_name'] = tests_makefile_name
        else:  # if type of tests_makefile_name is not str
            raise TypeError('tests_makefile_name must be a string, not %s'
                            % type(tests_makefile_name).__name__)

    def resetTestsMakefileName(self) -> None:
        '''Reset the name of makefile in automake syntax in the testsbase directory.
        Default is the value of 'makefile_name'.'''
        self.table['tests_makefile_name'] = ''

    # Define automake_subdir methods.
    def getAutomakeSubdir(self) -> bool:
        '''Return a bool describing whether --automake-subdir is in use
        or not.'''
        return self.table['automake_subdir']

    def setAutomakeSubdir(self, automake_subdir: bool) -> None:
        '''Set the value describing whether --automake-subdir is in use.'''
        if type(automake_subdir) is bool:
            self.table['automake_subdir'] = automake_subdir
        else:  # if type of automake_subdir is not bool
            raise TypeError('automake_subdir must be a bool, not %s'
                            % type(automake_subdir).__name__)

    def resetAutomakeSubdir(self) -> None:
        '''Reset the value describing whether --automake-subdir is in use.
        Default is False.'''
        self.table['automake_subdir'] = False

    # Define automake_subdir_tests methods.
    def getAutomakeSubdirTests(self) -> bool:
        '''Return a bool describing whether --automake-subdir-tests is in use
        or not.'''
        return self.table['automake_subdir_tests']

    def setAutomakeSubdirTests(self, automake_subdir_tests: bool) -> None:
        '''Set the value describing whether --automake-subdir-tests is in use.'''
        if type(automake_subdir_tests) is bool:
            self.table['automake_subdir_tests'] = automake_subdir_tests
        else:  # if type of automake_subdir_tests is not bool
            raise TypeError('automake_subdir_tests must be a bool, not %s'
                            % type(automake_subdir_tests).__name__)

    def resetAutomakeSubdirTests(self) -> None:
        '''Reset the value describing whether --automake-subdir-tests is in use.
        Default is False.'''
        self.table['automake_subdir_tests'] = False

    # Define podomain methods.
    def getPoDomain(self) -> str:
        '''Return the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        return self.table['podomain']

    def setPoDomain(self, podomain: str) -> None:
        '''Specify the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        if type(podomain) is str:
            if podomain:
                self.table['podomain'] = podomain
        else:  # if type of podomain is not str
            raise TypeError('podomain must be a string, not %s'
                            % type(podomain).__name__)

    def resetPoDomain(self) -> None:
        '''Reset the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        self.table['podomain'] = ''

    # Define witness_c_macro methods.
    def getWitnessCMacro(self) -> str:
        '''Return the C macro that is defined when the sources in this directory
        are compiled or used.'''
        return self.table['witness_c_macro']

    def setWitnessCMacro(self, witness_c_macro: str) -> None:
        '''Specify the C macro that is defined when the sources in this directory
        are compiled or used.'''
        if type(witness_c_macro) is str:
            if witness_c_macro:
                self.table['witness_c_macro'] = witness_c_macro
        else:  # if type of witness_c_macro is not str
            raise TypeError('witness_c_macro must be a string, not %s'
                            % type(witness_c_macro).__name__)

    def resetWitnessCMacro(self) -> None:
        '''Return the C macro that is defined when the sources in this directory
        are compiled or used.'''
        self.table['witness_c_macro'] = ''

    # Define vc_files methods.
    def checkVCFiles(self) -> bool | None:
        '''Check if update of the version control files is enabled or disabled.'''
        return self.table['vc_files']

    def setVCFiles(self, value: bool) -> None:
        '''Enable /disable update of the version control files.'''
        if type(value) is bool:
            self.table['vc_files'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetVCFiles(self) -> None:
        '''Reset update of the version control files and set it to None.'''
        self.table['vc_files'] = None

    # Define configure_ac methods.
    def getAutoconfFile(self) -> str:
        '''Return path of autoconf file relative to destdir.'''
        return self.table['configure_ac']

    def setAutoconfFile(self, configure_ac: str) -> None:
        '''Specify path of autoconf file relative to destdir.'''
        if type(configure_ac) is str:
            if configure_ac:
                self.table['configure_ac'] = configure_ac
        else:  # if type of configure_ac is not str
            raise TypeError('configure_ac must be a string, not %s'
                            % type(configure_ac).__name__)

    def resetAutoconfFile(self) -> None:
        '''Reset path of autoconf file relative to destdir.'''
        configure_ac = ''
        if os.path.isfile(joinpath(self.table['destdir'], 'configure.ac')):
            configure_ac = joinpath(self.table['destdir'], 'configure.ac')
        elif os.path.isfile(joinpath(self.table['destdir'], 'configure.in')):
            configure_ac = joinpath(self.table['destdir'], 'configure.in')
        self.table['configure_ac'] = configure_ac

    # Define ac_version methods.
    def getAutoconfVersion(self) -> float:
        '''Return preferred autoconf version. Default value is 2.64.'''
        return self.table['ac_version']

    def setAutoconfVersion(self, ac_version: float | int) -> None:
        '''Specify preferred autoconf version. Default value is 2.64.'''
        if type(ac_version) is float or type(ac_version) is int:
            self.table['ac_version'] = float(ac_version)
        else:  # if ac_version has not int or float type
            raise TypeError('ac_version must be an int or a float, not %s'
                            % type(ac_version).__name__)

    def resetAutoconfVersion(self) -> None:
        '''Specify preferred autoconf version. Default value is 2.64.'''
        self.table['ac_version'] = 2.64

    # Define copymode methods.
    def checkCopyMode(self) -> CopyAction:
        '''Check if pygnulib will copy files, create symlinks, or create hard links.'''
        return self.table['copymode']

    def setCopyMode(self, value: CopyAction) -> None:
        '''Change the method used for copying / linking files.'''
        if type(value) is CopyAction:
            self.table['copymode'] = value
        else:  # if type(value) is not CopyAction
            raise TypeError('value must be a CopyAction, not %s'
                            % type(value).__name__)

    def resetCopyMode(self) -> None:
        '''Reset the method used for creating files to copying instead of linking.'''
        self.table['copymode'] = CopyAction.Copy

    # Define lcopymode methods.
    def checkLCopyMode(self) -> CopyAction:
        '''Check if pygnulib will copy files, create symlinks, or create hard links,
        only for files from the local override directories.'''
        return self.table['lcopymode']

    def setLCopyMode(self, value: CopyAction) -> None:
        '''Change the method used for copying / linking files, only for files from
        the local override directories.'''
        if type(value) is CopyAction:
            self.table['lcopymode'] = value
        else:  # if type(value) is not CopyAction
            raise TypeError('value must be a CopyAction, not %s'
                            % type(value).__name__)

    def resetLCopyMode(self) -> None:
        '''Reset the method used for creating files to copying instead of linking,
        only for files from the local override directories.'''
        self.table['lcopymode'] = CopyAction.Copy

    # Define verbosity methods.
    def getVerbosity(self) -> int:
        '''Get verbosity level.'''
        return self.table['verbosity']

    def decreaseVerbosity(self) -> None:
        '''Decrease verbosity level.'''
        if self.table['verbosity'] > MODES['verbose-min']:
            self.table['verbosity'] -= 1

    def increaseVerbosity(self) -> None:
        '''Increase verbosity level.'''
        if self.table['verbosity'] < MODES['verbose-max']:
            self.table['verbosity'] += 1

    def setVerbosity(self, verbose: int) -> None:
        '''Set verbosity level to verbose, where -2 <= verbose <= 2.
        If verbosity level is less than -2, verbosity level will be set to -2.
        If verbosity level is greater than 2, verbosity level will be set to 2.'''
        if type(verbose) is int:
            if MODES['verbose-min'] <= verbose <= MODES['verbose-max']:
                self.table['verbosity'] = verbose
            elif verbose < MODES['verbose-min']:
                self.table['verbosity'] = MODES['verbose-min']
            elif verbose > MODES['verbose-max']:
                self.table['verbosity'] = MODES['verbose-max']
        else:  # if type(verbose) is not int
            raise TypeError('verbosity must be an int, not %s'
                            % type(verbose).__name__)

    def resetVerbosity(self) -> None:
        '''Reset verbosity level.'''
        self.table['verbosity'] = 0

    # Define libtests methods.
    def checkLibtests(self) -> bool:
        '''Return True if a testsbase/libtests.a is needed.'''
        return self.table['libtests']

    def setLibtests(self, value: bool) -> None:
        '''Specifies whether testsbase/libtests.a is needed.'''
        if type(value) is bool:
            self.table['libtests'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetLibtests(self) -> None:
        '''Reset status of testsbase/libtests.a.'''
        self.table['libtests'] = False

    # Define single_configure methods.
    def checkSingleConfigure(self) -> bool:
        '''Check whether single configure file should be generated.'''
        return self.table['single_configure']

    def setSingleConfigure(self, value: bool) -> None:
        '''Enable / disable generation of the single configure file.'''
        if type(value) is bool:
            self.table['single_configure'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetSingleConfigure(self) -> None:
        '''Reset status of the single configure file generation.'''
        self.table['single_configure'] = False

    # Define dryrun methods.
    def checkDryRun(self) -> bool:
        '''Check whether dryrun is enabled.'''
        return self.table['dryrun']

    def setDryRun(self, value: bool) -> None:
        '''Enable / disable dryrun mode.'''
        if type(value) is bool:
            self.table['dryrun'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetDryRun(self) -> None:
        '''Reset status of dryrun mode.'''
        self.table['dryrun'] = False

    # Define errors methods.
    def checkErrors(self) -> bool:
        '''Check if GLError will be raised in non-critical situations.'''
        return self.table['errors']

    def setErrors(self, value: bool) -> None:
        '''Enable / disable raising GLError in non-critical situations.'''
        if type(value) is bool:
            self.table['errors'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetErrors(self) -> None:
        '''Reset status of raising GLError in non-critical situations.'''
        self.table['errors'] = False
