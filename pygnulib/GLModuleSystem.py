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
import re
import sys
import hashlib
import subprocess as sp
import shlex
from collections import defaultdict
from typing import Any, ClassVar
from .constants import (
    DIRS,
    ENCS,
    TESTS,
    combine_lines,
    filter_filelist,
    joinpath,
    lines_to_multiline,
    subend,
)
from .GLError import GLError
from .GLConfig import GLConfig
from .GLFileSystem import GLFileSystem


# Regular expression matching a 'lib_SOURCES += ...' variable with the file
# names in capture group 1.
_LIB_SOURCES_PATTERN = re.compile(r'^lib_SOURCES[\t ]*\+=[\t ]*(.+?)[\t ]*(?:#|$)', re.MULTILINE)


def _extract_lib_SOURCES(snippet: str) -> list[str]:
    '''Return the file names specified in the lib_SOURCES variable of
    a Makefile.am snippet.'''
    lines = [ line.group(1)
              for line in re.finditer(_LIB_SOURCES_PATTERN, snippet) ]
    return [ file_name
             for line in lines
             for file_name in line.split() ]

def _isTestsModuleName(name: str) -> bool:
    '''Determine whether a module is a tests module, given its name.'''
    return name.endswith('-tests') and name != 'valgrind-tests'


