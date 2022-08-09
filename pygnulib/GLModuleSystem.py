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
import re
import sys
import codecs
import hashlib
import subprocess as sp
from . import constants
from .GLError import GLError
from .GLConfig import GLConfig
from .GLFileSystem import GLFileSystem


#===============================================================================
# Define module information
#===============================================================================
__author__ = constants.__author__
__license__ = constants.__license__
__copyright__ = constants.__copyright__


#===============================================================================
# Define global constants
#===============================================================================
DIRS = constants.DIRS
ENCS = constants.ENCS
TESTS = constants.TESTS
joinpath = constants.joinpath
subend = constants.subend
isdir = os.path.isdir
isfile = os.path.isfile
filter_filelist = constants.filter_filelist


#===============================================================================
# Define GLModuleSystem class
#===============================================================================
class GLModuleSystem(object):
    '''GLModuleSystem is used to operate with module system using dynamic
    searching and patching.'''

    def __init__(self, config):
        '''GLModuleSystem.__init__(config) -> GLModuleSystem

        Create new GLModuleSystem instance. Some functions use GLFileSystem class
        to look up a file in localpath or gnulib directories, or combine it through
        'patch' utility.'''
        self.args = dict()
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.filesystem = GLFileSystem(self.config)

    def __repr__(self):
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLModuleSystem %s>' % hex(id(self))
        return result

    def exists(self, module):
        '''GLModuleSystem.exists(module) -> bool

        Check whether the given module exists.
        GLConfig: localpath.'''
        if type(module) is not str:
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)
        localpath = self.config['localpath']
        result = False
        badnames = ['ChangeLog', 'COPYING', 'README', 'TEMPLATE',
                    'TEMPLATE-EXTENDED', 'TEMPLATE-TESTS']
        if module not in badnames:
            result = isfile(joinpath(DIRS['modules'], module))
            if not result:
                for localdir in localpath:
                    if (isdir(joinpath(localdir, 'modules'))
                            and isfile(joinpath(localdir, 'modules', module))):
                        result = True
                        break
        return result

    def find(self, module):
        '''GLModuleSystem.find(module) -> GLModule

        Find the given module.'''
        if type(module) is not str:
            raise TypeError('module must be a string, not %s'
                            % type(module).__name__)
        if self.exists(module):
            path, istemp = self.filesystem.lookup(joinpath('modules', module))
            result = GLModule(self.config, path, istemp)
            return result
        else:  # if not self.exists(module)
            if self.config['errors']:
                raise GLError(3, module)
            else:  # if not self.config['errors']
                sys.stderr.write('gnulib-tool: warning: ')
                sys.stderr.write('file %s does not exist\n' % str(module))

    def file_is_module(self, filename):
        '''Given the name of a file in the modules/ directory, return true
        if should be viewed as a module description file.'''
        return not (filename == 'ChangeLog' or filename.endswith('/ChangeLog')
                    or filename == 'COPYING' or filename.endswith('/COPYING')
                    or filename == 'README' or filename.endswith('/README')
                    or filename == 'TEMPLATE'
                    or filename == 'TEMPLATE-EXTENDED'
                    or filename == 'TEMPLATE-TESTS'
                    or filename.startswith('.')
                    or filename.endswith('~'))

    def list(self):
        '''GLModuleSystem.list() -> list

        Return the available module names as tuple. We could use a combination
        of os.walk() function and re module. However, it takes too much time to
        complete, so this version uses subprocess to run shell commands.'''
        result = ''
        listing = list()
        localpath = self.config['localpath']
        find_args = ['find', 'modules', '-type', 'f', '-print']

        # Read modules from gnulib root directory.
        os.chdir(constants.DIRS['root'])
        find = sp.Popen(find_args, stdout=sp.PIPE)
        result += find.stdout.read().decode("UTF-8")
        os.chdir(DIRS['cwd'])

        # Read modules from local directories.
        if len(localpath) > 0:
            for localdir in localpath:
                os.chdir(localdir)
                find = sp.Popen(find_args, stdout=sp.PIPE)
                result += find.stdout.read().decode("UTF-8")
                os.chdir(DIRS['cwd'])

        listing = [ line
                    for line in result.split('\n')
                    if line.strip() ]
        if len(localpath) > 0:
            listing = [ subend('.diff', '', line)
                        for line in listing ]
        # Remove modules/ prefix from each file name.
        pattern = re.compile('^modules/')
        listing = [ pattern.sub('', line)
                    for line in listing ]
        # Filter out undesired file names.
        listing = [ line
                    for line in listing
                    if self.file_is_module(line) and not line.endswith('-tests') ]
        modules = sorted(set(listing))
        return modules


