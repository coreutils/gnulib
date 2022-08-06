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
import copy
import tempfile
from . import constants
from .GLError import GLError


#===============================================================================
# Define module information
#===============================================================================
__author__ = constants.__author__
__license__ = constants.__license__
__copyright__ = constants.__copyright__


#===============================================================================
# Define global constants
#===============================================================================
MODES = constants.MODES
TESTS = constants.TESTS
joinpath = constants.joinpath
relpath = constants.relativize
remove_trailing_slashes = constants.remove_trailing_slashes
isfile = os.path.isfile
normpath = os.path.normpath


#===============================================================================
# Define GLConfig class
#===============================================================================
class GLConfig(object):
    '''This class is used to store intermediate settings for all pygnulib
    classes. It contains all necessary attributes to setup any other class.
    By default all attributes are set to empty string, empty list or zero.
    The most common value, however, is a None value.'''

    def __init__(self, destdir=None, localpath=None, auxdir=None,
                 sourcebase=None, m4base=None, pobase=None, docbase=None, testsbase=None,
                 modules=None, avoids=None, files=None,
                 incl_test_categories=None, excl_test_categories=None, libname=None,
                 lgpl=None, makefile_name=None, libtool=None, conddeps=None, macro_prefix=None,
                 podomain=None, witness_c_macro=None, vc_files=None, symbolic=None,
                 lsymbolic=None, configure_ac=None, ac_version=None,
                 libtests=None, single_configure=None, verbose=None, dryrun=None,
                 errors=None):
        '''GLConfig.__init__(arguments) -> GLConfig

        Create new GLConfig instance.'''
        self.table = dict()
        self.table['tempdir'] = tempfile.mkdtemp()
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
        # makefile_name
        self.resetMakefileName()
        if makefile_name != None:
            self.setMakefileName(makefile_name)
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
        # symbolic
        self.resetSymbolic()
        if symbolic != None:
            self.setSymbolic(symbolic)
        # lsymbolic
        self.resetLSymbolic()
        if lsymbolic != None:
            self.setLSymbolic(lsymbolic)
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
    def __repr__(self):
        '''x.__repr__() <==> repr(x)'''
        return '<pygnulib.GLConfig>'

    def __getitem__(self, y):
        '''x.__getitem__(y) <==> x[y]'''
        if y in self.table:
            result = self.table[y]
            if type(y) is list:
                result = list(self.table[y])
            if y == "auxdir":
                if self.table['auxdir']:
                    return self.table['auxdir']
                return "build-aux"
            return self.table[y]
        else:  # if y not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(y))

    def dictionary(self):
        '''Return the configuration as a dict object.'''
        return dict(self.table)

    def copy(self):
        '''Return the copy of the configuration.'''
        table = copy.deepcopy(self)
        return table

    def update(self, dictionary):
        '''Specify the dictionary whose keys will be used to update config.'''
        if type(dictionary) is not GLConfig:
            raise TypeError('dictionary must be a GLConfig, not %s'
                            % type(dictionary).__name__)
        dictionary = dict(dictionary.table)
        result = dict()
        for key in dictionary:
            src = self.table[key]
            dest = dictionary[key]
            result[key] = src
            if src != dest:
                if self.isdefault(key, src):
                    result[key] = dest
                else:  # if not self.isdefault(key, src)
                    result[key] == src
                    if not self.isdefault(key, dest):
                        if key in ['modules', 'avoids', 'tests']:
                            dest = sorted(set(src + dest))
                        result[key] = dest
        self.table = dict(result)

    def update_key(self, dictionary, key):
        '''Update the given key using value from the given dictionary.'''
        if key in self.table:
            if type(dictionary) is not GLConfig:
                raise TypeError('dictionary must be a GLConfig, not %s'
                                % type(dictionary).__name__)
            dictionary = dict(dictionary.table)
            self.table[key] = dictionary[key]
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def default(self, key):
        '''Return default value for the given key.'''
        if key in self.table:
            if key == 'libname':
                return 'libgnu'
            elif key == 'macro_prefix':
                return 'gl'
            elif key == 'include_guard_prefix':
                return 'GL'
            elif key == 'ac_version':
                return 2.59
            elif key == 'verbosity':
                return 0
            elif key == 'copyrights':
                return True
            elif key in ['modules', 'avoids', 'tests', 'incl_test_categories', 'excl_test_categories']:
                return list()
            elif key in ['libtool', 'lgpl', 'conddeps', 'symbolic', 'lsymbolic',
                         'libtests', 'dryrun']:
                return False
            elif key == 'vc_files':
                return None
            elif key == 'errors':
                return True
            else:  # otherwise
                return ''
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def isdefault(self, key, value):
        '''Check whether the value for the given key is a default value.'''
        if key in self.table:
            default = self.default(key)
            return value == default
        else:  # if key not in self.table
            raise KeyError('GLConfig does not contain key: %s' % repr(key))

    def keys(self):
        '''Return list of keys.'''
        return list(self.table.keys())

    def values(self):
        '''Return list of values.'''
        return list(self.table.values())

    # Define destdir methods.
    def getDestDir(self):
        '''Return the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        return self.table['destdir']

    def setDestDir(self, destdir):
        '''Specify the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        if type(destdir) is str:
            if destdir:
                self.table['destdir'] = os.path.normpath(destdir)
        else:  # if destdir has not str type
            raise TypeError('destdir must be a string, not %s'
                            % type(destdir).__name__)

    def resetDestDir(self):
        '''Reset the target directory. For --import, this specifies where your
        configure.ac can be found. Defaults to current directory.'''
        self.table['destdir'] = ''

    # Define localpath methods.
    def getLocalPath(self):
        '''Return a list of local override directories where to look up files
        before looking in gnulib's directory. The first one has the highest
        priority.'''
        return self.table['localpath']

    def setLocalPath(self, localpath):
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

    def resetLocalPath(self):
        '''Reset a list of local override directories where to look up files
        before looking in gnulib's directory.'''
        self.table['localpath'] = []

    # Define auxdir methods.
    def getAuxDir(self):
        '''Return directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        if self.table['auxdir']:
            return self.table['auxdir']
        return "build-aux"

    def setAuxDir(self, auxdir):
        '''Specify directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        if type(auxdir) is str:
            if auxdir:
                self.table['auxdir'] = remove_trailing_slashes(auxdir)
        else:  # if type of auxdir is not str
            raise TypeError('auxdir must be a string, not %s'
                            % type(auxdir).__name__)

    def resetAuxDir(self):
        '''Reset directory relative to --dir where auxiliary build tools are
        placed. Default comes from configure.ac or configure.in.'''
        self.table['auxdir'] = ''

    # Define sourcebase methods.
    def getSourceBase(self):
        '''Return directory relative to destdir where source code is placed.'''
        return self.table['sourcebase']

    def setSourceBase(self, sourcebase):
        '''Specify directory relative to destdir where source code is placed.'''
        if type(sourcebase) is str:
            if sourcebase:
                self.table['sourcebase'] = remove_trailing_slashes(sourcebase)
        else:  # if type of sourcebase is not str
            raise TypeError('sourcebase must be a string, not %s'
                            % type(sourcebase).__name__)

    def resetSourceBase(self):
        '''Return directory relative to destdir where source code is placed.'''
        self.table['sourcebase'] = ''

    # Define m4base methods.
    def getM4Base(self):
        '''Return directory relative to destdir where *.m4 macros are placed.'''
        return self.table['m4base']

    def setM4Base(self, m4base):
        '''Specify directory relative to destdir where *.m4 macros are placed.'''
        if type(m4base) is str:
            if m4base:
                self.table['m4base'] = remove_trailing_slashes(m4base)
        else:  # if type of m4base is not str
            raise TypeError('m4base must be a string, not %s'
                            % type(m4base).__name__)

    def resetM4Base(self):
        '''Reset directory relative to destdir where *.m4 macros are placed.'''
        self.table['m4base'] = ''

    # Define pobase methods.
    def getPoBase(self):
        '''Return directory relative to destdir where *.po files are placed.'''
        return self.table['pobase']

    def setPoBase(self, pobase):
        '''Specify directory relative to destdir where *.po files are placed.'''
        if type(pobase) is str:
            if pobase:
                self.table['pobase'] = remove_trailing_slashes(pobase)
        else:  # if type of pobase is not str
            raise TypeError('pobase must be a string, not %s'
                            % type(pobase).__name__)

    def resetPoBase(self):
        '''Reset directory relative to destdir where *.po files are placed.'''
        self.table['pobase'] = ''

    # Define docbase methods.
    def getDocBase(self):
        '''Return directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        return self.table['docbase']

    def setDocBase(self, docbase):
        '''Specify directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        if type(docbase) is str:
            if docbase:
                self.table['docbase'] = remove_trailing_slashes(docbase)
        else:  # if type of docbase is not str
            raise TypeError('docbase must be a string, not %s'
                            % type(docbase).__name__)

    def resetDocBase(self):
        '''Reset directory relative to destdir where doc files are placed.
        Default value for this variable is 'doc').'''
        self.table['docbase'] = ''

    # Define testsbase methods.
    def getTestsBase(self):
        '''Return directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        return self.table['testsbase']

    def setTestsBase(self, testsbase):
        '''Specify directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        if type(testsbase) is str:
            if testsbase:
                self.table['testsbase'] = remove_trailing_slashes(testsbase)
        else:  # if type of testsbase is not str
            raise TypeError('testsbase must be a string, not %s'
                            % type(testsbase).__name__)

    def resetTestsBase(self):
        '''Reset directory relative to destdir where unit tests are placed.
        Default value for this variable is 'tests').'''
        self.table['testsbase'] = ''

    # Define modules methods.
    def addModule(self, module):
        '''Add the module to the modules list.'''
        if type(module) is str:
            if module not in self.table['modules']:
                self.table['modules'] += [module]
        else:  # if module has not str type
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)

    def removeModule(self, module):
        '''Remove the module from the modules list.'''
        if type(module) is str:
            if module in self.table['modules']:
                self.table['modules'].remove(module)
        else:  # if module has not str type
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)

    def getModules(self):
        '''Return the modules list.'''
        return list(self.table['modules'])

    def setModules(self, modules):
        '''Set the modules list.'''
        if type(modules) is list or type(modules) is tuple:
            old_modules = self.table['modules']
            self.table['modules'] = list()
            for module in modules:
                try:  # Try to add each module
                    self.addModule(module)
                except TypeError as error:
                    self.table['modules'] = old_modules
                    raise TypeError('each module must be a string')
                except GLError:
                    self.table['modules'] = old_modules
                    raise
        else:  # if type of modules is not list or tuple
            raise TypeError('modules must be a list or a tuple, not %s'
                            % type(modules).__name__)

    def resetModules(self):
        '''Reset the list of the modules.'''
        self.table['modules'] = list()

    # Define avoids methods.
    def addAvoid(self, module):
        '''Avoid including the given module. Useful if you have code that provides
        equivalent functionality.'''
        if type(module) is str:
            if module not in self.table['avoids']:
                self.table['avoids'].append(module)
        else:  # if module has not str type
            raise TypeError('avoid must be a string, not %s'
                            % type(module).__name__)

    def removeAvoid(self, module):
        '''Remove the given module from the list of avoided modules.'''
        if type(module) is str:
            if module in self.table['avoids']:
                self.table['avoids'].remove(module)
        else:  # if module has not str type
            raise TypeError('avoid must be a string, not %s'
                            % type(module).__name__)

    def getAvoids(self):
        '''Return the list of the avoided modules.'''
        return list(self.table['avoids'])

    def setAvoids(self, modules):
        '''Specify the modules which will be avoided.'''
        if type(modules) is list or type(modules) is tuple:
            old_avoids = self.table['avoids']
            self.table['avoids'] = list()
            for module in modules:
                try:  # Try to add each module
                    self.addAvoid(module)
                except TypeError as error:
                    self.table['avoids'] = old_avoids
                    raise TypeError('each module must be a string')
                except GLError:
                    self.table['avoids'] = old_avoids
                    raise
        else:  # if type of modules is not list or tuple
            raise TypeError('modules must be a list or a tuple, not %s'
                            % type(modules).__name__)

    def resetAvoids(self):
        '''Reset the list of the avoided modules.'''
        self.table['avoids'] = list()

    # Define files methods.
    def addFile(self, file):
        '''Add file to the list of files.'''
        if type(file) is str:
            if file not in self.table['files']:
                self.table['files'].append(file)
        else:  # if file has not str type
            raise TypeError('file must be a string, not %s'
                            % type(file).__name__)

    def removeFile(self, file):
        '''Remove the given file from the list of files.'''
        if type(file) is str:
            if file in self.table['files']:
                self.table['files'].remove(file)
        else:  # if file has not str type
            raise TypeError('file must be a string, not %s'
                            % type(file).__name__)

    def getFiles(self):
        '''Return the list of the fileed files.'''
        return list(self.table['files'])

    def setFiles(self, files):
        '''Specify the list of files.'''
        if type(files) is list or type(files) is tuple:
            old_files = self.table['files']
            self.table['files'] = list()
            for file in files:
                try:  # Try to add each file
                    self.addFile(file)
                except TypeError as error:
                    self.table['files'] = old_files
                    raise TypeError('each file must be a string')
                except GLError:
                    self.table['files'] = old_files
                    raise
        else:  # if type of files is not list or tuple
            raise TypeError('files must be a list or a tuple, not %s'
                            % type(files).__name__)

    def resetFiles(self):
        '''Reset the list of files.'''
        self.table['files'] = list()

    # Define incl_test_categories methods
    def checkInclTestCategory(self, category):
        '''Tests whether the given test category is included.'''
        if category in TESTS.values():
            return category in self.table['incl_test_categories']
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def enableInclTestCategory(self, category):
        '''Enable the given test category.'''
        if category in TESTS.values():
            if category not in self.table['incl_test_categories']:
                self.table['incl_test_categories'].append(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def disableInclTestCategory(self, category):
        '''Disable the given test category.'''
        if category in TESTS.values():
            if category in self.table['incl_test_categories']:
                self.table['incl_test_categories'].remove(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def setInclTestCategory(self, category, enable):
        '''Enable or disable the given test category.'''
        if enable:
            self.enableInclTestCategory(category)
        else:
            self.disableInclTestCategory(category)

    def getInclTestCategories(self):
        '''Return the test categories that should be included.
        To get the list of all test categories, use the TESTS variable.'''
        return list(self.table['incl_test_categories'])

    def setInclTestCategories(self, categories):
        '''Specify the test categories that should be included.'''
        if type(categories) is list or type(categories) is tuple:
            old_categories = self.table['incl_test_categories']
            self.table['incl_test_categories'] = list()
            for category in categories:
                try:  # Try to enable each category
                    self.enableInclTestCategory(category)
                except TypeError as error:
                    self.table['incl_test_categories'] = old_categories
                    raise TypeError('each category must be one of TESTS integers')
        else:  # if type of categories is not list or tuple
            raise TypeError('categories must be a list or a tuple, not %s'
                            % type(categories).__name__)

    def resetInclTestCategories(self):
        '''Reset test categories.'''
        self.table['incl_test_categories'] = list()

    # Define excl_test_categories methods
    def checkExclTestCategory(self, category):
        '''Tests whether the given test category is excluded.'''
        if category in TESTS.values():
            return category in self.table['excl_test_categories']
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def enableExclTestCategory(self, category):
        '''Enable the given test category.'''
        if category in TESTS.values():
            if category not in self.table['excl_test_categories']:
                self.table['excl_test_categories'].append(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def disableExclTestCategory(self, category):
        '''Disable the given test category.'''
        if category in TESTS.values():
            if category in self.table['excl_test_categories']:
                self.table['excl_test_categories'].remove(category)
        else:  # if category is not in TESTS
            raise TypeError('unknown category: %s' % repr(category))

    def getExclTestCategories(self):
        '''Return the test categories that should be excluded.
        To get the list of all test categories, use the TESTS variable.'''
        return list(self.table['excl_test_categories'])

    def setExclTestCategories(self, categories):
        '''Specify the test categories that should be excluded.'''
        if type(categories) is list or type(categories) is tuple:
            old_categories = self.table['excl_test_categories']
            self.table['excl_test_categories'] = list()
            for category in categories:
                try:  # Try to enable each category
                    self.enableExclTestCategory(category)
                except TypeError as error:
                    self.table['excl_test_categories'] = old_categories
                    raise TypeError('each category must be one of TESTS integers')
        else:  # if type of categories is not list or tuple
            raise TypeError('categories must be a list or a tuple, not %s'
                            % type(categories).__name__)

    def resetExclTestCategories(self):
        '''Reset test categories.'''
        self.table['excl_test_categories'] = list()

    # Define libname methods.
    def getLibName(self):
        '''Return the library name.'''
        return self.table['libname']

    def setLibName(self, libname):
        '''Specify the library name.'''
        if type(libname) is str:
            if libname:
                self.table['libname'] = libname
        else:  # if type of libname is not str
            raise TypeError('libname must be a string, not %s'
                            % type(libname).__name__)

    def resetLibName(self):
        '''Reset the library name to 'libgnu'.'''
        self.table['libname'] = 'libgnu'

    # Define libtool methods.
    def checkLibtool(self):
        '''Check if user enabled libtool rules.'''
        return self.table['libtool']

    def setLibtool(self, value):
        '''Enable / disable libtool rules.'''
        if type(value) is bool:
            self.table['libtool'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetLibtool(self):
        '''Reset libtool rules.'''
        self.table['libtool'] = False

    # Define conddeps methods.
    def checkCondDeps(self):
        '''Check if user enabled conditional dependencies.'''
        return self.table['conddeps']

    def setCondDeps(self, value):
        '''Enable / disable conditional dependencies (may save configure time and object code).'''
        if type(value) is bool:
            self.table['conddeps'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetCondDeps(self):
        '''Reset conditional dependencies (may save configure time and object code).'''
        self.table['conddeps'] = False

    # Define lgpl methods.
    def getLGPL(self):
        '''Check for abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        return self.table['lgpl']

    def setLGPL(self, lgpl):
        '''Abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        if lgpl in [None, True, '2', '3orGPLv2', '3']:
            self.table['lgpl'] = lgpl
        else:
            raise TypeError('invalid LGPL version: %s' % repr(lgpl))

    def resetLGPL(self):
        '''Disable abort if modules aren't available under the LGPL.
        Default value is None, which means that lgpl is disabled.'''
        self.table['lgpl'] = None

    def getModuleIndicatorPrefix(self):
        '''Return module_indicator_prefix to use inside GLEmiter class.'''
        return self.getIncludeGuardPrefix()

    # Define macro_prefix and include_guard_prefix methods.
    # The include_guard_prefix is a replacement for ${gl_include_guard_prefix}.
    # It is determined from the macro_prefix.
    def getMacroPrefix(self):
        '''Return the prefix of the macros 'gl_EARLY' and 'gl_INIT'.
        Default macro_prefix is 'gl'.'''
        return self.table['macro_prefix']

    def getIncludeGuardPrefix(self):
        '''Return the replacement for ${gl_include_guard_prefix}.'''
        return self.table['include_guard_prefix']

    def setMacroPrefix(self, macro_prefix):
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

    def resetMacroPrefix(self):
        '''Reset the prefix of the macros 'gl_EARLY' and 'gl_INIT'.
        Default macro_prefix is 'gl'.'''
        self.table['macro_prefix'] = 'gl'
        self.table['include_guard_prefix'] = 'GL'

    # Define makefile_name methods.
    def getMakefileName(self):
        '''Return the name of makefile in automake syntax in the source-base and
        tests-base directories. Default is 'Makefile.am'.'''
        return self.table['makefile_name']

    def setMakefileName(self, makefile_name):
        '''Specify the name of makefile in automake syntax in the source-base and
        tests-base directories. Default is 'Makefile.am'.'''
        if type(makefile_name) is str:
            if makefile_name:
                self.table['makefile_name'] = makefile_name
        else:  # if type of makefile_name is not str
            raise TypeError('makefile_name must be a string, not %s'
                            % type(makefile_name).__name__)

    def resetMakefileName(self):
        '''Reset the name of makefile in automake syntax in the source-base and
        tests-base directories. Default is 'Makefile.am'.'''
        self.table['makefile_name'] = ''

    # Define podomain methods.
    def getPoDomain(self):
        '''Return the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        return self.table['podomain']

    def setPoDomain(self, podomain):
        '''Specify the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        if type(podomain) is str:
            if podomain:
                self.table['podomain'] = podomain
        else:  # if type of podomain is not str
            raise TypeError('podomain must be a string, not %s'
                            % type(podomain).__name__)

    def resetPoDomain(self):
        '''Reset the prefix of the i18n domain. Usually use the package name.
        A suffix '-gnulib' is appended.'''
        self.table['podomain'] = ''

    # Define witness_c_macro methods.
    def getWitnessCMacro(self):
        '''Return the C macro that is defined when the sources in this directory
        are compiled or used.'''
        return self.table['witness_c_macro']

    def setWitnessCMacro(self, witness_c_macro):
        '''Specify the C macro that is defined when the sources in this directory
        are compiled or used.'''
        if type(witness_c_macro) is str:
            if witness_c_macro:
                self.table['witness_c_macro'] = witness_c_macro
        else:  # if type of witness_c_macro is not str
            raise TypeError('witness_c_macro must be a string, not %s'
                            % type(witness_c_macro).__name__)

    def resetWitnessCMacro(self):
        '''Return the C macro that is defined when the sources in this directory
        are compiled or used.'''
        self.table['witness_c_macro'] = ''

    # Define vc_files methods.
    def checkVCFiles(self):
        '''Check if update of the version control files is enabled or disabled.'''
        return self.table['vc_files']

    def setVCFiles(self, value):
        '''Enable /disable update of the version control files.'''
        if type(value) is bool:
            self.table['vc_files'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetVCFiles(self):
        '''Reset update of the version control files and set it to None.'''
        self.table['vc_files'] = None

    # Define configure_ac methods.
    def getAutoconfFile(self):
        '''Return path of autoconf file relative to destdir.'''
        return self.table['configure_ac']

    def setAutoconfFile(self, configure_ac):
        '''Specify path of autoconf file relative to destdir.'''
        if type(configure_ac) is str:
            if configure_ac:
                self.table['configure_ac'] = \
                    relpath(self.table['destdir'], configure_ac)
        else:  # if type of configure_ac is not str
            raise TypeError('configure_ac must be a string, not %s'
                            % type(configure_ac).__name__)

    def resetAutoconfFile(self):
        '''Reset path of autoconf file relative to destdir.'''
        configure_ac = ''
        if isfile(joinpath(self.table['destdir'], 'configure.ac')):
            configure_ac = joinpath(self.table['destdir'], 'configure.ac')
        elif isfile(joinpath(self.table['destdir'], 'configure.in')):
            configure_ac = joinpath(self.table['destdir'], 'configure.in')
        self.table['configure_ac'] = configure_ac

    # Define ac_version methods.
    def getAutoconfVersion(self):
        '''Return preferred autoconf version. Default value is 2.59.'''
        return self.table['ac_version']

    def setAutoconfVersion(self, ac_version):
        '''Specify preferred autoconf version. Default value is 2.59.'''
        if type(ac_version) is float or type(ac_version) is int:
            self.table['ac_version'] = float(ac_version)
        else:  # if ac_version has not int or float type
            raise TypeError('ac_version must be an int or a float, not %s'
                            % type(ac_version).__name__)

    def resetAutoconfVersion(self):
        '''Specify preferred autoconf version. Default value is 2.59.'''
        self.table['ac_version'] = 2.59

    def checkCopyrights(self):
        '''Check if copyright notices in files should be replaced.'''
        return self.table['copyrights']

    # Define symbolic methods.
    def checkSymbolic(self):
        '''Check if pygnulib will make symbolic links instead of copying files.'''
        return self.table['symbolic']

    def setSymbolic(self, value):
        '''Enable / disable creation of the symbolic links instead of copying files.'''
        if type(value) is bool:
            self.table['symbolic'] = value
            self.table['copyrights'] = not value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetSymbolic(self):
        '''Reset creation of the symbolic links instead of copying files.'''
        self.table['symbolic'] = False
        self.table['copyrights'] = True

    # Define lsymbolic methods.
    def checkLSymbolic(self):
        '''Check if pygnulib will make symbolic links instead of copying files,
        only for files from the local override directories.'''
        return self.table['lsymbolic']

    def setLSymbolic(self, value):
        '''Enable / disable creation of symbolic links instead of copying files,
        only for files from the local override directories.'''
        if type(value) is bool:
            self.table['lsymbolic'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetLSymbolic(self):
        '''Reset creation of symbolic links instead of copying files, only for
        files from the local override directories.'''
        self.table['lsymbolic'] = False

    # Define verbosity methods.
    def getVerbosity(self):
        '''Get verbosity level.'''
        return self.table['verbosity']

    def decreaseVerbosity(self):
        '''Decrease verbosity level.'''
        if self.table['verbosity'] > MODES['verbose-min']:
            self.table['verbosity'] -= 1

    def increaseVerbosity(self):
        '''Increase verbosity level.'''
        if self.table['verbosity'] < MODES['verbose-max']:
            self.table['verbosity'] += 1

    def setVerbosity(self, verbose):
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

    def resetVerbosity(self):
        '''Reset verbosity level.'''
        self.table['verbosity'] = 0

    # Define libtests methods.
    def checkLibtests(self):
        '''Return True if a testsbase/libtests.a is needed.'''
        return self.table['libtests']

    def setLibtests(self, value):
        '''Specifies whether testsbase/libtests.a is needed.'''
        if type(value) is bool:
            self.table['libtests'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetLibtests(self):
        '''Reset status of testsbase/libtests.a.'''
        self.table['libtests'] = False

    # Define single_configure methods.
    def checkSingleConfigure(self):
        '''Check whether single configure file should be generated.'''
        return self.table['single_configure']

    def setSingleConfigure(self, value):
        '''Enable / disable generation of the single configure file.'''
        if type(value) is bool:
            self.table['single_configure'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetSingleConfigure(self):
        '''Reset status of the single configure file generation.'''
        self.table['single_configure'] = False

    # Define dryrun methods.
    def checkDryRun(self):
        '''Check whether dryrun is enabled.'''
        return self.table['dryrun']

    def setDryRun(self, value):
        '''Enable / disable dryrun mode.'''
        if type(value) is bool:
            self.table['dryrun'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetDryRun(self):
        '''Reset status of dryrun mode.'''
        self.table['dryrun'] = False

    # Define errors methods.
    def checkErrors(self):
        '''Check if GLError will be raised in non-critical situations.'''
        return self.table['errors']

    def setErrors(self, value):
        '''Enable / disable raising GLError in non-critical situations.'''
        if type(value) is bool:
            self.table['errors'] = value
        else:  # if type(value) is not bool
            raise TypeError('value must be a bool, not %s'
                            % type(value).__name__)

    def resetErrors(self):
        '''Reset status of raising GLError in non-critical situations.'''
        self.table['errors'] = False
