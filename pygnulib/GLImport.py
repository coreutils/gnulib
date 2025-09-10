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
import subprocess as sp
from .constants import (
    DIRS,
    UTILS,
    MODES,
    TESTS,
    cleaner,
    joinpath,
    lines_to_multiline,
    movefile,
    copyfile,
    copyfile2,
    substart,
    subend,
    bold_escapes,
    relconcat,
    relativize,
    rmtree,
)
from .functions import rewrite_file_name
from .GLError import GLError
from .GLConfig import GLConfig
from .GLModuleSystem import GLModuleTable
from .GLModuleSystem import GLModuleSystem
from .GLFileSystem import GLFileSystem
from .GLFileSystem import GLFileAssistant
from .GLMakefileTable import GLMakefileTable
from .GLEmiter import GLEmiter
from .GLFileTable import GLFileTable


#===============================================================================
# Define GLImport class
#===============================================================================
class GLImport:
    '''GLImport class is used to provide methods for --import, --add-import,
    --remove-import and --update actions. This is a high-level class, so
    developers may  have to use lower-level classes to create their own
    scripts. However, if user needs just to use power of gnulib-tool, this class
    is a very good choice.'''

    mode: int
    m4dirs: list[str]
    config: GLConfig
    cache: GLConfig
    emitter: GLEmiter
    modulesystem: GLModuleSystem
    moduletable: GLModuleTable
    makefiletable: GLMakefileTable

    def __init__(self, config: GLConfig, mode: int, m4dirs: list[str]) -> None:
        '''Create GLImport instance.
        config - must be a GLConfig object.
        mode - must be one of the values of the constants.MODES values.
        m4dirs - list of all directories that contain relevant .m4 files.'''
        if type(config) is not GLConfig:
            raise TypeError('config must have GLConfig type, not %s'
                            % repr(config))
        if not (type(mode) is int and MODES['import'] <= mode <= MODES['update']):
            raise TypeError('mode must be 0 <= mode <= 3, not %s'
                            % repr(mode))
        if type(m4dirs) is not list:
            raise TypeError('m4dirs must be a list of strings, not %s'
                            % repr(m4dirs))

        self.mode = mode
        self.m4dirs = m4dirs

        # config contains the configuration, as specified through command-line
        # parameters.

        # self.config records the configuration that we are building/deducing.
        # Initialize it with the properties from config.
        self.config = config.copy()

        # self.cache is the configuration extracted from some files on the
        # file system: configure.{ac,in}, gnulib-cache.m4, gnulib-comp.m4.
        self.cache = GLConfig()

        # Read configure.{ac,in}.
        with open(self.config.getAutoconfFile(), mode='r', newline='\n', encoding='utf-8') as file:
            data = file.read()

        # Get cached auxdir from configure.{ac,in}.
        self.cache.setAuxDir('.')
        pattern = re.compile(r'^AC_CONFIG_AUX_DIR\([\[ ]*([^]"$`\\)]+).*?$', re.MULTILINE)
        match = pattern.search(data)
        if match:
            self.cache.setAuxDir(match.group(1))
        if self.config['auxdir'] == '':
            self.config.setAuxDir(self.cache['auxdir'])

        # Get libtool guess from configure.{ac,in}.
        # XXX This is not actually used.
        pattern = re.compile(r'A[CM]_PROG_LIBTOOL', re.M)
        guessed_libtool = bool(pattern.findall(data))

        # Guess autoconf version.
        pattern = re.compile(r'.*AC_PREREQ\((.*)\)', re.M)
        versions = cleaner(pattern.findall(data))
        if versions:
            version = max({ float(version)
                            for version in versions })
            self.config.setAutoconfVersion(version)
            if version < 2.64:
                raise GLError(4, version)

        # Get other cached variables.
        path = joinpath(self.config['m4base'], 'gnulib-cache.m4')
        if os.path.isfile(path):
            with open(path, mode='r', newline='\n', encoding='utf-8') as file:
                data = file.read()

            # gl_LGPL is special, because it can occur with or without
            # an argument list.
            pattern = re.compile(r'^gl_LGPL$', re.M)
            if pattern.search(data):
                self.cache.setLGPL(True)
            # Find gl_* macros without an argument list.
            if 'gl_LIBTOOL' in data:
                self.cache.setLibtool(True)
                data = data.replace('gl_LIBTOOL', '')
            if 'gl_CONDITIONAL_DEPENDENCIES' in data:
                self.cache.setCondDeps(True)
                data = data.replace('gl_CONDITIONAL_DEPENDENCIES', '')
            if 'gl_WITH_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['tests'])
                data = data.replace('gl_WITH_TESTS', '')
            if 'gl_WITH_OBSOLETE' in data:
                self.cache.setIncObsolete(True)
                data = data.replace('gl_WITH_OBSOLETE', '')
            if 'gl_WITH_CXX_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['c++-test'])
                data = data.replace('gl_WITH_CXX_TESTS', '')
            if 'gl_WITH_LONGRUNNING_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['longrunning-test'])
                data = data.replace('gl_WITH_LONGRUNNING_TESTS', '')
            if 'gl_WITH_PRIVILEGED_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['privileged-test'])
                data = data.replace('gl_WITH_PRIVILEGED_TESTS', '')
            if 'gl_WITH_UNPORTABLE_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['unportable-test'])
                data = data.replace('gl_WITH_UNPORTABLE_TESTS', '')
            if 'gl_WITH_ALL_TESTS' in data:
                self.cache.enableInclTestCategory(TESTS['all-test'])
                data = data.replace('gl_WITH_ALL_TESTS', '')
            if 'gl_AUTOMAKE_SUBDIR' in data:
                self.cache.setAutomakeSubdir(True)
                data = data.replace('gl_AUTOMAKE_SUBDIR', '')
            # Find gl_* macros with an argument list.
            pattern = re.compile(r'^(gl_[A-Z0-9_]*)\((.*?)\)$', re.S | re.M)
            keys = \
                [
                    'gl_LOCAL_DIR', 'gl_MODULES', 'gl_AVOID', 'gl_SOURCE_BASE',
                    'gl_M4_BASE', 'gl_PO_BASE', 'gl_DOC_BASE', 'gl_TESTS_BASE',
                    'gl_LIB', 'gl_LGPL', 'gl_GPL',
                    'gl_MAKEFILE_NAME', 'gl_TESTS_MAKEFILE_NAME',
                    'gl_MACRO_PREFIX', 'gl_PO_DOMAIN', 'gl_WITNESS_C_MACRO',
                    'gl_VC_FILES',
                ]
            result = dict(pattern.findall(data))
            values = cleaner([ result.get(key, '')
                               for key in keys ])
            tempdict = dict(zip(keys, values))
            if 'gl_LGPL' in tempdict:
                lgpl = cleaner(tempdict['gl_LGPL'])
                if lgpl != '':
                    self.cache.setLGPL(lgpl)
            else:  # if 'gl_LGPL' not in tempdict
                self.cache.setLGPL(None)
            if tempdict['gl_LIB']:
                self.cache.setLibName(cleaner(tempdict['gl_LIB']))
            if tempdict['gl_GPL']:
                self.cache.setLibName(cleaner(tempdict['gl_GPL']))
            if tempdict['gl_LOCAL_DIR']:
                self.cache.setLocalPath(cleaner(tempdict['gl_LOCAL_DIR']).split(':'))
            if tempdict['gl_MODULES']:
                self.cache.setModules(cleaner(tempdict['gl_MODULES'].split()))
            if tempdict['gl_AVOID']:
                self.cache.setAvoids(cleaner(tempdict['gl_AVOID'].split()))
            if tempdict['gl_SOURCE_BASE']:
                self.cache.setSourceBase(cleaner(tempdict['gl_SOURCE_BASE']))
            if tempdict['gl_M4_BASE']:
                self.cache.setM4Base(cleaner(tempdict['gl_M4_BASE']))
            if tempdict['gl_PO_BASE']:
                self.cache.setPoBase(cleaner(tempdict['gl_PO_BASE']))
            if tempdict['gl_DOC_BASE']:
                self.cache.setDocBase(cleaner(tempdict['gl_DOC_BASE']))
            if tempdict['gl_TESTS_BASE']:
                self.cache.setTestsBase(cleaner(tempdict['gl_TESTS_BASE']))
            if tempdict['gl_MAKEFILE_NAME']:
                self.cache.setMakefileName(cleaner(tempdict['gl_MAKEFILE_NAME']))
            if tempdict['gl_TESTS_MAKEFILE_NAME']:
                self.cache.setTestsMakefileName(cleaner(tempdict['gl_TESTS_MAKEFILE_NAME']))
            if tempdict['gl_MACRO_PREFIX']:
                self.cache.setMacroPrefix(cleaner(tempdict['gl_MACRO_PREFIX']))
            if tempdict['gl_PO_DOMAIN']:
                self.cache.setPoDomain(cleaner(tempdict['gl_PO_DOMAIN']))
            if tempdict['gl_WITNESS_C_MACRO']:
                self.cache.setWitnessCMacro(cleaner(tempdict['gl_WITNESS_C_MACRO']))
            if tempdict['gl_VC_FILES'] != '':
                self.cache.setVCFiles(cleaner(tempdict['gl_VC_FILES']))

            # Get cached filelist from gnulib-comp.m4.
            destdir, m4base = self.config.getDestDir(), self.config.getM4Base()
            path = joinpath(destdir, m4base, 'gnulib-comp.m4')
            if os.path.isfile(path):
                with open(path, mode='r', newline='\n', encoding='utf-8') as file:
                    data = file.read()
                regex = r'AC_DEFUN\(\[%s_FILE_LIST\], \[(.*?)\]\)' % self.cache['macro_prefix']
                pattern = re.compile(regex, re.S | re.M)
                matches = pattern.findall(data)
                if matches:
                    files = matches[-1].strip().split()
                else:
                    files = []
                self.cache.setFiles(files)

        # The self.config['localpath'] defaults to the cached one. Recall that
        # the cached one is relative to self.config['destdir'], whereas the one
        # we use is relative to . or absolute.
        if len(self.config['localpath']) == 0:
            if len(self.cache['localpath']) > 0:
                localpath = [ self.relative_to_currdir(localdir)
                              for localdir in self.cache['localpath'] ]
                self.config.setLocalPath(localpath)

        if self.mode == MODES['import']:
            self.config.setModules(sorted(self.config.getModules()))
        else:
            if self.cache['m4base'] and (self.config['m4base'] != self.cache['m4base']):
                raise GLError(5, m4base)

            # Perform actions with modules. In --add-import, append each given module
            # to the list of cached modules; in --remove-import, remove each given
            # module from the list of cached modules; in --update, simply set
            # self.config['modules'] to its cached version.
            new, old = self.config.getModules(), self.cache.getModules()
            if self.mode == MODES['add-import']:
                modules = sorted(set(new + old))
            elif self.mode == MODES['remove-import']:
                modules = [ module
                            for module in old
                            if not module in new ]
            elif self.mode == MODES['update']:
                modules = self.cache.getModules()

            # Merge the configuration found on disk.
            self.config.update(self.cache)

            self.config.setModules(modules)

            # Merge with the configuration from the command-line parameters;
            # they override the configuration found on disk.
            for key in config.keys():
                if key not in ['modules', 'avoids',
                               'incl_test_categories', 'excl_test_categories']:
                    value = config[key]
                    if not config.isdefault(key, value):
                        self.config.update_key(config, key)

        # Determine whether --automake-subdir/--automake-subdir-tests are supported.
        if self.config['automake_subdir'] or self.config['automake_subdir_tests']:
            found_subdir_objects = False
            if self.config['destdir']:
                with open(self.config['configure_ac'], mode='r', newline='\n', encoding='utf-8') as file:
                    data = file.read()
                pattern = re.compile(r'^.*AM_INIT_AUTOMAKE\([\[ ]*([^])]*).*$', re.MULTILINE)
                configure_ac_automake_options = pattern.findall(data)
                if configure_ac_automake_options:
                    automake_options = { x
                                         for y in configure_ac_automake_options
                                         for x in y.split() }
                    found_subdir_objects = 'subdir-objects' in automake_options
            if not found_subdir_objects:
                if self.config['destdir']:
                    base = self.config['destdir']
                else:
                    base = '.'
                if os.path.isfile(joinpath(base, 'Makefile.am')):
                    with open(joinpath(base, 'Makefile.am'), mode='r', newline='\n', encoding='utf-8') as file:
                        data = file.read()
                    pattern = re.compile(r'^AUTOMAKE_OPTIONS[\t ]*=(.*)$', re.MULTILINE)
                    automake_options = pattern.findall(data)
                    if automake_options:
                        automake_options = { x
                                             for y in automake_options
                                             for x in y.split() }
                        found_subdir_objects = 'subdir-objects' in automake_options
            if not found_subdir_objects:
                raise GLError(21, None)

        # Define GLImport attributes.
        self.emitter = GLEmiter(self.config)
        self.modulesystem = GLModuleSystem(self.config)
        self.moduletable = GLModuleTable(self.config,
                                         self.config.checkInclTestCategory(TESTS['all-tests']),
                                         self.config.checkInclTestCategory(TESTS['all-tests']))
        self.makefiletable = GLMakefileTable(self.config)

    def __repr__(self) -> str:
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLImport %s>' % hex(id(self))
        return result

    def actioncmd(self) -> str:
        '''Return command-line invocation comment.'''
        modules = self.config.getModules()
        avoids = self.config.getAvoids()
        localpath = self.config.getLocalPath()
        auxdir = self.config.getAuxDir()
        sourcebase = self.config.getSourceBase()
        m4base = self.config.getM4Base()
        docbase = self.config.getDocBase()
        pobase = self.config.getPoBase()
        testsbase = self.config.getTestsBase()
        conddeps = self.config.checkCondDeps()
        libname = self.config.getLibName()
        lgpl = self.config.getLGPL()
        gpl = self.config.getGPL()
        gnu_make = self.config.getGnuMake()
        makefile_name = self.config.getMakefileName()
        tests_makefile_name = self.config.getTestsMakefileName()
        automake_subdir = self.config.getAutomakeSubdir()
        automake_subdir_tests = self.config.getAutomakeSubdirTests()
        libtool = self.config.checkLibtool()
        macro_prefix = self.config.getMacroPrefix()
        witness_c_macro = self.config.getWitnessCMacro()
        podomain = self.config.getPoDomain()
        vc_files = self.config.checkVCFiles()

        # Command-line invocation printed in a comment in generated gnulib-cache.m4.
        actioncmd = '# gnulib-tool --import'

        # Break the action command log into multiple lines.
        # Emacs puts some gnulib-tool log lines in its source repository, and
        # git send-email rejects patch lines longer than 998 characters.
        # Also, config.status uses awk, and the HP-UX 11.00 awk fails if a
        # line has length >= 3071.
        if len(localpath) > 0:
            actioncmd += ''.join([f' \\\n#  --local-dir={x}' for x in localpath])
        actioncmd += ' \\\n#  --lib=%s' % libname
        actioncmd += ' \\\n#  --source-base=%s' % sourcebase
        actioncmd += ' \\\n#  --m4-base=%s' % m4base
        if pobase:
            actioncmd += ' \\\n#  --po-base=%s' % pobase
        actioncmd += ' \\\n#  --doc-base=%s' % docbase
        actioncmd += ' \\\n#  --tests-base=%s' % testsbase
        actioncmd += ' \\\n#  --aux-dir=%s' % auxdir
        if self.config.checkInclTestCategory(TESTS['tests']):
            actioncmd += ' \\\n#  --with-tests'
        if self.config.checkIncObsolete():
            actioncmd += ' \\\n#  --with-obsolete'
        if self.config.checkInclTestCategory(TESTS['c++-test']):
            actioncmd += ' \\\n#  --with-c++-tests'
        if self.config.checkInclTestCategory(TESTS['longrunning-test']):
            actioncmd += ' \\\n#  --with-longrunning-tests'
        if self.config.checkInclTestCategory(TESTS['privileged-test']):
            actioncmd += ' \\\n#  --with-privileged-tests'
        if self.config.checkInclTestCategory(TESTS['unportable-test']):
            actioncmd += ' \\\n#  --with-unportable-tests'
        if self.config.checkInclTestCategory(TESTS['all-test']):
            actioncmd += ' \\\n#  --with-all-tests'
        if lgpl:
            if lgpl == True:
                actioncmd += ' \\\n#  --lgpl'
            else:  # if lgpl != True
                actioncmd += ' \\\n#  --lgpl=%s' % lgpl
        if gpl:
            actioncmd += ' \\\n#  --gpl=%s' % gpl
        if gnu_make:
            actioncmd += ' \\\n#  --gnu-make'
        if makefile_name:
            actioncmd += ' \\\n#  --makefile-name=%s' % makefile_name
        if tests_makefile_name:
            actioncmd += ' \\\n#  --tests-makefile-name=%s' % tests_makefile_name
        if automake_subdir:
            actioncmd += ' \\\n#  --automake-subdir'
        if automake_subdir_tests:
            actioncmd += ' \\\n#  --automake-subdir-tests'
        if conddeps == True:
            actioncmd += ' \\\n#  --conditional-dependencies'
        else:  # if conddeps == False or conddeps == None
            actioncmd += ' \\\n#  --no-conditional-dependencies'
        if libtool == True:
            actioncmd += ' \\\n#  --libtool'
        else:  # if libtool == False or libtool == None
            actioncmd += ' \\\n#  --no-libtool'
        actioncmd += ' \\\n#  --macro-prefix=%s' % macro_prefix
        if podomain:
            actioncmd += ' \\\n#  --po-domain=%s' % podomain
        if witness_c_macro:
            actioncmd += ' \\\n#  --witness-c-macro=%s' % witness_c_macro
        if vc_files == True:
            actioncmd += ' \\\n#  --vc-files'
        elif vc_files == False:
            actioncmd += ' \\\n#  --no-vc-files'
        if len(avoids) > 0:
            actioncmd += ''.join([f' \\\n#  --avoid={x}' for x in avoids])
        if len(modules) > 0:
            actioncmd += ''.join([f' \\\n#  {x}' for x in modules])
        return actioncmd

    def relative_to_destdir(self, dir: str) -> str:
        '''Convert a filename that represents dir, relative to the current
        directory, to a filename relative to destdir.
        GLConfig: destdir.'''
        destdir = self.config['destdir']
        if os.path.isabs(dir):
            return dir
        else:
            if os.path.isabs(destdir):
                # XXX This doesn't look right.
                return dir
            else:
                return relativize(destdir, dir)

    def relative_to_currdir(self, dir: str) -> str:
        '''The opposite of GLImport.relative_to_destdir:
        Convert a filename that represents dir, relative to destdir,
        to a filename relative to the current directory.
        GLConfig: destdir.'''
        destdir = self.config['destdir']
        if os.path.isabs(dir):
            return dir
        else:
            if os.path.isabs(destdir):
                # XXX This doesn't look right.
                return joinpath(destdir, dir)
            else:
                return relconcat(destdir, dir)

    def gnulib_cache(self) -> str:
        '''Emit the contents of generated $m4base/gnulib-cache.m4 file.
        GLConfig: destdir, localpath, tests, sourcebase, m4base, pobase, docbase,
        testsbase, conddeps, libtool, macro_prefix, podomain, vc_files.'''
        emit = ''
        actioncmd = self.actioncmd()
        localpath = self.config['localpath']
        sourcebase = self.config['sourcebase']
        m4base = self.config['m4base']
        pobase = self.config['pobase']
        docbase = self.config['docbase']
        testsbase = self.config['testsbase']
        lgpl = self.config['lgpl']
        gpl = self.config['gpl']
        libname = self.config['libname']
        makefile_name = self.config['makefile_name']
        tests_makefile_name = self.config['tests_makefile_name']
        automake_subdir = self.config['automake_subdir']
        conddeps = self.config['conddeps']
        libtool = self.config['libtool']
        macro_prefix = self.config['macro_prefix']
        podomain = self.config['podomain']
        witness_c_macro = self.config['witness_c_macro']
        vc_files = self.config['vc_files']
        modules = self.config['modules']
        avoids = self.config['avoids']
        emit += self.emitter.copyright_notice()
        emit += '''#
# This file represents the specification of how gnulib-tool is used.
# It acts as a cache: It is written and read by gnulib-tool.
# In projects that use version control, this file is meant to be put under
# version control, like the configure.ac and various Makefile.am files.


# Specification in the form of a command-line invocation:
%s

# Specification in the form of a few gnulib-tool.m4 macro invocations:\n''' % actioncmd
        # Store the localpath relative to destdir.
        relative_localpath = [ self.relative_to_destdir(localdir)
                               for localdir in localpath ]
        emit += 'gl_LOCAL_DIR([%s])\n' % ':'.join(relative_localpath)
        emit += 'gl_MODULES([\n'
        emit += '  %s\n' % '\n  '.join(modules)
        emit += '])\n'
        if self.config.checkIncObsolete():
            emit += 'gl_WITH_OBSOLETE\n'
        if self.config.checkInclTestCategory(TESTS['cxx-tests']):
            emit += 'gl_WITH_CXX_TESTS\n'
        if self.config.checkInclTestCategory(TESTS['longrunning-tests']):
            emit += 'gl_WITH_LONGRUNNING_TESTS\n'
        if self.config.checkInclTestCategory(TESTS['privileged-tests']):
            emit += 'gl_WITH_PRIVILEGED_TESTS\n'
        if self.config.checkInclTestCategory(TESTS['unportable-tests']):
            emit += 'gl_WITH_UNPORTABLE_TESTS\n'
        if self.config.checkInclTestCategory(TESTS['all-tests']):
            emit += 'gl_WITH_ALL_TESTS\n'
        emit += 'gl_AVOID([%s])\n' % ' '.join(avoids)
        emit += 'gl_SOURCE_BASE([%s])\n' % sourcebase
        emit += 'gl_M4_BASE([%s])\n' % m4base
        emit += 'gl_PO_BASE([%s])\n' % pobase
        emit += 'gl_DOC_BASE([%s])\n' % docbase
        emit += 'gl_TESTS_BASE([%s])\n' % testsbase
        if self.config.checkInclTestCategory(TESTS['tests']):
            emit += 'gl_WITH_TESTS\n'
        emit += 'gl_LIB([%s])\n' % libname
        if lgpl != None:
            if lgpl == True:
                emit += 'gl_LGPL\n'
            else:  # if lgpl != True
                emit += 'gl_LGPL([%s])\n' % lgpl
        if gpl != None:
            emit += 'gl_GPL([%s])\n' % gpl
        emit += 'gl_MAKEFILE_NAME([%s])\n' % makefile_name
        if tests_makefile_name:
            emit += 'gl_TESTS_MAKEFILE_NAME([%s])\n' % tests_makefile_name
        if automake_subdir:
            emit += 'gl_AUTOMAKE_SUBDIR\n'
        if conddeps:
            emit += 'gl_CONDITIONAL_DEPENDENCIES\n'
        if libtool:
            emit += 'gl_LIBTOOL\n'
        emit += 'gl_MACRO_PREFIX([%s])\n' % macro_prefix
        emit += 'gl_PO_DOMAIN([%s])\n' % podomain
        emit += 'gl_WITNESS_C_MACRO([%s])\n' % witness_c_macro
        if vc_files != None:
            # Convert Python bools to shell (True -> true).
            emit += 'gl_VC_FILES([%s])\n' % str(vc_files).lower()
        return emit

    def gnulib_comp(self, filetable: GLFileTable, gentests: bool) -> str:
        '''Emit the contents of generated $m4base/gnulib-comp.m4 file.
        GLConfig: destdir, localpath, tests, sourcebase, m4base, pobase, docbase,
        testsbase, conddeps, libtool, macro_prefix, podomain, vc_files.

        filetable is a GLFileTable containing file information for this
          import.
        gentests is True if a tests Makefile.am is being generated, False
          otherwise.'''
        if type(filetable) is not GLFileTable:
            raise TypeError(f'filetable should be a GLFileTable, not {type(filetable).__name__}')
        if type(gentests) is not bool:
            raise TypeError(f'gentests should be a bool, not {type(gentests).__name__}')
        emit = ''
        moduletable = self.moduletable
        auxdir = self.config['auxdir']
        sourcebase = self.config['sourcebase']
        m4base = self.config['m4base']
        testsbase = self.config['testsbase']
        libname = self.config['libname']
        libtool = self.config['libtool']
        macro_prefix = self.config['macro_prefix']
        witness_c_macro = self.config['witness_c_macro']
        configure_ac = self.config['configure_ac']
        libtests = self.config['libtests']
        emit += '# DO NOT EDIT! GENERATED AUTOMATICALLY!\n'
        emit += self.emitter.copyright_notice()
        emit += '''#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects that use version control, this file can be treated like
# other built files.


# This macro should be invoked from %s, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([%s_EARLY],
[
  m4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace
  m4_pattern_allow([^gl_ES$])dnl a valid locale name
  m4_pattern_allow([^gl_LIBOBJS$])dnl a variable
  m4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable\n''' % (configure_ac, macro_prefix)
        emit += self.emitter.preEarlyMacros(True, '  ', moduletable.getFinalModules())
        for module in moduletable.getFinalModules():
            emit += '  # Code from module %s:\n' % module.name
            snippet = module.getAutoconfEarlySnippet()
            lines = [ line
                      for line in snippet.split('\n')
                      if line != '' ]
            if lines:
                emit += '  %s\n' % '\n  '.join(lines)
        emit += '])\n'
        emit += '''
# This macro should be invoked from %s, in the section
# "Check for header files, types and library functions".
AC_DEFUN([%s_INIT],
[\n''' % (configure_ac, macro_prefix)

        # This AC_CONFIG_LIBOBJ_DIR invocation silences an error from the automake
        # front end:
        #   error: required file './alloca.c' not found
        # It is needed because of the last remaining use of AC_LIBSOURCES in
        # _AC_LIBOBJ_ALLOCA, invoked from AC_FUNC_ALLOCA.
        # All the m4_pushdef/m4_popdef logic in func_emit_initmacro_start/_end
        # does not help to avoid this error.
        newfile_set = { pair[1]
                        for pair in filetable.new_files }
        if 'lib/alloca.c' in newfile_set:
            emit += '  AC_CONFIG_LIBOBJ_DIR([%s])\n' % sourcebase
        elif 'tests=lib/alloca.c' in newfile_set:
            # alloca.c will be present in $testsbase.
            emit += '  AC_CONFIG_LIBOBJ_DIR([%s])\n' % testsbase

        if libtool:
            emit += '  AM_CONDITIONAL([GL_COND_LIBTOOL], [true])\n'
            emit += '  gl_cond_libtool=true\n'
        else:  # if not libtool
            emit += '  AM_CONDITIONAL([GL_COND_LIBTOOL], [false])\n'
            emit += '  gl_cond_libtool=false\n'
            emit += '  gl_libdeps=\n'
            emit += '  gl_ltlibdeps=\n'
        replace_auxdir = False
        if auxdir != 'build-aux':
            replace_auxdir = True
        emit += '  gl_m4_base=\'%s\'\n' % m4base
        emit += self.emitter.initmacro_start(macro_prefix, False)
        emit += self.emitter.shellvars_init(False, sourcebase)
        if witness_c_macro:
            emit += '  m4_pushdef([gl_MODULE_INDICATOR_CONDITION], [%s])\n' % witness_c_macro
        # Emit main autoconf snippets.
        emit += self.emitter.autoconfSnippets(moduletable.getMainModules(), moduletable.getMainModules(),
                                              moduletable, lambda module: True, True, False, True, replace_auxdir)
        if witness_c_macro:
            emit += '  m4_popdef([gl_MODULE_INDICATOR_CONDITION])\n'
        emit += '  # End of code from modules\n'
        emit += self.emitter.initmacro_end(macro_prefix, False)
        emit += '  gltests_libdeps=\n'
        emit += '  gltests_ltlibdeps=\n'
        emit += self.emitter.initmacro_start('%stests' % macro_prefix, gentests)
        emit += self.emitter.shellvars_init(True, testsbase)
        # Define a tests witness macro that depends on the package.
        # PACKAGE is defined by AM_INIT_AUTOMAKE, PACKAGE_TARNAME is defined by
        # AC_INIT.
        # See <https://lists.gnu.org/r/automake/2009-05/msg00145.html>.
        emit += 'changequote(,)dnl\n'
        emit += '  %stests_WITNESS=' % macro_prefix
        emit += 'IN_`echo "${PACKAGE-$PACKAGE_TARNAME}" | LC_ALL=C tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ | LC_ALL=C sed -e \'s/[^A-Z0-9_]/_/g\'`_GNULIB_TESTS\n'
        emit += 'changequote([, ])dnl\n'
        emit += '  AC_SUBST([%stests_WITNESS])\n' % macro_prefix
        emit += '  gl_module_indicator_condition=$%stests_WITNESS\n' % macro_prefix
        emit += '  m4_pushdef([gl_MODULE_INDICATOR_CONDITION], '
        emit += '[$gl_module_indicator_condition])\n'
        # Emit tests autoconf snippets.
        emit += self.emitter.autoconfSnippets(moduletable.getTestsModules(),
                                              moduletable.getMainModules() + moduletable.getTestsModules(),
                                              moduletable, lambda module: True, True, True, True, replace_auxdir)
        emit += '  m4_popdef([gl_MODULE_INDICATOR_CONDITION])\n'
        emit += self.emitter.initmacro_end('%stests' % macro_prefix, gentests)
        emit += '  AC_REQUIRE([gl_CC_GNULIB_WARNINGS])\n'
        # _LIBDEPS and _LTLIBDEPS variables are not needed if this library is
        # created using libtool, because libtool already handles the dependencies.
        if not libtool:
            libname_upper = libname.upper().replace('-', '_')
            emit += '  %s_LIBDEPS="$gl_libdeps"\n' % libname_upper
            emit += '  AC_SUBST([%s_LIBDEPS])\n' % libname_upper
            emit += '  %s_LTLIBDEPS="$gl_ltlibdeps"\n' % libname_upper
            emit += '  AC_SUBST([%s_LTLIBDEPS])\n' % libname_upper
        if libtests:
            emit += '  LIBTESTS_LIBDEPS="$gltests_libdeps"\n'
            emit += '  AC_SUBST([LIBTESTS_LIBDEPS])\n'
        emit += '])\n'
        emit += self.emitter.initmacro_done(macro_prefix, sourcebase)
        emit += self.emitter.initmacro_done('%stests' % macro_prefix, testsbase)
        emit += '''
# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([%s_FILE_LIST], [\n''' % macro_prefix
        emit += '  %s\n' % '\n  '.join(filetable.all_files)
        emit += '])\n'
        return emit

    def _done_dir_(self, directory: str, files_added: list[str], files_removed: list[str]) -> None:
        '''This method is used to determine ignore argument for _update_ignorelist_
        method and then call it.'''
        destdir = self.config['destdir']
        if (os.path.isdir(joinpath(destdir, 'CVS'))
                or os.path.isdir(joinpath(destdir, directory, 'CVS'))
                or os.path.isfile(joinpath(destdir, directory, '.cvsignore'))):
            self._update_ignorelist_(directory, '.cvsignore',
                                     files_added, files_removed)
        if (os.path.isdir(joinpath(destdir, '.git'))
                or os.path.isfile(joinpath(destdir, '.gitignore'))
                or os.path.isfile(joinpath(destdir, directory, '.gitignore'))):
            self._update_ignorelist_(directory, '.gitignore',
                                     files_added, files_removed)

    def _update_ignorelist_(self, directory: str, ignore: str, files_added: list[str],
                            files_removed: list[str]) -> None:
        '''Update .gitignore or .cvsignore files.'''
        destdir = self.config['destdir']
        if ignore == '.gitignore':
            # In a .gitignore file, "foo" applies to the current directory and all
            # subdirectories, whereas "/foo" applies to the current directory only.
            anchor = '/'
        else:
            anchor = ''
        srcpath = joinpath(directory, ignore)
        backupname = '%s~' % srcpath
        if os.path.isfile(joinpath(destdir, srcpath)):
            if files_added or files_removed:
                with open(joinpath(destdir, srcpath), mode='r', newline='\n', encoding='utf-8') as file:
                    original_lines = file.readlines()
                # Clean the newlines but not trailing whitespace.
                original_lines = [ line.rstrip('\n')
                                   for line in original_lines ]
                already_listed_filenames = { substart(anchor, '', filename)
                                             for filename in original_lines
                                             if filename.strip() }
                filenames_to_add = set(files_added).difference(already_listed_filenames)
                filenames_to_remove = set(files_removed)
                if filenames_to_add or filenames_to_remove:
                    if not self.config['dryrun']:
                        print('Updating %s (backup in %s)' % (srcpath, backupname))
                        copyfile2(joinpath(destdir, srcpath), joinpath(destdir, backupname))
                        new_lines = original_lines + [ f'{anchor}{filename}'
                                                       for filename in sorted(filenames_to_add) ]
                        if anchor != '':
                            lines_to_remove = filenames_to_remove.union({ f'{anchor}{filename}'
                                                                          for filename in filenames_to_remove })
                        else:
                            lines_to_remove = filenames_to_remove
                        new_lines = [ line
                                      for line in new_lines
                                      if line not in lines_to_remove ]
                        with open(joinpath(destdir, srcpath), mode='w', newline='\n', encoding='utf-8') as file:
                            file.write(lines_to_multiline(new_lines))
                    else:  # if self.config['dryrun']
                        print('Update %s (backup in %s)' % (srcpath, backupname))
        else:  # if not os.path.isfile(joinpath(destdir, srcpath))
            if files_added:
                if not self.config['dryrun']:
                    print('Creating %s' % srcpath)
                    files_added = sorted(set(files_added))
                    files_added = [ '%s%s' % (anchor, f)
                                    for f in files_added ]
                    if ignore == '.cvsignore':
                        # Automake generates Makefile rules that create .dirstamp files.
                        files_added = ['.deps', '.dirstamp'] + files_added
                    with open(joinpath(destdir, srcpath), mode='w', newline='\n', encoding='utf-8') as file:
                        file.write(lines_to_multiline(files_added))
                else:  # if self.config['dryrun']
                    print('Create %s' % srcpath)

    def prepare(self) -> tuple[GLFileTable, dict[str, tuple[re.Pattern, str] | None]]:
        '''Perform preperations before GLImport.execute().
        Returns a filetable and the transformers passed to GLFileAssistant().'''
        destdir = self.config['destdir']
        modules = list(self.config['modules'])
        m4base = self.config['m4base']
        lgpl = self.config['lgpl']
        gpl = self.config['gpl']
        verbose = self.config['verbosity']
        base_modules = set()
        for name in modules:
            module = self.modulesystem.find(name)
            if module is not None:
                base_modules.add(module)
        base_modules = sorted(base_modules)

        # Perform transitive closure.
        final_modules = self.moduletable.transitive_closure(base_modules)

        # Show final module list.
        if verbose >= 0:
            (bold_on, bold_off) = bold_escapes()
            print('Module list with included dependencies (indented):')
            for module in final_modules:
                if module.name in self.config.getModules():
                    print('  %s%s%s' % (bold_on, module.name, bold_off))
                else:  # if module.name not in self.config.getModules()
                    print('    %s' % module.name)

        # Separate modules into main_modules and tests_modules.
        modules = \
            self.moduletable.transitive_closure_separately(base_modules, final_modules)
        main_modules, tests_modules = modules

        # Print main_modules and tests_modules.
        if verbose >= 1:
            print('Main module list:')
            for module in main_modules:
                print('  %s' % module.name)
            print('Tests-related module list:')
            for module in tests_modules:
                print('  %s' % module.name)

        # Determine whether a $testsbase/libtests.a is needed.
        libtests = False
        for module in tests_modules:
            files = module.getFiles()
            for file in files:
                if file.startswith('lib/'):
                    libtests = True
                    break
        if libtests:
            self.config.setLibtests(True)

        # Add the dummy module to the main module list if needed.
        main_modules = self.moduletable.add_dummy(main_modules)
        if libtests:  # if we need to use libtests.a
            # Add the dummy module to the tests-related module list if needed.
            tests_modules = self.moduletable.add_dummy(tests_modules)

        # Transmit base_modules, final_modules, main_modules and tests_modules.
        self.moduletable.setBaseModules(base_modules)
        self.moduletable.setFinalModules(final_modules)
        self.moduletable.setMainModules(main_modules)
        self.moduletable.setTestsModules(tests_modules)

        # Check license incompatibilities.
        listing = []
        compatibilities = dict()
        compatibilities['all'] = ['GPLv2+ build tool', 'GPLed build tool',
                                  'public domain', 'unlimited',
                                  'unmodifiable license text']
        compatibilities['GPLv3']         = ['LGPLv2+', 'LGPLv3+ or GPLv2+', 'LGPLv3+', 'LGPL', 'GPLv2+', 'GPLv3+', 'GPL']
        compatibilities['GPLv2']         = ['LGPLv2+', 'LGPLv3+ or GPLv2+', 'GPLv2+']
        compatibilities['LGPLv3']        = ['LGPLv2+', 'LGPLv3+ or GPLv2+', 'LGPLv3+', 'LGPL']
        compatibilities['LGPLv3orGPLv2'] = ['LGPLv2+', 'LGPLv3+ or GPLv2+']
        compatibilities['LGPLv2']        = ['LGPLv2+']
        if lgpl or gpl:
            for module in main_modules:
                license = module.getLicense()
                if license not in compatibilities['all']:
                    if lgpl == True:
                        if license not in compatibilities['LGPLv3']:
                            listing.append(tuple([module.name, license]))
                    elif lgpl:
                        if license not in compatibilities['LGPLv'+lgpl]:
                            listing.append(tuple([module.name, license]))
                    elif gpl:
                        if license not in compatibilities['GPLv'+gpl]:
                            listing.append(tuple([module.name, license]))
            if listing:
                raise GLError(11, listing)

        # Print notices from modules.
        if verbose >= -1:
            for module in main_modules:
                notice = module.getNotice().strip('\n')
                if notice:
                    print('Notice from module %s:' % module.name)
                    pattern = re.compile(r'^(.*?)$', re.S | re.M)
                    notice = pattern.sub(r'  \1', notice)
                    print(notice)

        # Determine script to apply to imported library files.
        sed_transform_lib_file = None
        if 'config-h' in [ module.name
                           for module in main_modules ]:
            sed_transform_lib_file = (re.compile(r'^#ifdef[\t ]*HAVE_CONFIG_H[\t ]*$', re.MULTILINE), r'#if 1')

        sed_transform_main_lib_file = sed_transform_lib_file

        # Determine script to apply to auxiliary files that go into $auxdir/.
        sed_transform_build_aux_file = None

        # Determine script to apply to library files that go into $testsbase/.
        sed_transform_testsrelated_lib_file = sed_transform_lib_file

        # Determine the final file lists.
        main_filelist, tests_filelist = \
            self.moduletable.filelist_separately(main_modules, tests_modules)
        filelist = sorted(set(main_filelist + tests_filelist))
        if not filelist:
            raise GLError(12, None)

        # Print list of files.
        if verbose >= 0:
            print('File list:')
            for file in filelist:
                if file.startswith('tests=lib/'):
                    rest = file[10:]
                    print('  lib/%s -> tests/%s' % (rest, rest))
                else:
                    print('  %s' % file)

        # Prepare basic filelist and basic old_files/new_files variables.
        filelist = sorted(set(filelist))
        # Add m4/gnulib-tool.m4 to the file list. It is not part of any module.
        new_files = filelist + ['m4/gnulib-tool.m4']
        old_files = list(self.cache['files'])
        path = joinpath(destdir, m4base, 'gnulib-tool.m4')
        if os.path.isfile(path):
            old_files.append(joinpath('m4', 'gnulib-tool.m4'))
        # old_files is the list of files according to the last gnulib-tool invocation.
        # new_files is the list of files after this gnulib-tool invocation.

        # Construct the transformers.
        transformers = dict()
        transformers['lib'] = sed_transform_lib_file
        transformers['aux'] = sed_transform_build_aux_file
        transformers['main'] = sed_transform_main_lib_file
        transformers['tests'] = sed_transform_testsrelated_lib_file

        # old_table is a table with two columns: (rewritten-file-name original-file-name),
        # representing the files according to the last gnulib-tool invocation.
        old_table = { (rewrite_file_name(file_name, self.cache, True), file_name)
                      for file_name in old_files }

        # new_table is a table with two columns: (rewritten-file-name original-file-name),
        # representing the files after this gnulib-tool invocation.
        new_table = { (rewrite_file_name(file_name, self.config, True), file_name)
                      for file_name in new_files }

        # Prepare the filetable.
        filetable = GLFileTable(sorted(set(filelist)))
        filetable.old_files = sorted(old_table, key=lambda pair: pair[0])
        filetable.new_files = sorted(new_table, key=lambda pair: pair[0])

        # Return the result.
        result = tuple([filetable, transformers])
        return result

    def execute(self, filetable: GLFileTable, transformers: dict[str, tuple[re.Pattern, str] | None]) -> None:
        '''Perform operations on the lists of files, which are given in a special
        format except filelist argument. Such lists of files can be created using
        GLImport.prepare() function.'''
        if type(filetable) is not GLFileTable:
            raise TypeError('filetable must be a GLFileTable, not %s'
                            % type(filetable).__name__)
        destdir = self.config['destdir']
        auxdir = self.config['auxdir']
        sourcebase = self.config['sourcebase']
        m4base = self.config['m4base']
        pobase = self.config['pobase']
        docbase = self.config['docbase']
        testsbase = self.config['testsbase']
        libname = self.config['libname']
        makefile_name = self.config['makefile_name']
        tests_makefile_name = self.config['tests_makefile_name']
        automake_subdir = self.config['automake_subdir']
        macro_prefix = self.config['macro_prefix']
        vc_files = self.config['vc_files']
        configure_ac = self.config['configure_ac']
        actioncmd = self.actioncmd()

        # Determine whether to put anything into $testsbase.
        testsfiles = [ file
                       for file in filetable.all_files
                       if file.startswith('tests/') or file.startswith('tests=lib/') ]
        gentests = len(testsfiles) > 0

        # Create all necessary directories.
        dirs = [sourcebase, m4base]
        if pobase:
            dirs.append(pobase)
        if [ file
             for file in filetable.all_files
             if file.startswith('doc/') ]:
            dirs.append(docbase)
        if gentests:
            dirs.append(testsbase)
        dirs.append(auxdir)
        dirs += sorted([ os.path.dirname(pair[0])
                         for pair in filetable.new_files ])
        dirs = [ os.path.join(destdir, d)
                 for d in dirs ]
        for directory in dirs:
            if not os.path.isdir(directory):
                print('Creating directory %s' % directory)
                if not self.config['dryrun']:
                    try:  # Try to create directory
                        os.makedirs(directory)
                    except Exception as exc:
                        raise GLError(13, directory) from exc
                else:  # if self.config['dryrun']
                    print('Create directory %s' % directory)

        # Create GLFileAssistant instance to process files.
        assistant = GLFileAssistant(self.config, transformers)

        # Set of rewritten-file-names from filetable.old_files.
        old_rewritten_files = { pair[0]
                                for pair in filetable.old_files }
        # Set of rewritten-file-names from filetable.new_files.
        new_rewritten_files = { pair[0]
                                for pair in filetable.new_files }

        # Files which are in filetable.old_files and not in filetable.new_files.
        # They will be removed and added to filetable.removed_files list.
        pairs = [ pair
                  for pair in filetable.old_files
                  if pair[0] not in new_rewritten_files ]
        pairs = sorted(set(pairs), key=lambda pair: pair[0])
        files = sorted(set(pair[0] for pair in pairs))
        for file in files:
            path = joinpath(destdir, file)
            if os.path.isfile(path) or os.path.islink(path):
                if not self.config['dryrun']:
                    backup = '%s~' % path
                    print('Removing file %s (backup in %s)' % (path, backup))
                    try:  # Try to move file
                        if os.path.exists(backup):
                            os.remove(backup)
                        movefile(path, '%s~' % path)
                    except Exception as exc:
                        raise GLError(14, file) from exc
                else:  # if self.config['dryrun']
                    print('Remove file %s (backup in %s~)' % (path, path))
                filetable.removed_files.append(file)

        # Files which are in filetable.new_files and not in filetable.old_files.
        # They will be added/updated and added to filetable.added_files list.
        already_present = False
        pairs = [ pair
                  for pair in filetable.new_files
                  if pair[0] not in old_rewritten_files ]
        pairs = sorted(set(pairs))
        for pair in pairs:
            original = pair[1]
            rewritten = pair[0]
            assistant.setOriginal(original)
            assistant.setRewritten(rewritten)
            assistant.add_or_update(already_present)

        # Files which are in filetable.new_files and in filetable.old_files.
        # They will be added/updated and added to filetable.added_files list.
        already_present = True
        pairs = [ pair
                  for pair in filetable.new_files
                  if pair[0] in old_rewritten_files ]
        pairs = sorted(set(pairs))
        for pair in pairs:
            original = pair[1]
            rewritten = pair[0]
            assistant.setOriginal(original)
            assistant.setRewritten(rewritten)
            assistant.add_or_update(already_present)

        # Add files which were added to the list of filetable.added_files.
        filetable.added_files += assistant.getFiles()
        filetable.added_files = sorted(set(filetable.added_files))

        # Default the source makefile name to Makefile.am.
        if makefile_name:
            source_makefile_am = makefile_name
        else:
            source_makefile_am = 'Makefile.am'
        # Default the tests makefile name to the source makefile name.
        if tests_makefile_name:
            tests_makefile_am = tests_makefile_name
        else:
            tests_makefile_am = source_makefile_am

        # Create normal Makefile.ams.
        for_test = False

        # Setup list of Makefile.am edits that are to be performed afterwards.
        # Some of these edits apply to files that we will generate; others are
        # under the responsibility of the developer.
        if source_makefile_am == 'Makefile.am':
            sourcebase_dir = os.path.dirname(sourcebase)
            sourcebase_base = os.path.basename(sourcebase)
            self.makefiletable.editor(sourcebase_dir, 'SUBDIRS', sourcebase_base)
        if pobase:
            pobase_dir = os.path.dirname(pobase)
            pobase_base = os.path.basename(pobase)
            self.makefiletable.editor(pobase_dir, 'SUBDIRS', pobase_base)
        if self.config.checkInclTestCategory(TESTS['tests']):
            if tests_makefile_am == 'Makefile.am':
                testsbase_dir = os.path.dirname(testsbase)
                testsbase_base = os.path.basename(testsbase)
                self.makefiletable.editor(testsbase_dir, 'SUBDIRS', testsbase_base, True)
        self.makefiletable.editor('', 'ACLOCAL_AMFLAGS', m4base)
        self.makefiletable.parent(gentests, source_makefile_am, tests_makefile_am)

        # Create po/ directory.
        filesystem = GLFileSystem(self.config)
        if pobase:
            # Create po makefile and auxiliary files.
            for file in ['Makefile.in.in', 'remove-potcdate.sin', 'remove-potcdate.sed']:
                tmpfile = assistant.tmpfilename(joinpath(pobase, file))
                path = joinpath('build-aux', 'po', file)
                if os.path.exists(joinpath(DIRS['root'], path)):
                    lookedup, flag = filesystem.lookup(path)
                    copyfile(lookedup, tmpfile)
                    basename = joinpath(pobase, file)
                    filename, backup, flag = assistant.super_update(basename, tmpfile)
                    if flag == 1:
                        if not self.config['dryrun']:
                            print('Updating %s (backup in %s)' % (filename, backup))
                        else:  # if self.config['dryrun']
                            print('Update %s (backup in %s)' % (filename, backup))
                    elif flag == 2:
                        if not self.config['dryrun']:
                            print('Creating %s' % filename)
                        else:  # if self.config['dryrun']:
                            print('Create %s' % filename)
                        filetable.added_files.append(filename)
                    if os.path.isfile(tmpfile):
                        os.remove(tmpfile)

            # Create po makefile parameterization, part 1.
            basename = joinpath(pobase, 'Makevars')
            tmpfile = assistant.tmpfilename(basename)
            emit = self.emitter.po_Makevars()
            with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
                file.write(emit)
            filename, backup, flag = assistant.super_update(basename, tmpfile)
            if flag == 1:
                if not self.config['dryrun']:
                    print('Updating %s (backup in %s)' % (filename, backup))
                else:  # if self.config['dryrun']
                    print('Update %s (backup in %s)' % (filename, backup))
            elif flag == 2:
                if not self.config['dryrun']:
                    print('Creating %s' % filename)
                else:  # if self.config['dryrun']:
                    print('Create %s' % filename)
                filetable.added_files.append(filename)
            if os.path.isfile(tmpfile):
                os.remove(tmpfile)

            # Create po makefile parameterization, part 2.
            basename = joinpath(pobase, 'POTFILES.in')
            tmpfile = assistant.tmpfilename(basename)
            with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
                file.write(self.emitter.po_POTFILES_in(filetable.all_files))
            basename = joinpath(pobase, 'POTFILES.in')
            filename, backup, flag = assistant.super_update(basename, tmpfile)
            if flag == 1:
                if not self.config['dryrun']:
                    print('Updating %s (backup in %s)' % (filename, backup))
                else:  # if self.config['dryrun']
                    print('Update %s (backup in %s)' % (filename, backup))
            elif flag == 2:
                if not self.config['dryrun']:
                    print('Creating %s' % filename)
                else:  # if self.config['dryrun']:
                    print('Create %s' % filename)
                filetable.added_files.append(filename)
            if os.path.isfile(tmpfile):
                os.remove(tmpfile)

            # Fetch PO files.
            TP_URL = 'https://translationproject.org/latest/'
            if not self.config['dryrun']:
                print('Fetching gnulib PO files from %s' % TP_URL)
                args = ['wget', '--no-verbose', '--mirror', '--level=1', '-nd', '-A.po', '-P', '.',
                        '%sgnulib/' % TP_URL]
                sp.call(args, cwd=joinpath(destdir, pobase))
            else:  # if self.config['dryrun']
                print('Fetch gnulib PO files from %s' % TP_URL)

            # Create po/LINGUAS.
            basename = joinpath(pobase, 'LINGUAS')
            if not self.config['dryrun']:
                tmpfile = assistant.tmpfilename(basename)
                data = '# Set of available languages.\n'
                files = sorted([ subend('.po', '', file)
                                 for file in os.listdir(joinpath(destdir, pobase))
                                 if file.endswith('.po') ])
                data += lines_to_multiline(files)
                with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
                    file.write(data)
                filename, backup, flag = assistant.super_update(basename, tmpfile)
                if flag == 1:
                    print('Updating %s (backup in %s)' % (filename, backup))
                elif flag == 2:
                    print('Creating %s' % filename)
                    filetable.added_files.append(filename)
                if os.path.isfile(tmpfile):
                    os.remove(tmpfile)
            else:  # if not self.config['dryrun']
                backupname = '%s~' % basename
                if os.path.isfile(joinpath(destdir, basename)):
                    print('Update %s (backup in %s)' % (basename, backupname))
                else:  # if not os.path.isfile(joinpath(destdir, basename))
                    print('Create %s' % basename)

        # Create m4/gnulib-cache.m4.
        basename = joinpath(m4base, 'gnulib-cache.m4')
        tmpfile = assistant.tmpfilename(basename)
        emit = self.gnulib_cache()
        with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
            file.write(emit)
        filename, backup, flag = assistant.super_update(basename, tmpfile)
        if flag == 1:
            if not self.config['dryrun']:
                print('Updating %s (backup in %s)' % (filename, backup))
            else:  # if self.config['dryrun']
                print('Update %s (backup in %s)' % (filename, backup))
        elif flag == 2:
            if not self.config['dryrun']:
                print('Creating %s' % filename)
            else:  # if self.config['dryrun']:
                print('Create %s' % filename)
                if emit[-2:] == '\r\n':
                    emit = emit[:-2]
                elif emit[-1:] == '\n':
                    emit = emit[:-1]
                print(emit)
        if os.path.isfile(tmpfile):
            os.remove(tmpfile)

        # Create m4/gnulib-comp.m4.
        basename = joinpath(m4base, 'gnulib-comp.m4')
        tmpfile = assistant.tmpfilename(basename)
        emit = self.gnulib_comp(filetable, gentests)
        with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
            file.write(emit)
        filename, backup, flag = assistant.super_update(basename, tmpfile)
        if flag == 1:
            if not self.config['dryrun']:
                print('Updating %s (backup in %s)' % (filename, backup))
            else:  # if self.config['dryrun']
                print('Update %s (backup in %s)' % (filename, backup))
        elif flag == 2:
            if not self.config['dryrun']:
                print('Creating %s' % filename)
            else:  # if self.config['dryrun']:
                print('Create %s' % filename)
                if emit[-2:] == '\r\n':
                    emit = emit[:-2]
                elif emit[-1:] == '\n':
                    emit = emit[:-1]
                print(emit)
        if os.path.isfile(tmpfile):
            os.remove(tmpfile)

        if self.config['gnu_make']:
            # Regenerate aclocal.m4.
            # This is needed because the next step may run 'autoconf -t' and
            # the preceding steps may have added new *.m4 files (which need to
            # be reflected in aclocal.m4 before 'autoconf -t' is run).
            aclocal_args = []
            for dir in self.m4dirs:
                aclocal_args.append('-I')
                aclocal_args.append(dir)
            sp.run([UTILS['aclocal']] + aclocal_args, cwd=destdir)

        # Create library makefile.
        # Do this after creating gnulib-comp.m4, because func_emit_lib_Makefile_am
        # can run 'autoconf -t', which reads gnulib-comp.m4.
        basename = joinpath(sourcebase, source_makefile_am)
        tmpfile = assistant.tmpfilename(basename)
        emit = self.emitter.lib_Makefile_am(basename, self.moduletable.getMainModules(),
                                            self.moduletable, self.makefiletable,
                                            actioncmd, for_test)
        if automake_subdir:
            emit = sp.run([joinpath(DIRS['root'], 'build-aux/prefix-gnulib-mk'), '--from-gnulib-tool',
                           f'--lib-name={libname}', f'--prefix={sourcebase}/'],
                          input=emit, text=True, capture_output=True).stdout
        with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
            file.write(emit)
        filename, backup, flag = assistant.super_update(basename, tmpfile)
        if flag == 1:
            if not self.config['dryrun']:
                print('Updating %s (backup in %s)' % (filename, backup))
            else:  # if self.config['dryrun']
                print('Update %s (backup in %s)' % (filename, backup))
        elif flag == 2:
            if not self.config['dryrun']:
                print('Creating %s' % filename)
            else:  # if self.config['dryrun']:
                print('Create %s' % filename)
            filetable.added_files.append(filename)
        if os.path.isfile(tmpfile):
            os.remove(tmpfile)

        # Create tests Makefile.
        if gentests:
            basename = joinpath(testsbase, tests_makefile_am)
            tmpfile = assistant.tmpfilename(basename)
            emit = self.emitter.tests_Makefile_am(basename, self.moduletable.getTestsModules(),
                                                  self.moduletable, self.makefiletable,
                                                  '%stests_WITNESS' % macro_prefix, for_test)
            with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
                file.write(emit)
            filename, backup, flag = assistant.super_update(basename, tmpfile)
            if flag == 1:
                if not self.config['dryrun']:
                    print('Updating %s (backup in %s)' % (filename, backup))
                else:  # if self.config['dryrun']
                    print('Update %s (backup in %s)' % (filename, backup))
            elif flag == 2:
                if not self.config['dryrun']:
                    print('Creating %s' % filename)
                else:  # if self.config['dryrun']:
                    print('Create %s' % filename)
                filetable.added_files.append(filename)
            if os.path.isfile(tmpfile):
                os.remove(tmpfile)

        if vc_files != False:
            # Update the .cvsignore and .gitignore files.
            ignorelist = []
            # Treat gnulib-comp.m4 like an added file, even if it already existed.
            filetable.added_files.append(joinpath(m4base, 'gnulib-comp.m4'))
            filetable.added_files = sorted(set(filetable.added_files))
            filetable.removed_files = sorted(set(filetable.removed_files))
            for file in filetable.added_files:
                directory, basename = os.path.split(file)
                ignorelist.append(tuple([directory, '|A|', basename]))
            for file in filetable.removed_files:
                directory, basename = os.path.split(file)
                ignorelist.append(tuple([directory, '|R|', basename]))
            # Sort ignorelist by directory.
            ignorelist = sorted(ignorelist, key=lambda row: row[0])
            last_dir = ''
            last_dir_files_added = []
            last_dir_files_removed = []
            for row in ignorelist:
                next_dir = row[0]
                operand = row[1]
                filename = row[2]
                if next_dir != last_dir:
                    self._done_dir_(last_dir, last_dir_files_added, last_dir_files_removed)
                    last_dir = next_dir
                    last_dir_files_added = []
                    last_dir_files_removed = []
                if operand == '|A|':
                    last_dir_files_added.append(filename)
                elif operand == '|R|':
                    last_dir_files_removed.append(filename)
            self._done_dir_(last_dir, last_dir_files_added, last_dir_files_removed)

        # Finish the work.
        print('Finished.\n')
        print('You may need to add #include directives for the following .h files.')
        # Intersect 'base' modules and 'main' modules
        # (since 'base' modules is not necessarily of subset of 'main' modules
        # - some may have been skipped through --avoid, and since the elements of
        # 'main' modules but not in 'base' modules can go away without explicit
        # notice - through changes in the module dependencies).
        modules = sorted(set(self.moduletable.getBaseModules()).intersection(self.moduletable.getMainModules()))
        # First the #include <...> directives without #ifs, sorted for convenience,
        # then the #include "..." directives without #ifs, sorted for convenience,
        # then the #include directives that are surrounded by #ifs. Not sorted.
        include_angles = []
        include_quotes = []
        include_if = []
        for module in modules:
            include = module.getInclude()
            if '\n#if' in '\n'+include:
                include_if += [ f'  {line}'
                                for line in include.split('\n')
                                if line.strip() ]
            else:
                include_angles += [ f'  {line}'
                                    for line in include.split('\n')
                                    if 'include "' not in line and line.strip() ]
                include_quotes += [ f'  {line}'
                                    for line in include.split('\n')
                                    if 'include "' in line and line.strip() ]

        includes = lines_to_multiline(sorted(set(include_angles)))
        includes += lines_to_multiline(sorted(set(include_quotes)))
        includes += lines_to_multiline(include_if)
        print(includes, end='')

        # Get link directives.
        links = [ module.getLink()
                  for module in self.moduletable.getMainModules() ]
        lines = [ line
                  for link in links
                  for line in link.split('\n')
                  if line != '' ]
        lines = sorted(set(lines))
        if lines:
            print('''
You may need to use the following Makefile variables when linking.
Use them in <program>_LDADD when linking a program, or
in <library>_a_LDFLAGS or <library>_la_LDFLAGS when linking a library.''')
            for line in lines:
                print('  %s' % line)

        # Print reminders.
        print('')
        print('Don\'t forget to')
        if source_makefile_am == 'Makefile.am':
            print('  - add "%s/Makefile" to AC_CONFIG_FILES in %s,' % (sourcebase, configure_ac))
        else:  # if makefile_am != 'Makefile.am'
            print('  - "include %s" from within "%s/Makefile.am",' % (source_makefile_am, sourcebase))
        if pobase:
            print('  - add "%s/Makefile.in" to AC_CONFIG_FILES in %s,' % (pobase, configure_ac))
        if gentests:
            if tests_makefile_am == 'Makefile.am':
                print('  - add "%s/Makefile" to AC_CONFIG_FILES in %s,' % (testsbase, configure_ac))
            else:  # if makefile_am != 'Makefile.am'
                print('  - "include %s" from within "%s/Makefile.am",' % (tests_makefile_am, testsbase))
        # Print makefile edits.
        for current_edit in range(0, self.makefiletable.count()):
            dictionary = self.makefiletable[current_edit]
            if 'var' in dictionary:
                if dictionary['var'] == 'ACLOCAL_AMFLAGS':
                    print('  - mention "-I %s" in %s in %s'
                          % (dictionary['val'], dictionary['var'], joinpath(dictionary['dir'], 'Makefile.am')))
                    print('    or add an AC_CONFIG_MACRO_DIRS([%s]) invocation in %s,'
                          % (dictionary['val'], configure_ac))
                else:
                    print('  - mention "%s" in %s in %s,'
                          % (dictionary['val'], dictionary['var'], joinpath(dictionary['dir'], 'Makefile.am')))

        # Detect position_early_after.
        with open(configure_ac, mode='r', newline='\n', encoding='utf-8') as file:
            data = file.read()
        match_result1 = re.compile(r'^ *AC_PROG_CC_STDC', re.MULTILINE).search(data)
        match_result2 = re.compile(r'^ *AC_PROG_CC_C99', re.MULTILINE).search(data)
        if match_result1:
            print('  - replace AC_PROG_CC_STDC with AC_PROG_CC in %s,' % (configure_ac))
            position_early_after = 'AC_PROG_CC_STDC'
        elif match_result2:
            print('  - replace AC_PROG_CC_C99 with AC_PROG_CC in %s,' % (configure_ac))
            position_early_after = 'AC_PROG_CC_C99'
        else:  # if not any([match_result1, match_result2])
            position_early_after = 'AC_PROG_CC'
        print('  - invoke %s_EARLY in %s, right after %s,' % (macro_prefix, configure_ac, position_early_after))
        print('  - invoke %s_INIT in %s.' % (macro_prefix, configure_ac))