#===============================================================================
# Define GLModule class
#===============================================================================
class GLModule(object):
    '''GLModule is used to create a module object from the file with the given
    path. GLModule can get all information about module, get its dependencies,
    files, etc.'''

    section_label_pattern = \
        re.compile('^(Description|Comment|Status|Notice|Applicability|'
                   + 'Files|Depends-on|configure\\.ac-early|configure\\.ac|'
                   + 'Makefile\\.am|Include|Link|License|Maintainer):$',
                   re.M)

    # List of characters allowed in shell identifiers.
    shell_id_chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_'

    def __init__(self, config, path, patched=False):
        '''GLModule.__init__(config, path[, patched]) -> GLModule

        Create new GLModule instance. Arguments are path and patched, where
        path is a string representing the path to the module and patched is a
        bool indicating that module was created after applying patch.'''
        self.args = dict()
        self.cache = dict()
        self.content = ''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if type(path) is not str:
            raise TypeError('path must be a string, not %s'
                            % type(path).__name__)
        if type(patched) is not bool:
            raise TypeError('patched must be a bool, not %s'
                            % type(patched).__name__)
        self.path = path
        self.patched = patched
        self.config = config
        self.filesystem = GLFileSystem(self.config)
        self.modulesystem = GLModuleSystem(self.config)
        # Read the module description file into memory.
        with codecs.open(path, 'rb', 'UTF-8') as file:
            self.content = file.read().replace('\r\n', '\n')
        # Dissect it into sections.
        self.sections = dict()
        last_section_label = None
        last_section_start = 0
        for match in GLModule.section_label_pattern.finditer(self.content):
            if last_section_label != None:
                self.sections[last_section_label] = self.content[last_section_start : match.start()]
            last_section_label = match.group(1)
            last_section_start = match.end() + 1
        if last_section_label != None:
            self.sections[last_section_label] = self.content[last_section_start:]

    def __eq__(self, module):
        '''x.__eq__(y) <==> x==y'''
        result = bool()
        if type(module) is GLModule:
            if self.path == module.path:
                result = True
        return result

    def __ne__(self, module):
        '''x.__ne__(y) <==> x!=y'''
        result = bool()
        if type(module) is GLModule:
            if self.path != module.path:
                result = True
        return result

    def __ge__(self, module):
        '''x.__ge__(y) <==> x>=y'''
        result = bool()
        if type(module) is GLModule:
            if self.path >= module.path:
                result = True
        return result

    def __gt__(self, module):
        '''x.__gt__(y) <==> x>y'''
        result = bool()
        if type(module) is GLModule:
            if self.path > module.path:
                result = True
        return result

    def __hash__(self):
        '''x.__hash__() <==> hash(x)'''
        result = hash(self.path) ^ hash(self.patched)
        return result

    def __le__(self, module):
        '''x.__le__(y) <==> x<=y'''
        result = bool()
        if type(module) is GLModule:
            if self.path <= module.path:
                result = True
        return result

    def __lt__(self, module):
        '''x.__lt__(y) <==> x<y'''
        result = bool()
        if type(module) is GLModule:
            if self.path < module.path:
                result = True
        return result

    def __str__(self):
        '''x.__str__() <==> str(x)'''
        result = self.getName()
        return result

    def __repr__(self):
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLModule %s %s>' % (repr(self.getName()), hex(id(self)))
        return result

    def getName(self):
        '''GLModule.getName() -> str

        Return the name of the module.'''
        pattern = re.compile(joinpath('modules', '(.*)$'))
        result = pattern.findall(self.path)[0]
        return result

    def isPatched(self):
        '''GLModule.isPatched() -> bool

        Check whether module was created after applying patch.'''
        return self.patched

    def isTests(self):
        '''GLModule.isTests() -> bool

        Check whether module is a -tests version of module.'''
        result = self.getName().endswith('-tests')
        return result

    def isNonTests(self):
        '''GLModule.isTests() -> bool

        Check whether module is not a -tests version of module.'''
        result = not(self.isTests())
        return result

    def getTestsName(self):
        '''Return -tests version of the module name.'''
        result = self.getName()
        if not result.endswith('-tests'):
            result += '-tests'
        return result

    def getTestsModule(self):
        '''Return -tests version of the module as GLModule.'''
        result = self.modulesystem.find(self.getTestsName())
        return result

    def getShellFunc(self):
        '''GLModule.getShellFunc() -> str

        Computes the shell function name that will contain the m4 macros for the
        module.'''
        macro_prefix = self.config['macro_prefix']
        valid_shell_id = True
        for char in self.getName():
            if char not in GLModule.shell_id_chars:
                valid_shell_id = False
                break
        identifier = None
        if valid_shell_id:
            identifier = self.getName()
        else:
            hash_input = '%s\n' % self.getName()
            identifier = hashlib.md5(hash_input.encode(ENCS['default'])).hexdigest()
        result = 'func_%s_gnulib_m4code_%s' % (macro_prefix, identifier)
        return result

    def getShellVar(self):
        '''GLModule.getShellVar() -> str

        Compute the shell variable name the will be set to true once the m4 macros
        for the module have been executed.'''
        macro_prefix = self.config['macro_prefix']
        valid_shell_id = True
        for char in self.getName():
            if char not in GLModule.shell_id_chars:
                valid_shell_id = False
                break
        identifier = None
        if valid_shell_id:
            identifier = self.getName()
        else:
            hash_input = '%s\n' % self.getName()
            identifier = hashlib.md5(hash_input.encode(ENCS['default'])).hexdigest()
        result = '%s_gnulib_enabled_%s' % (macro_prefix, identifier)
        return result

    def getConditionalName(self):
        '''GLModule.getConditionalName() -> str

        Return the automake conditional name.
        GLConfig: macro_prefix.'''
        macro_prefix = self.config['macro_prefix']
        valid_shell_id = True
        for char in self.getName():
            if char not in GLModule.shell_id_chars:
                valid_shell_id = False
                break
        identifier = None
        if valid_shell_id:
            identifier = self.getName()
        else:
            hash_input = '%s\n' % self.getName()
            identifier = hashlib.md5(hash_input.encode(ENCS['default'])).hexdigest()
        result = '%s_GNULIB_ENABLED_%s' % (macro_prefix, identifier)
        return result

    def getDescription(self):
        '''GLModule.getDescription() -> str

        Return description of the module.'''
        return self.sections.get('Description', '')

    def getComment(self):
        '''GLModule.getComment() -> str

        Return comment to module.'''
        return self.sections.get('Comment', '')

    def getStatus(self):
        '''GLModule.getStatus() -> str

        Return module status.'''
        return self.sections.get('Status', '')

    def getStatuses(self):
        '''GLModule.getStatuses() -> list

        Return module status.'''
        if 'statuses' not in self.cache:
            snippet = self.getStatus()
            result = [ line.strip()
                       for line in snippet.split('\n')
                       if line.strip() ]
            self.cache['statuses'] = result
        return self.cache['statuses']

    def getNotice(self):
        '''GLModule.getNotice() -> str

        Return notice to module.'''
        return self.sections.get('Notice', '')

    def getApplicability(self):
        '''GLModule.getApplicability() -> str

        Return applicability of module.'''
        if 'applicability' not in self.cache:
            result = self.sections.get('Applicability', '')
            result = result.strip()
            if not result:
                # The default is 'main' or 'tests', depending on the module's name.
                if self.isTests():
                    result = 'tests'
                else:
                    result = 'main'
            self.cache['applicability'] = result
        return self.cache['applicability']

    def getFiles(self):
        '''GLModule.getFiles() -> list

        Return list of files.
        GLConfig: ac_version.'''
        if 'files' not in self.cache:
            snippet = self.sections.get('Files', '')
            result = [ line.strip()
                       for line in snippet.split('\n')
                       if line.strip() ]
            result.append(joinpath('m4', '00gnulib.m4'))
            result.append(joinpath('m4', 'zzgnulib.m4'))
            result.append(joinpath('m4', 'gnulib-common.m4'))
            self.cache['files'] = result
        return self.cache['files']

    def getDependencies(self):
        '''GLModule.getDependencies() -> str

        Return list of dependencies, as a snippet.
        GLConfig: localpath.'''
        if 'dependencies' not in self.cache:
            result = ''
            # ${module}-tests implicitly depends on ${module}, if that module exists.
            if self.isTests():
                main_module = subend('-tests', '', self.getName())
                if self.modulesystem.exists(main_module):
                    result += '%s\n' % main_module
            # Then the explicit dependencies listed in the module description.
            snippet = self.sections.get('Depends-on', '')
            # Remove comment lines.
            snippet = re.compile('^#.*$[\n]', re.M).sub('', snippet)
            result += snippet
            self.cache['dependencies'] = result
        return self.cache['dependencies']

    def getDependenciesWithoutConditions(self):
        '''GLModule.getDependenciesWithoutConditions() -> list

        Return list of dependencies, as a list of GLModule objects.
        GLConfig: localpath.'''
        if 'dependenciesWithoutCond' not in self.cache:
            snippet = self.getDependencies()
            lines = [ line.strip()
                      for line in snippet.split('\n')
                      if line.strip() ]
            pattern = re.compile(' *\\[.*$')
            lines = [ pattern.sub('', line)
                      for line in lines ]
            result = [ self.modulesystem.find(module)
                       for module in lines
                       if module != '' ]
            self.cache['dependenciesWithoutCond'] = result
        return self.cache['dependenciesWithoutCond']

    def getDependenciesWithConditions(self):
        '''GLModule.getDependenciesWithConditions() -> list

        Return list of dependencies, as a list of pairs (GLModule object, condition).
        The "true" condition is denoted by None.
        GLConfig: localpath.'''

        if 'dependenciesWithCond' not in self.cache:
            snippet = self.getDependencies()
            lines = [ line.strip()
                      for line in snippet.split('\n')
                      if line.strip() ]
            pattern = re.compile(' *\\[')
            result = []
            for line in lines:
                match = pattern.search(line)
                if match:
                    module = line[0 : match.start()]
                    condition = line[match.end() :]
                    condition = subend(']', '', condition)
                else:
                    module = line
                    condition = None
                if module != '':
                    if condition == 'true':
                        condition = None
                    result.append(tuple([self.modulesystem.find(module), condition]))
            self.cache['dependenciesWithCond'] = result
        return self.cache['dependenciesWithCond']

    def getAutoconfEarlySnippet(self):
        '''GLModule.getAutoconfEarlySnippet() -> str

        Return autoconf-early snippet.'''
        return self.sections.get('configure.ac-early', '')

    def getAutoconfSnippet(self):
        '''GLModule.getAutoconfSnippet() -> str

        Return autoconf snippet.'''
        return self.sections.get('configure.ac', '')

    def getAutomakeSnippet(self):
        '''getAutomakeSnippet() -> str

        Get automake snippet.
        GLConfig: auxdir, ac_version.'''
        result = ''
        conditional = self.getAutomakeSnippet_Conditional()
        if conditional.strip():
            result += self.getAutomakeSnippet_Conditional()
        else:  # if not conditional.strip()
            result += '\n'
        result += self.getAutomakeSnippet_Unconditional()
        return result

    def getAutomakeSnippet_Conditional(self):
        '''GLModule.getAutomakeSnippet_Conditional() -> str

        Return conditional automake snippet.'''
        return self.sections.get('Makefile.am', '')

    def getAutomakeSnippet_Unconditional(self):
        '''GLModule.getAutomakeSnippet_Unconditional() -> str

        Return unconditional automake snippet.
        GLConfig: auxdir, ac_version.'''
        auxdir = self.config['auxdir']
        ac_version = self.config['ac_version']
        result = ''
        if 'makefile-unconditional' not in self.cache:
            if self.isTests():
                files = self.getFiles()
                extra_files = filter_filelist(constants.NL, files,
                                              'tests/', '', 'tests/', '').split(constants.NL)
                extra_files = sorted(set(extra_files))
                if extra_files:
                    result += 'EXTRA_DIST += %s' % ' '.join(extra_files)
                    result += constants.NL * 2
            else:  # if not tests module
                # TODO: unconditional automake snippet for nontests modules
                snippet = self.getAutomakeSnippet_Conditional()
                snippet = constants.combine_lines(snippet)
                pattern = re.compile('^lib_SOURCES[\t ]*\\+=[\t ]*(.*)$', re.M)
                mentioned_files = pattern.findall(snippet)
                if mentioned_files != list():
                    mentioned_files = mentioned_files[-1].split(' ')
                    mentioned_files = [ f.strip()
                                        for f in mentioned_files ]
                    mentioned_files = [ f
                                        for f in mentioned_files
                                        if f != '' ]
                    mentioned_files = sorted(set(mentioned_files))
                all_files = self.getFiles()
                lib_files = filter_filelist(constants.NL, all_files,
                                            'lib/', '', 'lib/', '').split(constants.NL)
                extra_files = [ f
                                for f in lib_files
                                if f not in mentioned_files ]
                extra_files = sorted(set(extra_files))
                if extra_files != [''] and extra_files:
                    result += 'EXTRA_DIST += %s' % ' '.join(extra_files)
                    result += '\n\n'
                # Synthesize also an EXTRA_lib_SOURCES augmentation
                if str(self) != 'relocatable-prog-wrapper' and str(self) != 'pt_chown':
                    extra_files = filter_filelist(constants.NL, extra_files,
                                                  '', '.c', '', '').split(constants.NL)
                    extra_files = sorted(set(extra_files))
                    if extra_files != ['']:
                        result += 'EXTRA_lib_SOURCES += %s' % ' '.join(extra_files)
                        result += '\n\n'
                # Synthesize an EXTRA_DIST augmentation also for the files in build-aux
                buildaux_files = filter_filelist(constants.NL, all_files,
                                                 'build-aux/', '', 'build-aux/', '').split(constants.NL)
                buildaux_files = sorted(set(buildaux_files))
                if buildaux_files != ['']:
                    buildaux_files = ''.join(buildaux_files)
                    buildaux_files = joinpath('$(top_srcdir)', auxdir, buildaux_files)
                    result += 'EXTRA_DIST += %s' % buildaux_files
                    result += '\n\n'
                # Synthesize an EXTRA_DIST augmentation also for the files from top/.
                top_files = filter_filelist(constants.NL, all_files,
                                            'top/', '', 'top/', '').split(constants.NL)
                top_files = sorted(set(top_files))
                if top_files != ['']:
                    top_files = ''.join(top_files)
                    top_files = joinpath('$(top_srcdir)', top_files)
                    result += 'EXTRA_DIST += %s' % top_files
                    result += '\n\n'
            result = constants.nlconvert(result)
            self.cache['makefile-unconditional'] = result
        return self.cache['makefile-unconditional']

    def getInclude(self):
        '''GLModule.getInclude() -> str

        Return include directive.'''
        if 'include' not in self.cache:
            snippet = self.sections.get('Include', '')
            pattern = re.compile('^(["<])', re.M)
            result = pattern.sub('#include \\1', snippet)
            self.cache['include'] = result
        return self.cache['include']

    def getLink(self):
        '''GLModule.getLink() -> str

        Return link directive.'''
        return self.sections.get('Link', '')

    def getLicense_Raw(self):
        '''GLModule.getLicense_Raw() -> str

        Return module license.'''
        return self.sections.get('License', '')

    def getLicense(self):
        '''GLModule.getLicense(self) -> str

        Get license and warn user if module lacks a license.'''
        if 'license' not in self.cache:
            license = self.getLicense_Raw().strip()
            # Warn if the License field is missing.
            if not self.isTests():
                if not license:
                    if self.config['errors']:
                        raise GLError(18, str(self))
                    else:  # if not self.config['errors']
                        sys.stderr.write('gnulib-tool: warning: module %s lacks a License\n' % str(self))
            if str(self) == 'parse-datetime':
                # This module is under a weaker license only for the purpose of some
                # users who hand-edit it and don't use gnulib-tool. For the regular
                # gnulib users they are under a stricter license.
                result = 'GPL'
            else:
                result = license
                # The default is GPL.
                if not result:
                    result = 'GPL'
            self.cache['license'] = result
        return self.cache['license']

    def getMaintainer(self):
        '''GLModule.getMaintainer() -> str

        Return maintainer directive.'''
        return self.sections.get('Maintainer', '')