#===============================================================================
# Define GLModuleSystem class
#===============================================================================
class GLModuleSystem:
    '''GLModuleSystem is used to operate with module system using dynamic
    searching and patching.'''

    config: GLConfig
    filesystem: GLFileSystem

    def __init__(self, config: GLConfig) -> None:
        '''Create new GLModuleSystem instance. Some functions use GLFileSystem class
        to look up a file in localpath or gnulib directories, or combine it through
        'patch' utility.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.filesystem = GLFileSystem(self.config)

    def __repr__(self) -> str:
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLModuleSystem %s>' % hex(id(self))
        return result

    def exists(self, module_name: str) -> bool:
        '''Check whether the given module exists.
        GLConfig: localpath.'''
        if type(module_name) is not str:
            raise TypeError('module_name must be a string, not %s'
                            % type(module_name).__name__)
        localpath = self.config['localpath']
        result = False
        badnames = ['ChangeLog', 'COPYING', 'README', 'TEMPLATE',
                    'TEMPLATE-EXTENDED', 'TEMPLATE-TESTS']
        if module_name not in badnames:
            result = os.path.isfile(joinpath(DIRS['modules'], module_name))
            if not result:
                for localdir in localpath:
                    if (os.path.isdir(joinpath(localdir, 'modules'))
                            and os.path.isfile(joinpath(localdir, 'modules', module_name))):
                        result = True
                        break
        return result

    def find(self, module_name: str) -> GLModule | None:
        '''Return the GLModule object given the module name,
        or None if the module description file with that name does not exist.
        - module_name, The name of the module.'''
        if type(module_name) is not str:
            raise TypeError('module_name must be a string, not %s'
                            % type(module_name).__name__)
        if self.exists(module_name):
            path, istemp = self.filesystem.lookup(joinpath('modules', module_name))
            result = GLModule(self.config, module_name, path, istemp)
            return result
        else:  # if not self.exists(module)
            if self.config['errors']:
                raise GLError(23, module_name)
            else:  # if not self.config['errors']
                sys.stderr.write('gnulib-tool: warning: ')
                sys.stderr.write("module %s doesn't exist\n" % module_name)

    def file_is_module(self, filename: str) -> bool:
        '''Given the name of a file in the modules/ directory, return true
        if should be viewed as a module description file.'''
        return not (filename == 'ChangeLog' or filename.endswith('/ChangeLog')
                    or filename == 'COPYING' or filename.endswith('/COPYING')
                    or filename == 'README' or filename.endswith('/README')
                    or filename == 'TEMPLATE'
                    or filename == 'TEMPLATE-EXTENDED'
                    or filename == 'TEMPLATE-TESTS'
                    or filename.startswith('.')
                    or filename.endswith('.orig')
                    or filename.endswith('.rej')
                    or filename.endswith('~'))

    def list(self, with_tests: bool | None = False) -> list[str]:
        '''Return the available module names as tuple. We could use a combination
        of os.walk() function and re module. However, it takes too much time to
        complete, so this version uses subprocess to run shell commands.
        Arguments are:
        - with_tests: Include test module names, optional argument.'''
        result = ''
        localpath = self.config['localpath']
        find_args = ['find', 'modules', '-type', 'f', '-print']

        # Read modules from gnulib root directory.
        result += sp.run(find_args, cwd=DIRS['root'], text=True, capture_output=True, check=False).stdout

        # Read modules from local directories.
        if len(localpath) > 0:
            for localdir in localpath:
                result += sp.run(find_args, cwd=localdir, text=True, capture_output=True, check=False).stdout

        listing = [ line
                    for line in result.split('\n')
                    if line.strip() ]
        if len(localpath) > 0:
            listing = [ subend('.diff', '', line)
                        for line in listing ]
        # Remove modules/ prefix from each file name.
        pattern = re.compile(r'^modules/')
        listing = [ pattern.sub(r'', line)
                    for line in listing ]
        # Filter out undesired file names.
        listing = [ line
                    for line in listing
                    if self.file_is_module(line) ]
        # Check if test modules should be filtered out.
        if not with_tests:
            listing = [ line
                        for line in listing
                        if not _isTestsModuleName(line) ]
        modules = sorted(set(listing))
        return modules

    def getAllModules(self) -> list[GLModule]:
        '''Return a list of all modules as a list of GLModule objects.'''
        module_names = self.list(True)
        modules = [ self.find(module)
                    for module in module_names ]
        modules = [ module
                    for module in modules
                    if module is not None ]
        return modules


#===============================================================================
# Define GLModule class
#===============================================================================
class GLModule:
    '''GLModule is used to create a module object from the file with the given
    path. GLModule can get all information about module, get its dependencies,
    files, etc.'''

    # Regular expression matching the start of a section in the module description.
    section_label_pattern: ClassVar[re.Pattern] = \
        re.compile(r'^(Description|Comment|Status|Notice|Applicability|Usable-in-testdir|'
                   + r'Files|Depends-on|configure\.ac-early|configure\.ac|'
                   + r'Makefile\.am|Include|Link|License|Maintainer):$',
                   re.M)

    # Regular expression matching module names that can be used as shell ids.
    shell_id_pattern: ClassVar[re.Pattern] = re.compile(r'^\w*$', re.ASCII)

    cache: dict[str, Any]
    content: str
    name: str
    path: str
    patched: bool
    config: GLConfig
    modulesystem: GLModuleSystem
    sections: dict[str, str]

    def __init__(self, config: GLConfig, name: str, path: str, patched: bool = False) -> None:
        '''Create new GLModule instance. Arguments are:
        - name, the name of the module,
        - path, the file name of the (possibly patched) module description,
        - patched, indicating whether that module description was created by applying a patch.'''
        self.cache = dict()
        self.content = ''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if type(name) is not str:
            raise TypeError('name must be a string, not %s'
                            % type(name).__name__)
        if type(path) is not str:
            raise TypeError('path must be a string, not %s'
                            % type(path).__name__)
        if type(patched) is not bool:
            raise TypeError('patched must be a bool, not %s'
                            % type(patched).__name__)
        self.name = name
        self.path = path
        self.patched = patched
        self.config = config
        self.modulesystem = GLModuleSystem(self.config)
        # Read the module description file into memory.
        with open(path, mode='r', newline='\n', encoding='utf-8') as file:
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

    def __eq__(self, module: object) -> bool:
        '''x.__eq__(y) <==> x==y'''
        result = False
        if type(module) is GLModule:
            if self.name == module.name:
                result = True
        return result

    def __ne__(self, module: object) -> bool:
        '''x.__ne__(y) <==> x!=y'''
        result = False
        if type(module) is GLModule:
            if self.name != module.name:
                result = True
        return result

    def __ge__(self, module: object) -> bool:
        '''x.__ge__(y) <==> x>=y'''
        result = False
        if type(module) is GLModule:
            if self.name >= module.name:
                result = True
        return result

    def __gt__(self, module: object) -> bool:
        '''x.__gt__(y) <==> x>y'''
        result = False
        if type(module) is GLModule:
            if self.name > module.name:
                result = True
        return result

    def __hash__(self) -> int:
        '''x.__hash__() <==> hash(x)'''
        result = hash(self.name)
        return result

    def __le__(self, module: object) -> bool:
        '''x.__le__(y) <==> x<=y'''
        result = False
        if type(module) is GLModule:
            if self.name <= module.name:
                result = True
        return result

    def __lt__(self, module: object) -> bool:
        '''x.__lt__(y) <==> x<y'''
        result = False
        if type(module) is GLModule:
            if self.name < module.name:
                result = True
        return result

    def __str__(self) -> str:
        '''x.__str__() <==> str(x)'''
        return self.name

    def __repr__(self) -> str:
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLModule %s %s>' % (repr(self.name), hex(id(self)))
        return result

    def isPatched(self) -> bool:
        '''Check whether module was created after applying patch.'''
        return self.patched

    def isTests(self) -> bool:
        '''Check whether module is a *-tests module or a module of
        applicability 'all'.'''
        result = self.getApplicability() != 'main'
        return result

    def isNonTests(self) -> bool:
        '''Check whether module is not a *-tests module.'''
        result = not _isTestsModuleName(self.name)
        return result

    def getTestsName(self) -> str:
        '''Return -tests version of the module name.'''
        return f'{self.name}-tests'

    def getTestsModule(self) -> GLModule | None:
        '''Return -tests version of the module as GLModule.'''
        result = self.modulesystem.find(self.getTestsName())
        return result

    def repeatModuleInTests(self) -> bool:
        '''Tests whether, when the tests have their own configure.ac script,
        a given module should be repeated in the tests, although it was
        already among the main modules.'''
        # This module is special because it relies on a gl_LIBTEXTSTYLE_OPTIONAL
        # invocation that it does not itself do or require. Therefore if the
        # tests contain such an invocation, the module - as part of tests -
        # will produce different AC_SUBSTed variable values than the same module
        # - as part of the main configure.ac -.
        result = self.name == 'libtextstyle-optional'
        return result

    def getLinkDirectiveRecursively(self) -> str:
        '''Return a list of the link directives of this module separated
        by a newline.'''
        handledmodules = set()
        inmodules = set()
        outmodules = set()

        # In order to process every module only once (for speed), process an "input
        # list" of modules, producing an "output list" of modules. During each round,
        # more modules can be queued in the input list. Once a module on the input
        # list has been processed, it is added to the "handled list", so we can avoid
        # to process it again.
        inmodules.add(self)
        while len(inmodules) > 0:
            inmodules_this_round = inmodules
            inmodules = set()  # Accumulator, queue for next round
            for module in inmodules_this_round:
                if module.getLink() != '':
                    # The module description has a 'Link:' field. Ignore the dependencies.
                    outmodules.add(module)
                else:
                    # The module description has no 'Link:' field. Recurse through the dependencies.
                    inmodules = inmodules.union(module.getDependenciesWithoutConditions())
            handledmodules = handledmodules.union(inmodules_this_round)
            # Remove handledmodules from inmodules.
            inmodules = inmodules.difference(handledmodules)

        # Remove whitespace from sections.
        link_sections = [ module.getLink().strip()
                          for module in outmodules ]
        # Sort the link directives.
        directives = sorted({ line
                              for section in link_sections
                              for line in section.splitlines() })
        return lines_to_multiline(directives)

    def getShellId(self) -> str:
        '''Return an unique id suitable for use in shell scripts. If the
        module name is not a valid shell identifier use its MD5 digest.'''
        if re.match(self.shell_id_pattern, self.name):
            return self.name
        # Newline character needed for compatibility with gnulib-tool.sh.
        return hashlib.md5(f'{self.name}\n'.encode(ENCS['default'])).hexdigest()

    def getShellFunc(self) -> str:
        '''Computes the shell function name that will contain the m4 macros
        for the module.'''
        macro_prefix = self.config['macro_prefix']
        return 'func_%s_gnulib_m4code_%s' % (macro_prefix, self.getShellId())

    def getShellVar(self) -> str:
        '''Compute the shell variable name the will be set to true once the
        m4 macros for the module have been executed.'''
        macro_prefix = self.config['macro_prefix']
        return '%s_gnulib_enabled_%s' % (macro_prefix, self.getShellId())

    def getConditionalName(self) -> str:
        '''Return the automake conditional name.
        GLConfig: macro_prefix.'''
        macro_prefix = self.config['macro_prefix']
        return '%s_GNULIB_ENABLED_%s' % (macro_prefix, self.getShellId())

    def getDescription(self) -> str:
        '''Return description of the module.'''
        return self.sections.get('Description', '')

    def getComment(self) -> str:
        '''Return comment to module.'''
        return self.sections.get('Comment', '')

    def getStatus(self) -> str:
        '''Return module status.'''
        return self.sections.get('Status', '')

    def getStatuses(self) -> list[str]:
        '''Return module status.'''
        if 'statuses' not in self.cache:
            snippet = self.getStatus()
            result = [ line.strip()
                       for line in snippet.split('\n')
                       if line.strip() ]
            self.cache['statuses'] = result
        return self.cache['statuses']

    def getNotice(self) -> str:
        '''Return notice to module.'''
        return self.sections.get('Notice', '')

    def getApplicability(self) -> str:
        '''Return applicability of module.'''
        if 'applicability' not in self.cache:
            result = self.sections.get('Applicability', '')
            result = result.strip()
            if not result:
                # The default is 'main' or 'tests', depending on the module's name.
                if _isTestsModuleName(self.name):
                    result = 'tests'
                else:
                    result = 'main'
            self.cache['applicability'] = result
        return self.cache['applicability']

    def getUsabilityInTestdir(self) -> str:
        '''Return the usability-in-testdir of module.'''
        if 'usability-in-testdir' not in self.cache:
            snippet = self.sections.get('Usable-in-testdir', '')
            result = [ line.strip()
                       for line in snippet.split('\n')
                       if not line.startswith('#') and line.strip() ]
            result = lines_to_multiline(result).strip()
            self.cache['usability-in-testdir'] = result
        return self.cache['usability-in-testdir']

    def getFiles_Raw(self) -> str:
        '''Return the unmodified list of files as a string.'''
        return self.sections.get('Files', '')

    def getFiles(self) -> list[str]:
        '''Return list of files.'''
        if 'files' not in self.cache:
            snippet = self.getFiles_Raw()
            result = [ line.strip()
                       for line in snippet.split('\n')
                       if line.strip() ]
            result.append(joinpath('m4', '00gnulib.m4'))
            result.append(joinpath('m4', 'zzgnulib.m4'))
            result.append(joinpath('m4', 'gnulib-common.m4'))
            self.cache['files'] = result
        return self.cache['files']

    def getDependencies(self) -> str:
        '''Return list of dependencies, as a snippet.
        GLConfig: localpath.'''
        if 'dependencies' not in self.cache:
            result = ''
            # ${module}-tests implicitly depends on ${module}, if that module exists.
            if _isTestsModuleName(self.name):
                main_module = subend('-tests', '', self.name)
                if self.modulesystem.exists(main_module):
                    result += '%s\n' % main_module
            # Then the explicit dependencies listed in the module description.
            snippet = self.sections.get('Depends-on', '')
            # Remove comment lines.
            snippet = re.compile(r'^#.*$[\n]', re.M).sub(r'', snippet)
            result += snippet
            self.cache['dependencies'] = result
        return self.cache['dependencies']

    def getDependenciesWithoutConditions(self) -> list[GLModule]:
        '''Return list of dependencies, as a list of GLModule objects.
        GLConfig: localpath.'''
        if 'dependenciesWithoutCond' not in self.cache:
            snippet = self.getDependencies()
            lines = [ line.strip()
                      for line in snippet.split('\n')
                      if line.strip() ]
            pattern = re.compile(r' *\[.*$')
            lines = [ pattern.sub(r'', line)
                      for line in lines ]
            result = [ self.modulesystem.find(module)
                       for module in lines
                       if module != '' ]
            result = [ module
                       for module in result
                       if module is not None ]
            self.cache['dependenciesWithoutCond'] = result
        return self.cache['dependenciesWithoutCond']

    def getDependenciesWithConditions(self) -> list[tuple[GLModule, str | None]]:
        '''Return list of dependencies, as a list of pairs (GLModule object, condition).
        The "true" condition is denoted by None.
        GLConfig: localpath.'''

        if 'dependenciesWithCond' not in self.cache:
            snippet = self.getDependencies()
            lines = [ line.strip()
                      for line in snippet.split('\n')
                      if line.strip() ]
            pattern = re.compile(r' *\[')
            result = []
            for line in lines:
                match = pattern.search(line)
                if not match:
                    module_name = line
                    condition = None
                else:
                    module_name = line[0 : match.start()]
                    condition = line[match.end() :]
                    condition = subend(']', '', condition)
                    if condition == 'true':
                        condition = None
                module = self.modulesystem.find(module_name)
                if module is not None:
                    result.append((module, condition))
            self.cache['dependenciesWithCond'] = result
        return self.cache['dependenciesWithCond']

    def getDependenciesRecursively(self) -> set[GLModule]:
        '''Return a list of recursive dependencies of this module,
        as a set of GLModule objects.'''
        handledmodules = set()
        inmodules = set()
        outmodules = set()

        # In order to process every module only once (for speed), process an "input
        # list" of modules, producing an "output list" of modules. During each round,
        # more modules can be queued in the input list. Once a module on the input
        # list has been processed, it is added to the "handled list", so we can avoid
        # to process it again.
        inmodules.add(self)
        while len(inmodules) > 0:
            inmodules_this_round = inmodules
            inmodules = set()  # Accumulator, queue for next round
            for module in inmodules_this_round:
                outmodules.add(module)
                inmodules = inmodules.union(module.getDependenciesWithoutConditions())
            handledmodules = handledmodules.union(inmodules_this_round)
            # Remove handledmodules from inmodules.
            inmodules = inmodules.difference(handledmodules)

        return outmodules

    def getDependents(self) -> list[GLModule]:
        '''Return list of dependents (a.k.a. "reverse dependencies"),
        as a list of GLModule objects.
        This implementation is optimized for the case of a single invocation
        of getDependents.  If you need multiple invocations, better use _getDependents.
        GLConfig: localpath.'''
        if 'dependents' not in self.cache:
            localpath = self.config['localpath']
            # Find a set of module candidates quickly.
            # Convert the module name to a POSIX basic regex.
            # Needs to handle . [ \ * ^ $.
            regex = self.name.replace('\\', '\\\\').replace('[', '\\[').replace('^', '\\^')
            regex = re.compile(r'([.*$])').sub(r'[\1]', regex)
            line_regex = '^' + regex
            # We can't add a '$' to line_regex, because that would fail to match
            # lines that denote conditional dependencies. We could invoke grep
            # twice, once to search for  line_regex + '$'  and once to search
            # for   line_regex + [ <TAB>]  but that would be twice as slow.
            # Read module candidates from gnulib root directory.
            command = "find modules -type f -print | xargs -n 100 grep -l %s /dev/null | sed -e 's,^modules/,,'" % shlex.quote(line_regex)
            result = sp.run(command, shell=True, cwd=DIRS['root'], capture_output=True).stdout.decode('utf-8')
            # Read module candidates from local directories.
            if localpath != None and len(localpath) > 0:
                command = "find modules -type f -print | xargs -n 100 grep -l %s /dev/null | sed -e 's,^modules/,,' -e 's,\\.diff$,,'" % shlex.quote(line_regex)
                for localdir in localpath:
                    result += sp.run(command, shell=True, cwd=localdir, capture_output=True).stdout.decode('utf-8')
            listing = [ line
                        for line in result.split('\n')
                        if line.strip() ]
            # Remove modules/ prefix from each file name.
            pattern = re.compile(r'^modules/')
            listing = [ pattern.sub('', line)
                        for line in listing ]
            # Filter out undesired file names.
            listing = [ line
                        for line in listing
                        if self.modulesystem.file_is_module(line) ]
            # ${module}-tests implicitly depends on ${module}, if both exist.
            if self.isNonTests():
                implicit_dependent = self.name+'-tests'
                if self.modulesystem.exists(implicit_dependent):
                    listing.append(implicit_dependent)
            candidates = sorted(set(listing))
            result = []
            for name in candidates:
                module = self.modulesystem.find(name)
                if module:  # Ignore module candidates that don't actually exist.
                    if self in module.getDependenciesWithoutConditions():
                        result.append(module)
            self.cache['dependents'] = result
        return self.cache['dependents']

    def _getDependents(self, modules: list[GLModule]) -> list[GLModule]:
        '''Return the list of dependents of this module,
        among the given set of modules.'''
        result = []
        for module in modules:
            if self in set(module.getDependenciesWithoutConditions()):
                result.append(module)
        return result

    def getDependentsRecursively(self) -> set[GLModule]:
        '''Return a list of recursive dependents of this module,
        as a set of GLModule objects.'''
        handledmodules = set()
        inmodules = set()
        outmodules = set()

        modules = self.modulesystem.getAllModules()

        # In order to process every module only once (for speed), process an "input
        # list" of modules, producing an "output list" of modules. During each round,
        # more modules can be queued in the input list. Once a module on the input
        # list has been processed, it is added to the "handled list", so we can avoid
        # to process it again.
        inmodules.add(self)
        while len(inmodules) > 0:
            inmodules_this_round = inmodules
            inmodules = set()  # Accumulator, queue for next round
            for module in inmodules_this_round:
                outmodules.add(module)
                inmodules = inmodules.union(module._getDependents(modules))
            handledmodules = handledmodules.union(inmodules_this_round)
            # Remove handledmodules from inmodules.
            inmodules = inmodules.difference(handledmodules)

        return outmodules

    def getAutoconfEarlySnippet(self) -> str:
        '''Return autoconf-early snippet.'''
        return self.sections.get('configure.ac-early', '')

    def getAutoconfSnippet(self) -> str:
        '''Return autoconf snippet.'''
        return self.sections.get('configure.ac', '')

    def getAutomakeSnippet(self) -> str:
        '''Get automake snippet.
        GLConfig: auxdir.'''
        result = ''
        conditional = self.getAutomakeSnippet_Conditional()
        if conditional.strip():
            result += self.getAutomakeSnippet_Conditional()
        else:  # if not conditional.strip()
            result += '\n'
        result += self.getAutomakeSnippet_Unconditional()
        return result

    def getAutomakeSnippet_Conditional(self) -> str:
        '''Return conditional automake snippet.'''
        return self.sections.get('Makefile.am', '')

    def getAutomakeSnippet_Unconditional(self) -> str:
        '''Return unconditional automake snippet.
        GLConfig: auxdir'''
        auxdir = self.config['auxdir']
        result = ''
        if 'makefile-unconditional' not in self.cache:
            if _isTestsModuleName(self.name):
                # *-tests module live in tests/, not lib/.
                # Synthesize an EXTRA_DIST augmentation.
                files = self.getFiles()
                extra_files = filter_filelist('\n', files,
                                              'tests/', '', 'tests/', '')
                if extra_files != '':
                    result += 'EXTRA_DIST += %s' % ' '.join(extra_files.split('\n'))
                    result += '\n' * 2
            else:  # if not tests module
                # Synthesize an EXTRA_DIST augmentation.
                snippet = self.getAutomakeSnippet_Conditional()
                snippet = combine_lines(snippet)
                # Get all the file names from 'lib_SOURCES += ...'.
                mentioned_files = set(_extract_lib_SOURCES(snippet))
                all_files = self.getFiles()
                lib_files = filter_filelist('\n', all_files,
                                            'lib/', '', 'lib/', '')
                if lib_files != '':
                    lib_files = set(lib_files.split('\n'))
                else:
                    lib_files = set()
                # Remove mentioned_files from lib_files.
                extra_files = sorted(lib_files.difference(mentioned_files))
                if extra_files:
                    result += 'EXTRA_DIST += %s' % ' '.join(extra_files)
                    result += '\n\n'
                # Synthesize also an EXTRA_lib_SOURCES augmentation.
                # This is necessary so that automake can generate the right list of
                # dependency rules.
                # A possible approach would be to use autom4te --trace of the redefined
                # AC_LIBOBJ and AC_REPLACE_FUNCS macros when creating the Makefile.am
                # (use autom4te --trace, not just grep, so that AC_LIBOBJ invocations
                # inside autoconf's built-in macros are not missed).
                # But it's simpler and more robust to do it here, based on the file list.
                # If some .c file exists and is not used with AC_LIBOBJ - for example,
                # a .c file is preprocessed into another .c file for BUILT_SOURCES -,
                # automake will generate a useless dependency; this is harmless.
                if self.name != 'relocatable-prog-wrapper' and self.name != 'pt_chown':
                    extra_files = filter_filelist('\n', extra_files,
                                                  '', '.c', '', '')
                    if extra_files != '':
                        result += 'EXTRA_lib_SOURCES += %s' % ' '.join(extra_files.split('\n'))
                        result += '\n\n'
                # Synthesize an EXTRA_DIST augmentation also for the files in build-aux
                buildaux_files = filter_filelist('\n', all_files,
                                                 'build-aux/', '', 'build-aux/', '')
                if buildaux_files != '':
                    buildaux_files = [ os.path.join('$(top_srcdir)', joinpath(auxdir, filename))
                                       for filename in buildaux_files.split('\n') ]
                    result += 'EXTRA_DIST += %s' % ' '.join(buildaux_files)
                    result += '\n\n'
            self.cache['makefile-unconditional'] = result
        return self.cache['makefile-unconditional']

    def getInclude(self) -> str:
        '''Return include directive.'''
        if 'include' not in self.cache:
            snippet = self.sections.get('Include', '')
            pattern = re.compile(r'^(["<])', re.M)
            result = pattern.sub(r'#include \1', snippet)
            self.cache['include'] = result
        return self.cache['include']

    def getLink(self) -> str:
        '''Return link directive.'''
        return self.sections.get('Link', '')

    def getLicense_Raw(self) -> str:
        '''Return module license.'''
        return self.sections.get('License', '')

    def getLicense(self) -> str:
        '''Get license and warn user if module lacks a license.'''
        if 'license' not in self.cache:
            license = self.getLicense_Raw().strip()
            # Warn if the License field is missing.
            if not _isTestsModuleName(self.name):
                if not license:
                    if self.config['errors']:
                        raise GLError(18, self.name)
                    else:  # if not self.config['errors']
                        sys.stderr.write('gnulib-tool: warning: module %s lacks a License\n' % self.name)
            if self.name.startswith('parse-datetime'):
                # These modules are under a weaker license only for the purpose of some
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

    def getMaintainer(self) -> str:
        '''Return maintainer directive.'''
        return self.sections.get('Maintainer', '')


#===============================================================================
# Define GLModuleTable class
#===============================================================================
class GLModuleTable:
    '''GLModuleTable is used to work with the list of the modules.'''

    dependers: defaultdict[GLModule, set[GLModule]]
    conditionals: dict[tuple[GLModule, GLModule], str | bool]
    unconditionals: set[GLModule]
    base_modules: list[GLModule]
    main_modules: list[GLModule]
    tests_modules: list[GLModule]
    final_modules: list[GLModule]
    config: GLConfig
    modulesystem: GLModuleSystem
    inc_all_direct_tests: bool
    inc_all_indirect_tests: bool
    avoids: set[GLModule]

    def __init__(self, config: GLConfig, inc_all_direct_tests: bool, inc_all_indirect_tests: bool) -> None:
        '''Create new GLModuleTable instance. If modules are specified, then add
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
        self.dependers = defaultdict(set)
        self.conditionals = dict()
        self.unconditionals = set()
        self.base_modules = []
        self.main_modules = []
        self.tests_modules = []
        self.final_modules = []
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config
        self.modulesystem = GLModuleSystem(self.config)
        if type(inc_all_direct_tests) is not bool:
            raise TypeError('inc_all_direct_tests must be a bool, not %s'
                            % type(inc_all_direct_tests).__name__)
        self.inc_all_direct_tests = inc_all_direct_tests
        self.inc_all_indirect_tests = inc_all_indirect_tests
        self.avoids = set()
        for avoid in self.config.getAvoids():
            module = self.modulesystem.find(avoid)
            if module:
                self.avoids.add(module)

    def __repr__(self) -> str:
        '''x.__repr__() <==> repr(x)'''
        result = '<pygnulib.GLModuleTable %s>' % hex(id(self))
        return result

    def addConditional(self, parent: GLModule, module: GLModule, condition: str | bool) -> None:
        '''Add new conditional dependency from parent to module with condition.'''
        if type(parent) is not GLModule:
            raise TypeError('parent must be a GLModule, not %s'
                            % type(parent).__name__)
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        if not (type(condition) is str or condition == True):
            raise TypeError('condition must be a string or True, not %s'
                            % type(condition).__name__)
        if module not in self.unconditionals:
            # No unconditional dependency to the given module is known at this point.
            self.dependers[module].add(parent)
            self.conditionals[(parent, module)] = condition

    def addUnconditional(self, module: GLModule) -> None:
        '''Add module as unconditional dependency.'''
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        self.unconditionals.add(module)
        if module in self.dependers:
            self.dependers.pop(module)

    def isConditional(self, module: GLModule) -> bool:
        '''Check whether module is unconditional.'''
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        result = module in self.dependers
        return result

    def getCondition(self, parent: GLModule, module: GLModule) -> str | bool | None:
        '''Return condition from parent to module. Condition can be string or True.
        If module is not in the list of conddeps, method returns None.'''
        if type(parent) is not GLModule:
            raise TypeError('parent must be a GLModule, not %s'
                            % type(parent).__name__)
        if type(module) is not GLModule:
            raise TypeError('module must be a GLModule, not %s'
                            % type(module).__name__)
        result = self.conditionals.get((parent, module), None)
        return result

    def transitive_closure(self, modules: list[GLModule]) -> list[GLModule]:
        '''Use transitive closure to add module and its dependencies. Add every
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
        handledmodules = set()
        inmodules = modules
        outmodules = []
        if self.config['conddeps']:
            for module in modules:
                if module not in self.avoids:
                    self.addUnconditional(module)
        while inmodules:
            inmodules_this_round = inmodules
            inmodules = []               # Accumulator, queue for next round
            for module in inmodules_this_round:
                if module not in self.avoids:
                    outmodules.append(module)
                    if self.config['conddeps']:
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
                        duplicate_depmodule_names = [ depmodule.name
                                                      for depmodule in duplicate_depmodules ]
                        message = ('gnulib-tool: warning: module %s has duplicated dependencies: %s\n'
                                   % (module, duplicate_depmodule_names))
                        sys.stderr.write(message)
                    if self.config.checkInclTestCategory(TESTS['tests']):
                        testsname = module.getTestsName()
                        if self.modulesystem.exists(testsname):
                            testsmodule = self.modulesystem.find(testsname)
                            depmodules.append(testsmodule)
                            conditions.append(None)
                    for depmodule in depmodules:
                        # Determine whether to include the dependency or tests module.
                        include = True
                        statuses = depmodule.getStatuses()
                        for word in statuses:
                            if word == 'obsolete':
                                if not self.config.checkIncObsolete():
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
                            inmodules.append(depmodule)
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
            handledmodules = handledmodules.union(inmodules_this_round)
            # Remove handledmodules from inmodules.
            inmodules = [ module
                          for module in inmodules
                          if module not in handledmodules ]
            inmodules = sorted(set(inmodules))
            inc_all_tests = self.inc_all_indirect_tests
        modules = sorted(set(outmodules))
        return modules

    def transitive_closure_separately(self, basemodules: list[GLModule],
                                      finalmodules: list[GLModule]) -> tuple[list[GLModule], list[GLModule]]:
        '''Determine main module list and tests-related module list separately.
        The main module list is the transitive closure of the specified modules,
        ignoring tests modules. Its lib/* sources go into $sourcebase/. If lgpl is
        specified, it will consist only of LGPLed source; if gpl is specified, it
        will consist of only source than can be relicensed under the specified GPL
        version.
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
        main_modules_set = set(main_modules)
        tests_modules = \
            [ m
              for m in finalmodules
              if not (m in main_modules_set and m.getApplicability() == 'main') ]
        # Note: Since main_modules is (hopefully) a subset of finalmodules, this
        # ought to be the same as
        #   [ m
        #     for m in finalmodules
        #     if m not in main_modules ] \
        #   + [ m
        #       for m in main_modules
        #       if m.getApplicability() != 'main' ]
        tests_modules = sorted(set(tests_modules))
        # If testsrelated_modules consists only of modules with applicability 'all',
        # set it to empty (because such modules are only helper modules for other modules).
        have_nontrivial_testsrelated_modules = False
        for module in tests_modules:
            if module.getApplicability() != 'all':
                have_nontrivial_testsrelated_modules = True
                break
        if not have_nontrivial_testsrelated_modules:
            tests_modules = []
        result = tuple([main_modules, tests_modules])
        return result

    def remove_if_blocks(self, snippet: str) -> str:
        '''Removes if...endif blocks from an automake snippet.'''
        lines = snippet.splitlines()
        cleansed = []
        depth = 0
        for line in lines:
            if line.startswith('if '):
                depth += 1
            elif line.startswith('endif'):
                depth -= 1
                # Make sure gnulib-tool.py and gnulib-tool.sh produce the same
                # output.
                cleansed.append(line[5:])
            elif depth == 0:
                cleansed.append(line)
        return lines_to_multiline(cleansed)

    def add_dummy(self, modules: list[GLModule]) -> list[GLModule]:
        '''Add dummy package to list of modules if dummy package is needed.
        If not, return original list of modules.
        GLConfig: conddeps.'''
        conddeps = self.config['conddeps']
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        # Determine whether any module provides a lib_SOURCES augmentation.
        have_lib_sources = False
        for module in modules:
            if module.isNonTests():
                if conddeps and self.isConditional(module):
                    # Ignore conditional modules, since they are not guaranteed to
                    # contribute to lib_SOURCES.
                    pass
                else:
                    snippet = module.getAutomakeSnippet()
                    # Extract the value of unconditional "lib_SOURCES += ..." augmentations.
                    snippet = combine_lines(snippet)
                    snippet = self.remove_if_blocks(snippet)
                    file_names = _extract_lib_SOURCES(snippet)
                    for file_name in file_names:
                        # Ignore .h files since they are not compiled.
                        if not file_name.endswith('.h'):
                            have_lib_sources = True
                            break
                    if have_lib_sources:
                        break
        # Add the dummy module, to make sure the library will be non-empty.
        if not have_lib_sources:
            dummy = self.modulesystem.find('dummy')
            if dummy not in self.avoids:
                if dummy not in modules:
                    modules = sorted(set(modules)) + [dummy]
        return list(modules)

    def filelist(self, modules: list[GLModule]) -> list[str]:
        '''Determine the final file list for the given list of modules.
        The list of modules must already include dependencies.'''
        filelist = []
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        listings = [ module.getFiles()
                     for module in modules ]
        for listing in listings:
            for file in listing:
                if file not in filelist:
                    filelist.append(file)
        return filelist

    def filelist_separately(self, main_modules: list[GLModule],
                            tests_modules: list[GLModule]) -> tuple[list[str], list[str]]:
        '''Determine the final file lists. They must be computed separately, because
        files in lib/* go into $sourcebase/ if they are in the main file list but
        into $testsbase/ if they are in the tests-related file list. Furthermore
        lib/dummy.c can be in both.'''
        main_filelist = self.filelist(main_modules)
        tests_filelist = self.filelist(tests_modules)
        tests_filelist = [ file.replace('lib/', 'tests=lib/', 1) if file.startswith('lib/') else file
                           for file in tests_filelist ]
        result = tuple([main_filelist, tests_filelist])
        return result

    def getBaseModules(self) -> list[GLModule]:
        '''Return list of base modules.'''
        return list(self.base_modules)

    def setBaseModules(self, modules: list[GLModule]) -> None:
        '''Specify list of base modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.base_modules = modules

    def getFinalModules(self) -> list[GLModule]:
        '''Return list of final modules.'''
        return list(self.final_modules)

    def setFinalModules(self, modules: list[GLModule]) -> None:
        '''Specify list of final modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.final_modules = modules

    def getMainModules(self) -> list[GLModule]:
        '''Return list of main modules.'''
        return list(self.main_modules)

    def setMainModules(self, modules: list[GLModule]) -> None:
        '''Specify list of main modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.main_modules = modules

    def getTestsModules(self) -> list[GLModule]:
        '''Return list of tests modules.'''
        return list(self.tests_modules)

    def setTestsModules(self, modules: list[GLModule]) -> None:
        '''Specify list of tests modules.'''
        for module in modules:
            if type(module) is not GLModule:
                raise TypeError('each module must be a GLModule instance')
        self.tests_modules = modules