#===============================================================================
# Define GLModuleTable class
#===============================================================================
class GLModuleTable(object):
    '''GLModuleTable is used to work with the list of the modules.'''

    def __init__(self, config, inc_all_direct_tests, inc_all_indirect_tests):
        '''GLModuleTable.__init__(config, inc_all_direct_tests, inc_all_indirect_tests) -> GLModuleTable

        Create new GLModuleTable instance. If modules are specified, then add
        every module from iterable as unconditional module. If avoids is specified,
        then in transitive_closure every dependency which is in avoids won't be
        included in the final modules list. If conddeps are enabled,
        then store condition for each dependency if it has a condition.
        The only necessary argument is localpath, which is needed just to create
        modulesystem instance to look for dependencies.
        inc_all_direct_tests = True if all kinds of problematic unit tests among
                                    the unit tests of the specified modules
                                    should be included
        inc_all_indirect_tests = True if all kinds of problematic unit tests
                                    among the unit tests of the dependencies
                                    should be included

        Methods for conditional dependencies:
        - addUnconditional(B)
          notes the presence of B as an unconditional module.
        - addConditional(A, B. cond)
          notes the presence of a conditional dependency from module A to module B,
          subject to the condition that A is enabled and cond is true.
        - isConditional(B)
          tests whether module B is conditional.
        - getCondition(A, B)
          returns the condition when B should be enabled as a dependency of A,
          once the m4 code for A has been executed.
        '''
        self.dependers = dict()  # Dependencies
        self.conditionals = dict()  # Conditional modules
        self.unconditionals = dict()  # Unconditional modules
        self.base_modules = list()  # Base modules
        self.main_modules = list()  # Main modules
        self.tests_modules = list()  # Tests modules
        self.final_modules = list()  # Final modules
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.filesystem = GLFileSystem(self.config)
        self.modulesystem = GLModuleSystem(self.config)
        if type(inc_all_direct_tests) is not bool:
            raise TypeError('inc_all_direct_tests must be a bool, not %s'
                            % type(inc_all_direct_tests).__name__)
        self.inc_all_direct_tests = inc_all_direct_tests
        self.inc_all_indirect_tests = inc_all_indirect_tests
        self.avoids = list()  # Avoids
        for avoid in self.config.getAvoids():
            module = self.modulesystem.find(avoid)
            if module:
                self.avoids.append(module)

    def __repr__(self):
        '''x.__repr__() <==> repr(x)'''
        result = '<pygnulib.GLModuleTable %s>' % hex(id(self))
        return result

    def __getitem__(self, y):
        '''x.__getitem__(y) <==> x[y]'''
        if y in ['base', 'final', 'main', 'tests', 'avoids']:
            if y == 'base':
                return self.getBaseModules()
            elif y == 'final':
                return self.getFinalModules()
            elif y == 'main':
                return self.getMainModules()
            elif y == 'tests':
                return self.getTestsModules()
            else:  # if y == 'avoids'
                return self.getAvoids()
        else:  # if y is not in list
            raise KeyError('GLModuleTable does not contain key: %s' % repr(y))

    def addConditional(self, parent, module, condition):
        '''GLModuleTable.addConditional(module, condition)

        Add new conditional dependency from parent to module with condition.'''
        if type(parent) is not GLModule:
            raise TypeError('parent must be a GLModule, not %s'
                            % type(parent).__name__)
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        if not (type(condition) is str or condition == True):
            raise TypeError('condition must be a string or True, not %s'
                            % type(condition).__name__)
        if not str(module) in self.unconditionals:
            # No unconditional dependency to the given module is known at this point.
            if str(module) not in self.dependers:
                self.dependers[str(module)] = list()
            if str(parent) not in self.dependers[str(module)]:
                self.dependers[str(module)].append(str(parent))
            key = '%s---%s' % (str(parent), str(module))
            self.conditionals[key] = condition

    def addUnconditional(self, module):
        '''GLModuleTable.addUnconditional(module)

        Add module as unconditional dependency.'''
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        self.unconditionals[str(module)] = True
        if str(module) in self.dependers:
            self.dependers.pop(str(module))

    def isConditional(self, module):
        '''GLModuleTable.isConditional(module) -> bool

        Check whether module is unconditional.'''
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        result = str(module) in self.dependers
        return result

    def getCondition(self, parent, module):
        '''GLModuleTable.getCondition(module) -> str or True

        Return condition from parent to module. Condition can be string or True.
        If module is not in the list of conddeps, method returns None.'''
        if type(parent) is not GLModule:
            raise TypeError('parent must be a GLModule, not %s'
                            % type(parent).__name__)
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        key = '%s---%s' % (str(parent), str(module))
        result = self.conditionals.get(key, None)
        return result

    def transitive_closure(self, modules):
        '''GLModuleTable.transitive_closure(modules) -> list

        Use transitive closure to add module and its dependencies. Add every
        module and its dependencies from modules list, but do not add dependencies
        which contain in avoids list. If any incl_test_categories is enabled, then
        add dependencies which are in these categories. If any excl_test_categories,
        then do not add dependencies which are in these categories. If conddeps are enabled,
        then store condition for each dependency if it has a condition. This method
        is used to update final list of modules. Method returns list of modules.
        GLConfig: incl_test_categories, excl_test_categories.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        # In order to process every module only once (for speed), process an
        # "input list" of modules, producing an "output list" of modules. During
        # each round, more modules can be queued in the input list. Once a
        # module on the input list has been processed, it is added to the
        # "handled list", so we can avoid to process it again.
        inc_all_tests = self.inc_all_direct_tests
        handledmodules = list()
        inmodules = modules
        outmodules = list()
        if self.config['conddeps']:
            for module in modules:
                if module not in self.avoids:
                    self.addUnconditional(module)
        while inmodules:
            inmodules_this_round = inmodules
            inmodules = list()               # Accumulator, queue for next round
            for module in inmodules_this_round:
                if module not in self.avoids:
                    outmodules += [module]
                    if self.config['conddeps']:
                        automake_snippet = \
                            module.getAutomakeSnippet_Conditional()
                        pattern = re.compile('^if', re.M)
                        if not pattern.findall(automake_snippet):
                            # A module whose Makefile.am snippet contains a
                            # reference to an automake conditional. If we were
                            # to use it conditionally, we would get an error
                            #   configure: error: conditional "..." was never defined.
                            # because automake 1.11.1 does not handle nested
                            # conditionals correctly. As a workaround, make the
                            # module unconditional.
                            self.addUnconditional(module)
                        conditional = self.isConditional(module)
                    dependencies = module.getDependenciesWithConditions()
                    depmodules = [ pair[0]
                                   for pair in dependencies ]
                    conditions = [ pair[1]
                                   for pair in dependencies ]
                    # Duplicate dependencies are harmless, but Jim wants a warning.
                    duplicate_depmodules = [ depmodule
                                             for depmodule in set(depmodules)
                                             if depmodules.count(depmodule) > 1 ]
                    if duplicate_depmodules:
                        duplicate_depmodule_names = [ str(depmodule)
                                                      for depmodule in duplicate_depmodules ]
                        message = ('gnulib-tool: warning: module %s has duplicated dependencies: %s\n'
                                   % (module, duplicate_depmodule_names))
                        sys.stderr.write(message)
                    if self.config.checkInclTestCategory(TESTS['tests']):
                        testsname = module.getTestsName()
                        if self.modulesystem.exists(testsname):
                            testsmodule = self.modulesystem.find(testsname)
                            depmodules += [testsmodule]
                            conditions += [None]
                    for depmodule in depmodules:
                        # Determine whether to include the dependency or tests module.
                        include = True
                        statuses = depmodule.getStatuses()
                        for word in statuses:
                            if word == 'obsolete':
                                if not self.config.checkInclTestCategory(TESTS['obsolete']):
                                    include = False
                            elif word == 'c++-test':
                                if self.config.checkExclTestCategory(TESTS['c++-test']):
                                    include = False
                                if not (inc_all_tests or self.config.checkInclTestCategory(TESTS['c++-test'])):
                                    include = False
                            elif word == 'longrunning-test':
                                if self.config.checkExclTestCategory(TESTS['longrunning-test']):
                                    include = False
                                if not (inc_all_tests or self.config.checkInclTestCategory(TESTS['longrunning-test'])):
                                    include = False
                            elif word == 'privileged-test':
                                if self.config.checkExclTestCategory(TESTS['privileged-test']):
                                    include = False
                                if not (inc_all_tests or self.config.checkInclTestCategory(TESTS['privileged-test'])):
                                    include = False
                            elif word == 'unportable-test':
                                if self.config.checkExclTestCategory(TESTS['unportable-test']):
                                    include = False
                                if not (inc_all_tests or self.config.checkInclTestCategory(TESTS['unportable-test'])):
                                    include = False
                            elif word.endswith('-test'):
                                if not inc_all_tests:
                                    include = False
                        if include and depmodule not in self.avoids:
                            inmodules += [depmodule]
                            if self.config['conddeps']:
                                index = depmodules.index(depmodule)
                                condition = conditions[index]
                                if condition == True:
                                    condition = None
                                if condition:
                                    self.addConditional(module, depmodule, condition)
                                else:  # if condition
                                    if conditional:
                                        self.addConditional(module, depmodule, True)
                                    else:  # if not conditional
                                        self.addUnconditional(depmodule)
            handledmodules = sorted(set(handledmodules + inmodules_this_round))
            # Remove handledmodules from inmodules.
            inmodules = [module
                         for module in inmodules
                         if module not in handledmodules]
            inmodules = sorted(set(inmodules))
            inc_all_tests = self.inc_all_indirect_tests
        modules = sorted(set(outmodules))
        self.modules = modules
        return list(modules)

    def transitive_closure_separately(self, basemodules, finalmodules):
        '''GLModuleTable.transitive_closure_separately(*args, **kwargs) -> tuple

        Determine main module list and tests-related module list separately.
        The main module list is the transitive closure of the specified modules,
        ignoring tests modules. Its lib/* sources go into $sourcebase/. If lgpl is
        specified, it will consist only of LGPLed source.
        The tests-related module list is the transitive closure of the specified
        modules, including tests modules, minus the main module list excluding
        modules of applicability 'all'. Its lib/* sources (brought in through
        dependencies of *-tests modules) go into $testsbase/. It may contain GPLed
        source, even if lgpl is specified.
        Arguments are basemodules and finalmodules, where basemodules argument
        represents modules specified by user and finalmodules represents modules
        list after previous transitive_closure.
        Method returns tuple which contains two lists: the list of main modules and
        the list of tests-related modules. Both lists contain dependencies.
        GLConfig: incl_test_categories, excl_test_categories.'''
        for module in basemodules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        for module in finalmodules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        # Determine main module list.
        saved_inctests = self.config.checkInclTestCategory(TESTS['tests'])
        self.config.disableInclTestCategory(TESTS['tests'])
        main_modules = self.transitive_closure(basemodules)
        self.config.setInclTestCategory(TESTS['tests'], saved_inctests)
        # Determine tests-related module list.
        tests_modules = \
            [ m
              for m in finalmodules
              if not (m in main_modules and m.getApplicability() == 'main') ]
        # Note: Since main_modules is (hopefully) a subset of finalmodules, this
        # ought to be the same as
        #   [ m
        #     for m in finalmodules
        #     if m not in main_modules ] \
        #   + [ m
        #       for m in main_modules
        #       if m.getApplicability() != 'main' ]
        tests_modules = sorted(set(tests_modules))
        result = tuple([main_modules, tests_modules])
        return result

    def add_dummy(self, modules):
        '''GLModuleTable.add_dummy(modules) -> list

        Add dummy package to list of modules if dummy package is needed. If not,
        return original list of modules.
        GLConfig: auxdir, ac_version.'''
        auxdir = self.config['auxdir']
        ac_version = self.config['ac_version']
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        # Determine whether any module provides a lib_SOURCES augmentation.
        have_lib_sources = False
        for module in modules:
            if not module.isTests():
                snippet = module.getAutomakeSnippet()
                # Extract the value of "lib_SOURCES += ...".
                snippet = constants.remove_backslash_newline(snippet)
                pattern = re.compile('^lib_SOURCES[\t ]*\\+=([^#]*).*$', re.M)
                for matching_rhs in pattern.findall(snippet):
                    files = matching_rhs.split(' ')
                    for file in files:
                        # Ignore .h files since they are not compiled.
                        if not file.endswith('.h'):
                            have_lib_sources = True
                            break
        # Add the dummy module, to make sure the library will be non-empty.
        if not have_lib_sources:
            dummy = self.modulesystem.find('dummy')
            if dummy not in self.avoids:
                if dummy not in modules:
                    modules = sorted(set(modules)) + [dummy]
        return list(modules)

    def filelist(self, modules):
        '''GLModuleTable.filelist(modules) -> list

        Determine the final file list for the given list of modules. The list of
        modules must already include dependencies.
        GLConfig: ac_version.'''
        ac_version = self.config['ac_version']
        filelist = list()
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        listings = [ module.getFiles()
                     for module in modules ]
        for listing in listings:
            for file in listing:
                if file not in filelist:
                    filelist += [file]
        return filelist

    def filelist_separately(self, main_modules, tests_modules):
        '''GLModuleTable.filelist_separately(**kwargs) -> list

        Determine the final file lists. They must be computed separately, because
        files in lib/* go into $sourcebase/ if they are in the main file list but
        into $testsbase/ if they are in the tests-related file list. Furthermore
        lib/dummy.c can be in both.'''
        ac_version = self.config['ac_version']
        main_filelist = self.filelist(main_modules)
        tests_filelist = self.filelist(tests_modules)
        tests_filelist = [ file.replace('lib/', 'tests=lib/', 1) if file.startswith('lib/') else file
                           for file in tests_filelist ]
        result = tuple([main_filelist, tests_filelist])
        return result

    def getAvoids(self):
        '''GLModuleTable.getAvoids() -> list

        Return list of avoids.'''
        return list(self.avoids)

    def setAvoids(self, modules):
        '''GLModuleTable.setAvoids(modules)

        Specify list of avoids.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.avoids = sorted(set(modules))

    def getBaseModules(self):
        '''GLModuleTable.getBaseModules() -> list

        Return list of base modules.'''
        return list(self.base_modules)

    def setBaseModules(self, modules):
        '''GLModuleTable.setBaseModules(modules)

        Specify list of base modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.base_modules = sorted(set(modules))

    def getFinalModules(self):
        '''GLModuleTable.getFinalModules() -> list

        Return list of final modules.'''
        return list(self.final_modules)

    def setFinalModules(self, modules):
        '''GLModuleTable.setFinalModules(modules)

        Specify list of final modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.final_modules = sorted(set(modules))

    def getMainModules(self):
        '''GLModuleTable.getMainModules() -> list

        Return list of main modules.'''
        return list(self.main_modules)

    def setMainModules(self, modules):
        '''GLModuleTable.setMainModules(modules)

        Specify list of main modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.main_modules = sorted(set(modules))

    def getTestsModules(self):
        '''GLModuleTable.getTestsModules() -> list

        Return list of tests modules.'''
        return list(self.tests_modules)

    def setTestsModules(self, modules):
        '''GLModuleTable.setTestsModules(modules)

        Specify list of tests modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.tests_modules = sorted(set(modules))
