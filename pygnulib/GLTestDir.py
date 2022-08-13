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
import subprocess as sp
from . import constants
from .GLError import GLError
from .GLConfig import GLConfig
from .GLModuleSystem import GLModuleTable
from .GLModuleSystem import GLModuleSystem
from .GLFileSystem import CopyAction
from .GLFileSystem import GLFileSystem
from .GLFileSystem import GLFileAssistant
from .GLMakefileTable import GLMakefileTable
from .GLEmiter import GLEmiter


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
UTILS = constants.UTILS
TESTS = constants.TESTS
joinpath = constants.joinpath
relinverse = constants.relinverse
copyfile = constants.copyfile
movefile = constants.movefile
isdir = os.path.isdir
isfile = os.path.isfile
normpath = os.path.normpath


#===============================================================================
# Define GLTestDir class
#===============================================================================
class GLTestDir(object):
    '''GLTestDir class is used to create a scratch package with the given
    list of the modules.'''

    def __init__(self, config, testdir):
        '''GLTestDir.__init__(config, testdir) -> GLTestDir

        Create new GLTestDir instance.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if type(testdir) is not str:
            raise TypeError('testdir must be a string, not %s'
                            % type(testdir).__name__)
        self.config = config
        self.testdir = os.path.normpath(testdir)
        if not os.path.exists(self.testdir):
            try:  # Try to create directory
                os.mkdir(self.testdir)
            except Exception as error:
                raise GLError(19, self.testdir)
        self.emiter = GLEmiter(self.config)
        self.filesystem = GLFileSystem(self.config)
        self.modulesystem = GLModuleSystem(self.config)
        self.assistant = GLFileAssistant(self.config)
        self.makefiletable = GLMakefileTable(self.config)

        # Subdirectory names.
        self.config.setSourceBase('gllib')
        self.config.setM4Base('glm4')
        self.config.setDocBase('gldoc')
        self.config.setTestsBase('gltests')
        self.config.setMacroPrefix('gl')
        self.config.resetPoBase()
        self.config.resetPoDomain()
        self.config.resetWitnessCMacro()
        self.config.resetVCFiles()

    def rewrite_files(self, files):
        '''GLTestDir.rewrite_files(files)

        Replace auxdir, docbase, sourcebase, m4base and testsbase from default
        to their version from config.'''
        if type(files) is not list:
            raise TypeError('files argument must have list type, not %s'
                            % type(files).__name__)
        for file in files:
            if type(file) is not str:
                raise TypeError('each file must be a string instance')
        files = sorted(set(files))
        auxdir = self.config['auxdir']
        docbase = self.config['docbase']
        sourcebase = self.config['sourcebase']
        m4base = self.config['m4base']
        testsbase = self.config['testsbase']
        result = list()
        for file in files:
            if file.startswith('build-aux/'):
                path = constants.substart('build-aux/', '%s/' % auxdir, file)
            elif file.startswith('doc/'):
                path = constants.substart('doc/', '%s/' % docbase, file)
            elif file.startswith('lib/'):
                path = constants.substart('lib/', '%s/' % sourcebase, file)
            elif file.startswith('m4/'):
                path = constants.substart('m4/', '%s/' % m4base, file)
            elif file.startswith('tests/'):
                path = constants.substart('tests/', '%s/' % testsbase, file)
            elif file.startswith('tests=lib/'):
                path = constants.substart('tests=lib/', '%s/' % testsbase, file)
            elif file.startswith('top/'):
                path = constants.substart('top/', '', file)
            else:  # file is not a special file
                path = file
            result += [os.path.normpath(path)]
        result = sorted(set(result))
        return list(result)

    def execute(self):
        '''GLTestDir.execute()

        Create a scratch package with the given modules.'''
        auxdir = self.config['auxdir']
        sourcebase = self.config['sourcebase']
        m4base = self.config['m4base']
        pobase = self.config['pobase']
        docbase = self.config['docbase']
        testsbase = self.config['testsbase']
        libname = self.config['libname']
        libtool = self.config['libtool']
        witness_c_macro = self.config['witness_c_macro']
        single_configure = self.config['single_configure']
        include_guard_prefix = self.config['include_guard_prefix']
        macro_prefix = self.config['macro_prefix']
        verbose = self.config['verbosity']

        specified_modules = self.config['modules']
        if len(specified_modules) == 0:
            # All modules together.
            # Except config-h, which breaks all modules which use HAVE_CONFIG_H.
            # Except non-recursive-gnulib-prefix-hack, which represents a
            # nonstandard way of using Automake.
            # Except ftruncate, mountlist, which abort the configuration on mingw.
            # Except lib-ignore, which leads to link errors when Sun C++ is used.
            specified_modules = self.modulesystem.list()
            specified_modules = [module
                                 for module in specified_modules
                                 if module not in ['config-h', 'non-recursive-gnulib-prefix-hack',
                                                   'ftruncate', 'mountlist', 'lib-ignore']]

        # Canonicalize the list of specified modules.
        specified_modules = sorted(set(specified_modules))
        specified_modules = [ self.modulesystem.find(m)
                              for m in specified_modules ]

        # Test modules which invoke AC_CONFIG_FILES cannot be used with
        # --with-tests --single-configure. Avoid them.
        inctests = self.config.checkInclTestCategory(TESTS['tests'])
        if inctests and self.config.checkSingleConfigure():
            self.config.addAvoid('havelib-tests')

        # Now it's time to create the GLModuleTable.
        moduletable = GLModuleTable(self.config,
                                    True,
                                    self.config.checkInclTestCategory(TESTS['all-tests']))

        # When computing transitive closures, don't consider $module to depend on
        # $module-tests. Need this because tests are implicitly GPL and may depend
        # on GPL modules - therefore we don't want a warning in this case.
        saved_inctests = self.config.checkInclTestCategory(TESTS['tests'])
        self.config.disableInclTestCategory(TESTS['tests'])
        for requested_module in specified_modules:
            requested_licence = requested_module.getLicense()
            if requested_licence != 'GPL':
                # Here we use moduletable.transitive_closure([module]), not
                # just module.getDependencies, so that we also detect weird
                # situations like an LGPL module which depends on a GPLed build
                # tool module which depends on a GPL module.
                modules = moduletable.transitive_closure([requested_module])
                for module in modules:
                    license = module.getLicense()
                    if license not in ['GPLv2+ build tool', 'GPLed build tool',
                                       'public domain', 'unlimited', 'unmodifiable license text']:
                        incompatible = False
                        if requested_licence == 'GPLv3+' or requested_licence == 'GPL':
                            if license not in ['LGPLv2+', 'LGPLv3+ or GPLv2', 'LGPLv3+', 'LGPL', 'GPLv2+', 'GPLv3+', 'GPL']:
                                incompatible = True
                        elif requested_licence == 'GPLv2+':
                            if license not in ['LGPLv2+', 'LGPLv3+ or GPLv2', 'GPLv2+']:
                                incompatible = True
                        elif requested_licence == 'LGPLv3+' or requested_licence == 'LGPL':
                            if license not in ['LGPLv2+', 'LGPLv3+ or GPLv2', 'LGPLv3+', 'LGPL']:
                                incompatible = True
                        elif requested_licence == 'LGPLv3+ or GPLv2':
                            if license not in ['LGPLv2+', 'LGPLv3+ or GPLv2']:
                                incompatible = True
                        elif requested_licence == 'LGPLv2+':
                            if license not in ['LGPLv2+']:
                                incompatible = True
                        if incompatible:
                            errormsg = 'module %s depends on a module with an incompatible license: %s\n' % (requested_module, module)
                            sys.stderr.write(errormsg)
        self.config.setInclTestCategory(TESTS['tests'], saved_inctests)

        # Determine final module list.
        modules = moduletable.transitive_closure(specified_modules)
        final_modules = list(modules)

        # Show final module list.
        if verbose >= 0:
            bold_on = ''
            bold_off = ''
            term = os.getenv('TERM')
            if term == 'xterm':
                bold_on = '\x1b[1m'
                bold_off = '\x1b[0m'
            print('Module list with included dependencies (indented):')
            for module in final_modules:
                if str(module) in self.config.getModules():
                    print('  %s%s%s' % (bold_on, module, bold_off))
                else:  # if str(module) not in self.config.getModules()
                    print('    %s' % module)

        # Generate lists of the modules.
        if single_configure:
            # Determine main module list and tests-related module list separately.
            main_modules, tests_modules = \
                moduletable.transitive_closure_separately(specified_modules, final_modules)
            # Print main_modules and tests_modules.
            if verbose >= 1:
                print('Main module list:')
                for module in main_modules:
                    print('  %s' % str(module))
                print('Tests-related module list:')
                for module in tests_modules:
                    print('  %s' % str(module))
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

        if single_configure:
            # Add the dummy module to the main module list if needed.
            main_modules = moduletable.add_dummy(main_modules)
            if libtests:  # if we need to use libtests.a
                # Add the dummy module to the tests-related module list if needed.
                tests_modules = moduletable.add_dummy(tests_modules)
        else:  # if not single_configure
            modules = moduletable.add_dummy(modules)

        # Show banner notice of every module.
        if single_configure:
            for module in main_modules:
                notice = module.getNotice()
                if notice:
                    print('Notice from module %s:' % str(module))
                    pattern = re.compile('^(.*)$', re.M)
                    notice = pattern.sub('  \\1', notice)
                    print(notice)
        else:  # if not single_configure
            for module in modules:
                notice = module.getNotice()
                if notice:
                    print('Notice from module %s:' % str(module))
                    pattern = re.compile('^(.*)$', re.M)
                    notice = pattern.sub('  \\1', notice)
                    print(notice)

        # Determine final file list.
        if single_configure:
            main_filelist, tests_filelist = \
                moduletable.filelist_separately(main_modules, tests_modules)
            filelist = sorted(set(main_filelist + tests_filelist))
        else:  # if not single_configure
            filelist = moduletable.filelist(modules)

        filelist = sorted(set(filelist))

        # Print list of files.
        if verbose >= 0:
            print('File list:')
            for file in filelist:
                if file.startswith('tests=lib/'):
                    rest = file[10:]
                    print('  lib/%s -> tests/%s' % (rest, rest))
                else:
                    print('  %s' % file)

        # Add files for which the copy in gnulib is newer than the one that
        # "automake --add-missing --copy" would provide.
        filelist += ['build-aux/config.guess', 'build-aux/config.sub']
        filelist = sorted(set(filelist))

        # Create directories.
        directories = [os.path.dirname(file)
                       for file in self.rewrite_files(filelist)]
        directories = sorted(set(directories))

        # Copy files or make symbolic links.
        filetable = list()
        for src in filelist:
            dest = self.rewrite_files([src])[-1]
            filetable += [tuple([dest, src])]
        for row in filetable:
            src = row[1]
            dest = row[0]
            destpath = joinpath(self.testdir, dest)
            dirname = os.path.dirname(destpath)
            if not isdir(dirname):
                os.makedirs(dirname)
            if src.startswith('tests=lib/'):
                src = constants.substart('tests=lib/', 'lib/', src)
            lookedup, flag = self.filesystem.lookup(src)
            if isfile(destpath):
                os.remove(destpath)
            if flag:
                copyfile(lookedup, destpath)
            else:  # if not flag
                if self.filesystem.shouldLink(src, lookedup) == CopyAction.Symlink:
                    constants.link_relative(lookedup, destpath)
                else:
                    copyfile(lookedup, destpath)

        # Create $sourcebase/Makefile.am.
        for_test = True
        directory = joinpath(self.testdir, sourcebase)
        if not isdir(directory):
            os.mkdir(directory)
        destfile = joinpath(directory, 'Makefile.am')
        if single_configure:
            emit, uses_subdirs = self.emiter.lib_Makefile_am(destfile, main_modules,
                                                             moduletable, self.makefiletable, '', for_test)
        else:  # if not single_configure
            emit, uses_subdirs = self.emiter.lib_Makefile_am(destfile, modules,
                                                             moduletable, self.makefiletable, '', for_test)
        with codecs.open(destfile, 'wb', 'UTF-8') as file:
            file.write(emit)
        any_uses_subdirs = uses_subdirs

        # Create $m4base/Makefile.am.
        directory = joinpath(self.testdir, m4base)
        if not isdir(directory):
            os.mkdir(directory)
        destfile = joinpath(directory, 'Makefile.am')
        emit = '## Process this file with automake to produce Makefile.in.\n\n'
        emit += 'EXTRA_DIST =\n'
        for file in filelist:
            if file.startswith('m4/'):
                file = constants.substart('m4/', '', file)
                emit += 'EXTRA_DIST += %s\n' % file
        emit = constants.nlconvert(emit)
        with codecs.open(destfile, 'wb', 'UTF-8') as file:
            file.write(emit)

        subdirs = [sourcebase, m4base]
        subdirs_with_configure_ac = list()

        testsbase_appened = False
        inctests = self.config.checkInclTestCategory(TESTS['tests'])
        if inctests:
            directory = joinpath(self.testdir, testsbase)
            if not isdir(directory):
                os.mkdir(directory)
            if single_configure:
                # Create $testsbase/Makefile.am.
                destfile = joinpath(directory, 'Makefile.am')
                witness_macro = '%stests_WITNESS' % macro_prefix
                emit, uses_subdirs = self.emiter.tests_Makefile_am(destfile,
                                                                   tests_modules, self.makefiletable, witness_macro, for_test)
                with codecs.open(destfile, 'wb', 'UTF-8') as file:
                    file.write(emit)
            else:  # if not single_configure
                # Create $testsbase/Makefile.am.
                destfile = joinpath(directory, 'Makefile.am')
                libtests = False
                self.config.setLibtests(False)
                emit, uses_subdirs = self.emiter.tests_Makefile_am(destfile,
                                                                   modules, self.makefiletable, '', for_test)
                with codecs.open(destfile, 'wb', 'UTF-8') as file:
                    file.write(emit)
                # Viewed from the $testsbase subdirectory, $auxdir is different.
                emit = ''
                auxdir = os.path.normpath(joinpath(relinverse(testsbase), auxdir))
                self.config.setAuxDir(auxdir)
                # Create $testsbase/configure.ac.
                emit += '# Process this file with autoconf '
                emit += 'to produce a configure script.\n'
                emit += 'AC_INIT([dummy], [0])\n'
                emit += 'AC_CONFIG_AUX_DIR([%s])\n' % auxdir
                emit += 'AM_INIT_AUTOMAKE\n\n'
                emit += 'AC_CONFIG_HEADERS([config.h])\n\n'
                emit += 'AC_PROG_CC\n'
                emit += 'AC_PROG_INSTALL\n'
                emit += 'AC_PROG_MAKE_SET\n'
                emit += self.emiter.preEarlyMacros(False, '', modules)
                if uses_subdirs:
                    emit += 'AM_PROG_CC_C_O\n\n'
                snippets = list()
                for module in modules:
                    if str(module) in ['gnumakefile', 'maintainer-makefile']:
                        # These are meant to be used only in the top-level directory.
                        pass
                    # if str(module) not in ['gnumakefile', 'maintainer-makefile']
                    else:
                        snippet = module.getAutoconfEarlySnippet()
                        lines = [ line
                                  for line in snippet.split('\n')
                                  if line.strip() ]
                        snippet = '\n'.join(lines)
                        pattern = re.compile('AC_REQUIRE\\(\\[([^()]*)\\]\\)', re.M)
                        snippet = pattern.sub('\\1', snippet)
                        snippet = snippet.strip()
                        snippets += [snippet]
                snippets = [ snippet
                             for snippet in snippets
                             if snippet.strip()]
                emit += '%s\n' % '\n'.join(snippets)
                if libtool:
                    emit += 'LT_INIT([win32-dll])\n'
                    emit += 'LT_LANG([C++])\n'
                    emit += 'AM_CONDITIONAL([GL_COND_LIBTOOL], [true])\n'
                    emit += 'gl_cond_libtool=true\n'
                else:  # if not libtool
                    emit += 'AM_CONDITIONAL([GL_COND_LIBTOOL], [false])\n'
                    emit += 'gl_cond_libtool=false\n'
                    emit += 'gl_libdeps=\n'
                    emit += 'gl_ltlibdeps=\n'
                # Wrap the set of autoconf snippets into an autoconf macro that is then
                # invoked. This is needed because autoconf does not support AC_REQUIRE
                # at the top level:
                #   error: AC_REQUIRE(gt_CSHARPCOMP): cannot be used outside of an
                #     AC_DEFUN'd macro
                # but we want the AC_REQUIRE to have its normal meaning (provide one
                # expansion of the required macro before the current point, and only
                # one expansion total).
                emit += 'AC_DEFUN([gl_INIT], [\n'
                replace_auxdir = True
                emit += "gl_m4_base='../%s'\n" % m4base
                emit += self.emiter.initmacro_start(macro_prefix)
                # We don't have explicit ordering constraints between the various
                # autoconf snippets. It's cleanest to put those of the library before
                # those of the tests.
                emit += "gl_source_base='../%s'\n" % sourcebase
                emit += self.emiter.autoconfSnippets(modules,
                                                     moduletable, 1, False, False, False,
                                                     replace_auxdir)
                emit += "gl_source_base='.'"
                emit += self.emiter.autoconfSnippets(modules,
                                                     moduletable, 2, False, False, False,
                                                     replace_auxdir)
                emit += self.emiter.initmacro_end(macro_prefix)
                # _LIBDEPS and _LTLIBDEPS variables are not needed if this library is
                # created using libtool, because libtool already handles the
                # dependencies.
                if not libtool:
                    libname_upper = libname.upper().replace('-', '_')
                    emit += '  %s_LIBDEPS="$gl_libdeps"\n' % libname_upper
                    emit += '  AC_SUBST([%s_LIBDEPS])\n' % libname_upper
                    emit += '  %s_LTLIBDEPS="$gl_ltlibdeps"\n' % libname_upper
                    emit += '  AC_SUBST([%s_LTLIBDEPS])\n' % libname_upper
                emit += '])\n'
                # FIXME use $sourcebase or $testsbase?
                emit += self.emiter.initmacro_done(macro_prefix, sourcebase)
                emit += '\ngl_INIT\n\n'
                # Usually $testsbase/config.h will be a superset of config.h. Verify
                # this by "merging" config.h into $testsbase/config.h; look out for gcc
                # warnings.
                emit += 'AH_TOP([#include \"../config.h\"])\n\n'
                emit += 'AC_CONFIG_FILES([Makefile])\n'
                emit += 'AC_OUTPUT\n'
                emit = constants.nlconvert(emit)
                path = joinpath(self.testdir, testsbase, 'configure.ac')
                with codecs.open(path, 'wb', 'UTF-8') as file:
                    file.write(emit)

                # Restore changed variables.
                self.config.setAuxDir(saved_auxdir)
                auxdir = self.config['auxdir']
                subdirs_with_configure_ac += [testsbase]

            subdirs += [testsbase]
            testsbase_appened = True

        # Create Makefile.am.
        emit = '## Process this file with automake to produce Makefile.in.\n\n'
        emit += 'AUTOMAKE_OPTIONS = 1.9.6 foreign\n\n'
        emit += 'SUBDIRS = %s\n\n' % ' '.join(subdirs)
        emit += 'ACLOCAL_AMFLAGS = -I %s\n' % m4base
        emit = constants.nlconvert(emit)
        path = joinpath(self.testdir, 'Makefile.am')
        with codecs.open(path, 'wb', 'UTF-8') as file:
            file.write(emit)

        # Create configure.ac
        emit = '# Process this file with autoconf '
        emit += 'to produce a configure script.\n'
        emit += 'AC_INIT([dummy], [0])\n'
        if auxdir != '.':
            emit += 'AC_CONFIG_AUX_DIR([%s])\n' % auxdir
        emit += 'AM_INIT_AUTOMAKE\n\n'
        emit += 'AC_CONFIG_HEADERS([config.h])\n\n'
        emit += 'AC_PROG_CC\n'
        emit += 'AC_PROG_INSTALL\n'
        emit += 'AC_PROG_MAKE_SET\n\n'
        emit += '# For autobuild.\n'
        emit += 'AC_CANONICAL_BUILD\n'
        emit += 'AC_CANONICAL_HOST\n\n'
        emit += 'm4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace\n'
        emit += 'm4_pattern_allow([^gl_ES$])dnl a valid locale name\n'
        emit += 'm4_pattern_allow([^gl_LIBOBJS$])dnl a variable\n'
        emit += 'm4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable\n'
        emit += self.emiter.preEarlyMacros(False, '', modules)
        if any_uses_subdirs:
            emit += 'AM_PROG_CC_C_O\n'
        snippets = list()
        for module in final_modules:
            if single_configure:
                solution = True
            else:  # if not single_configure
                solution = module.isNonTests()
            if solution:
                snippet = module.getAutoconfEarlySnippet()
                lines = [ line
                          for line in snippet.split('\n')
                          if line.strip() ]
                snippet = '\n'.join(lines)
                pattern = re.compile('AC_REQUIRE\\(\\[([^()]*)\\]\\)', re.M)
                snippet = pattern.sub('\\1', snippet)
                snippet = snippet.strip()
                snippets += [snippet]
        snippets = [ snippet
                     for snippet in snippets
                     if snippet.strip() ]
        emit += '%s\n' % '\n'.join(snippets)
        if libtool:
            emit += 'LT_INIT([win32-dll])\n'
            emit += 'LT_LANG([C++])\n'
            emit += 'AM_CONDITIONAL([GL_COND_LIBTOOL], [true])\n'
            emit += 'gl_cond_libtool=true\n'
        else:  # if not libtool
            emit += 'AM_CONDITIONAL([GL_COND_LIBTOOL], [false])\n'
            emit += 'gl_cond_libtool=false\n'
            emit += 'gl_libdeps=\n'
            emit += 'gl_ltlibdeps=\n'
        # Wrap the set of autoconf snippets into an autoconf macro that is then
        # invoked. This is needed because autoconf does not support AC_REQUIRE
        # at the top level:
        #   error: AC_REQUIRE(gt_CSHARPCOMP): cannot be used outside of an
        #     AC_DEFUN'd macro
        # but we want the AC_REQUIRE to have its normal meaning (provide one
        # expansion of the required macro before the current point, and only one
        # expansion total).
        emit += 'AC_DEFUN([gl_INIT], [\n'
        if auxdir != 'build-aux':
            replace_auxdir = True
        else:  # auxdir == 'build-aux'
            replace_auxdir = False
        emit += 'gl_m4_base=\'%s\'\n' % m4base
        emit += self.emiter.initmacro_start(macro_prefix)
        emit += 'gl_source_base=\'%s\'\n' % sourcebase
        if single_configure:
            emit += self.emiter.autoconfSnippets(main_modules, moduletable,
                                                 0, False, False, False, replace_auxdir)
        else:  # if not single_configure
            emit += self.emiter.autoconfSnippets(modules, moduletable,
                                                 1, False, False, False, replace_auxdir)
        emit += self.emiter.initmacro_end(macro_prefix)
        if single_configure:
            emit += '  gltests_libdeps=\n'
            emit += '  gltests_ltlibdeps=\n'
            emit += self.emiter.initmacro_start('%stests' % macro_prefix)
            emit += '  gl_source_base=\'%s\'\n' % testsbase
            # Define a tests witness macro.
            emit += '  %stests_WITNESS=IN_GNULIB_TESTS\n' % macro_prefix
            emit += '  AC_SUBST([%stests_WITNESS])\n' % macro_prefix
            emit += '  gl_module_indicator_condition=$%stests_WITNESS\n' % macro_prefix
            emit += '  m4_pushdef([gl_MODULE_INDICATOR_CONDITION], '
            emit += '[$gl_module_indicator_condition])\n'
            snippets = self.emiter.autoconfSnippets(tests_modules, moduletable,
                                                    1, True, False, False, replace_auxdir)
            emit += snippets.strip()
            emit += '  m4_popdef([gl_MODULE_INDICATOR_CONDITION])\n'
            emit += self.emiter.initmacro_end('%stests' % macro_prefix)
        # _LIBDEPS and _LTLIBDEPS variables are not needed if this library is
        # created using libtool, because libtool already handles the dependencies.
        if not libtool:
            libname_upper = libname.upper().replace('-', '_')
            emit += '  %s_LIBDEPS="$gl_libdeps"\n' % libname_upper
            emit += '  AC_SUBST([%s_LIBDEPS])\n' % libname_upper
            emit += '  %s_LTLIBDEPS="$gl_ltlibdeps"\n' % libname_upper
            emit += '  AC_SUBST([%s_LTLIBDEPS])\n' % libname_upper
        if single_configure and libtests:
            emit += '  LIBTESTS_LIBDEPS="$gltests_libdeps"\n'
            emit += '  AC_SUBST([LIBTESTS_LIBDEPS])\n'
        emit += '])\n'
        emit += self.emiter.initmacro_done(macro_prefix, sourcebase)
        if single_configure:
            emit += self.emiter.initmacro_done('%stests' % macro_prefix, testsbase)
        emit += '\ngl_INIT\n\n'
        if subdirs_with_configure_ac:
            if single_configure:
                emit += 'AC_CONFIG_SUBDIRS([%s])\n' % ' '.join(subdirs_with_configure_ac[:-1])
            else:  # if not single_configure
                emit += 'AC_CONFIG_SUBDIRS([%s])\n' % ' '.join(subdirs_with_configure_ac)
        makefiles = ['Makefile']
        for directory in subdirs:
            # For subdirs that have a configure.ac by their own, it's the subdir's
            # configure.ac which creates the subdir's Makefile.am, not this one.
            makefiles += [joinpath(directory, 'Makefile')]
        if not single_configure:
            makefiles = makefiles[:-1]
        emit += 'AC_CONFIG_FILES([%s])\n' % ' '.join(makefiles)
        emit += 'AC_OUTPUT\n'
        path = joinpath(self.testdir, 'configure.ac')
        with codecs.open(path, 'wb', 'UTF-8') as file:
            file.write(emit)

        # Create autogenerated files.
        # Do not use "${AUTORECONF} --force --install", because it may invoke
        # autopoint, which brings in older versions of some of our .m4 files.
        os.chdir(self.testdir)
        # gettext
        if isfile(joinpath(m4base, 'gettext.m4')):
            args = [UTILS['autopoint'], '--force']
            constants.execute(args, verbose)
            for src in os.listdir(m4base):
                src = joinpath(m4base, src)
                if src.endswith('.m4~'):
                    dest = src[:-1]
                    if isfile(dest):
                        os.remove(dest)
                    movefile(src, dest)
        # libtoolize
        if libtool:
            args = [UTILS['libtoolize'], '--copy']
            constants.execute(args, verbose)
        # aclocal
        args = [UTILS['aclocal'], '-I', m4base]
        constants.execute(args, verbose)
        if not isdir('build-aux'):
            os.mkdir('build-aux')
        # autoconf
        args = [UTILS['autoconf']]
        constants.execute(args, verbose)
        # autoheader
        args = [UTILS['autoheader']]
        constants.execute(args, verbose)
        # automake
        args = [UTILS['automake'], '--add-missing', '--copy']
        constants.execute(args, verbose)
        os.chdir(DIRS['cwd'])
        if inctests and not single_configure:
            # Do not use "${AUTORECONF} --force --install", because it may invoke
            # autopoint, which brings in older versions of some of our .m4 files.
            os.chdir(joinpath(self.testdir, testsbase))
            # gettext
            if isfile(joinpath(m4base, 'gettext.m4')):
                args = [UTILS['autopoint'], '--force']
                constants.execute(args, verbose)
                for src in os.listdir(m4base):
                    src = joinpath(m4base, src)
                    if src.endswith('.m4~'):
                        dest = src[:-1]
                        if isfile(dest):
                            os.remove(dest)
                        movefile(src, dest)
            # aclocal
            args = [UTILS['aclocal'], '-I', joinpath('..', m4base)]
            constants.execute(args, verbose)
            if not isdir(joinpath('../build-aux')):
                os.mkdir('../build-aux')
            # autoconf
            args = [UTILS['autoconf']]
            constants.execute(args, verbose)
            # autoheader
            args = [UTILS['autoheader']]
            constants.execute(args, verbose)
            # automake
            args = [UTILS['automake'], '--add-missing', '--copy']
            constants.execute(args, verbose)
            os.chdir(DIRS['cwd'])

        # Need to run configure and make once, to create built files that are to be
        # distributed (such as parse-datetime.c).
        path = joinpath(self.testdir, sourcebase, 'Makefile.am')
        with codecs.open(path, 'rb', 'UTF-8') as file:
            snippet = file.read()
        snippet = constants.remove_backslash_newline(snippet)
        cleaned_files = list()
        tests_cleaned_files = list()
        built_sources = list()
        tests_built_sources = list()
        distributed_built_sources = list()
        tests_distributed_built_sources = list()

        # Extract the value of "CLEANFILES += ..." and "MOSTLYCLEANFILES += ...".
        regex_find = list()
        pattern = re.compile('^CLEANFILES[\t ]*\\+=(.*)$', re.M)
        regex_find += pattern.findall(snippet)
        pattern = re.compile('^MOSTLYCLEANFILES[\t ]*\\+=(.*)$', re.M)
        regex_find += pattern.findall(snippet)
        regex_find = [ line.strip()
                       for line in regex_find
                       if line.strip() ]
        for part in regex_find:
            cleaned_files += \
                [ line.strip()
                  for line in part.split(' ')
                  if line.strip() ]

        # Extract the value of "BUILT_SOURCES += ...". Remove variable references
        # such $(FOO_H) because they don't refer to distributed files.
        regex_find = list()
        pattern = re.compile('^BUILT_SOURCES[\t ]*\\+=(.*)$', re.M)
        regex_find += pattern.findall(snippet)
        regex_find = [ line.strip()
                       for line in regex_find
                       if line.strip()]
        for part in regex_find:
            built_sources += \
                [ line.strip()
                  for line in part.split(' ')
                  if line.strip()]
        built_sources = [ line
                          for line in built_sources
                          if not bool(re.compile('[$]\\([A-Za-z0-9_]*\\)$').findall(line)) ]
        distributed_built_sources = [ file
                                      for file in built_sources
                                      if file not in cleaned_files ]

        if inctests:
            # Likewise for built files in the $testsbase directory.
            path = joinpath(self.testdir, testsbase, 'Makefile.am')
            with codecs.open(path, 'rb', 'UTF-8') as file:
                snippet = file.read()
            snippet = constants.remove_backslash_newline(snippet)

            # Extract the value of "CLEANFILES += ..." and "MOSTLYCLEANFILES += ...".
            regex_find = list()
            pattern = re.compile('^CLEANFILES[\t ]*\\+=(.*)$', re.M)
            regex_find += pattern.findall(snippet)
            pattern = re.compile('^MOSTLYCLEANFILES[\t ]*\\+=(.*)$', re.M)
            regex_find += pattern.findall(snippet)
            regex_find = [ line.strip()
                           for line in regex_find
                           if line.strip() ]
            for part in regex_find:
                tests_cleaned_files += \
                    [ line.strip()
                      for line in part.split(' ')
                      if line.strip() ]

            # Extract the value of "BUILT_SOURCES += ...". Remove variable references
            # such $(FOO_H) because they don't refer to distributed files.
            regex_find = list()
            tests_built_sources = list()
            pattern = re.compile('^BUILT_SOURCES[\t ]*\\+=(.*)$', re.M)
            regex_find += pattern.findall(snippet)
            regex_find = [ line.strip()
                           for line in regex_find
                           if line.strip() ]
            for part in regex_find:
                tests_built_sources += \
                    [ line.strip()
                      for line in part.split(' ')
                      if line.strip() ]
            tests_built_sources = [ line
                                    for line in tests_built_sources
                                    if not bool(re.compile('[$]\\([A-Za-z0-9_]*\\)$').findall(line)) ]
            tests_distributed_built_sources = [ file
                                                for file in tests_built_sources
                                                if file not in cleaned_files]

        if distributed_built_sources or tests_distributed_built_sources:
            os.chdir(self.testdir)
            sp.call('./configure')
            if distributed_built_sources:
                os.chdir(sourcebase)
                with codecs.open('Makefile', 'ab', 'UTF-8') as file:
                    file.write('built_sources: $(BUILT_SOURCES)\n')
                args = [UTILS['make'],
                        'AUTOCONF=%s' % UTILS['autoconf'],
                        'AUTOHEADER=%s' % UTILS['autoheader'],
                        'ACLOCAL=%s' % UTILS['aclocal'],
                        'AUTOMAKE=%s' % UTILS['automake'],
                        'AUTORECONF=%s' % UTILS['autoreconf'],
                        'built_sources']
                sp.call(args)
                os.chdir('..')
            if tests_distributed_built_sources:
                os.chdir(testsbase)
                with codecs.open('Makefile', 'ab', 'UTF-8') as file:
                    file.write('built_sources: $(BUILT_SOURCES)\n')
                args = [UTILS['make'],
                        'AUTOCONF=%s' % UTILS['autoconf'],
                        'AUTOHEADER=%s' % UTILS['autoheader'],
                        'ACLOCAL=%s' % UTILS['aclocal'],
                        'AUTOMAKE=%s' % UTILS['automake'],
                        'AUTORECONF=%s' % UTILS['autoreconf'],
                        'built_sources']
                sp.call(args)
                os.chdir('..')
            args = [UTILS['make'],
                    'AUTOCONF=%s' % UTILS['autoconf'],
                    'AUTOHEADER=%s' % UTILS['autoheader'],
                    'ACLOCAL=%s' % UTILS['aclocal'],
                    'AUTOMAKE=%s' % UTILS['automake'],
                    'AUTORECONF=%s' % UTILS['autoreconf'],
                    'AUTOPOINT=%s' % UTILS['autopoint'],
                    'LIBTOOLIZE=%s' % UTILS['libtoolize'],
                    'distclean']
            sp.call(args)
        sp.call(['rm', '-rf', self.config['tempdir']], shell=False)


#===============================================================================
# Define GLMegaTestDir class
#===============================================================================
class GLMegaTestDir(object):
    '''GLMegaTestDir class is used to create a mega scratch package with the
    given modules one by one and all together.'''

    def __init__(self, config, megatestdir):
        '''GLMegaTestDir.__init__(config, megatestdir) -> GLMegaTestDir

        Create new GLTestDir instance.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if type(megatestdir) is not str:
            raise TypeError('megatestdir must be a string, not %s'
                            % type(megatestdir).__name__)
        self.config = config
        self.megatestdir = os.path.normpath(megatestdir)
        if not os.path.exists(self.megatestdir):
            try:  # Try to create directory
                os.mkdir(self.megatestdir)
            except Exception as error:
                raise GLError(19, self.megatestdir)
        self.emiter = GLEmiter(self.config)
        self.filesystem = GLFileSystem(self.config)
        self.modulesystem = GLModuleSystem(self.config)
        self.assistant = GLFileAssistant(self.config)
        self.makefiletable = GLMakefileTable(self.config)

    def execute(self):
        '''GLMegaTestDir.execute()

        Create a mega scratch package with the given modules one by one and all
        together.'''
        auxdir = self.config['auxdir']
        verbose = self.config['verbosity']

        megasubdirs = list()
        modules = [ self.modulesystem.find(m)
                    for m in self.config['modules'] ]
        if not modules:
            modules = self.modulesystem.list()
            modules = [ self.modulesystem.find(m)
                        for m in modules ]
        modules = sorted(set(modules))

        # First, all modules one by one.
        for module in modules:
            self.config.setModules([str(module)])
            #GLTestDir(self.config, self.megatestdir).execute()
            megasubdirs += [str(module)]

        # Then, all modules all together.
        # Except config-h, which breaks all modules which use HAVE_CONFIG_H.
        modules = [ module
                    for module in modules
                    if str(module) != 'config-h' ]
        self.config.setModules([ str(module)
                                 for module in modules ])
        #GLTestDir(self.config, self.megatestdir).execute()
        megasubdirs += ['ALL']

        # Create autobuild.
        emit = ''
        repdict = dict()
        repdict['Jan'] = repdict['January'] = '01'
        repdict['Feb'] = repdict['February'] = '02'
        repdict['Mar'] = repdict['March'] = '03'
        repdict['Apr'] = repdict['April'] = '04'
        repdict['May'] = repdict['May'] = '05'
        repdict['Jun'] = repdict['June'] = '06'
        repdict['Jul'] = repdict['July'] = '07'
        repdict['Aug'] = repdict['August'] = '08'
        repdict['Sep'] = repdict['September'] = '09'
        repdict['Oct'] = repdict['October'] = '10'
        repdict['Nov'] = repdict['November'] = '11'
        repdict['Dec'] = repdict['December'] = '12'
        vc_witness = joinpath(DIRS['root'], '.git', 'refs', 'heads', 'master')
        mdate_sh = joinpath(DIRS['root'], 'build-aux', 'mdate-sh')
        args = ['sh', mdate_sh, vc_witness]
        cvsdate = sp.check_output(args).decode("UTF-8").strip()
        for key in repdict:
            if len(key) > 3:
                cvsdate = cvsdate.replace(key, repdict[key])
        for key in repdict:
            cvsdate = cvsdate.replace(key, repdict[key])
        cvsdate = ''.join([ date
                            for date in cvsdate.split(' ')
                            if date.strip() ])
        cvsdate = '%s%s%s' % (cvsdate[4:], cvsdate[2:4], cvsdate[:2])
        emit += '#!/bin/sh\n'
        emit += 'CVSDATE=%s\n' % cvsdate
        emit += ': ${MAKE=make}\n'
        emit += 'test -d logs || mkdir logs\n'
        emit += 'for module in %s; do\n' % ' '.join(megasubdirs)
        emit += '  echo "Working on module $module..."\n'
        emit += '  safemodule=`echo $module | sed -e \'s|/|-|g\'`\n'
        emit += '  (echo "To: gnulib@autobuild.josefsson.org"\\\n'
        emit += '   echo\n'
        emit += '   set -x\n'
        emit += '   : autobuild project... $module\n'
        emit += '   : autobuild revision... cvs-$CVSDATE-000000\n'
        emit += '   : autobuild timestamp... `date "+%Y%m%d-%H%M%S"`\n'
        emit += '   : autobuild hostname... `hostname`\n'
        emit += '   cd $module && ./configure $CONFIGURE_OPTIONS && $MAKE'
        emit += ' && $MAKE check && $MAKE distclean\n'
        emit += '   echo rc=$?\n'
        emit += '  ) 2>&1 | { if test -n "$AUTOBUILD_SUBST"; then '
        emit += 'sed -e "$AUTOBUILD_SUBST"; else cat; fi; } > logs/$safemodule\n'
        emit += 'done\n'
        emit = constants.nlconvert(emit)
        path = joinpath(self.megatestdir, 'do-autobuild')
        with codecs.open(path, 'wb', 'UTF-8') as file:
            file.write(emit)

        # Create Makefile.am.
        emit = '## Process this file with automake to produce Makefile.in.\n\n'
        emit += 'AUTOMAKE_OPTIONS = 1.9.6 foreign\n\n'
        emit += 'SUBDIRS = %s\n\n' % ' '.join(megasubdirs)
        emit += 'EXTRA_DIST = do-autobuild\n'
        emit = constants.nlconvert(emit)
        path = joinpath(self.megatestdir, 'Makefile.am')
        with codecs.open(path, 'wb', 'UTF-8') as file:
            file.write(emit)

        emit = '# Process this file with autoconf '
        emit += 'to produce a configure script.\n'
        emit += 'AC_INIT([dummy], [0])\n\n'
        if auxdir != '.':
            emit += 'AC_CONFIG_AUX_DIR([%s])\n' % auxdir
        emit += 'AM_INIT_AUTOMAKE\n\n'
        emit += 'AC_PROG_MAKE_SET\n\n'
        emit += 'AC_CONFIG_SUBDIRS([%s])\n' % ' '.megasubdirs
        emit += 'AC_CONFIG_FILES([Makefile])\n'
        emit += 'AC_OUTPUT\n'
        emit = constants.nlconvert(emit)
        path = joinpath(self.megatestdir, 'Makefile.am')
        with codecs.open(path, 'wb', 'UTF-8') as file:
            file.write(emit)

        # Create autogenerated files.
        os.chdir(DIRS['cwd'])
        args = [UTILS['aclocal']]
        constants.execute(args, verbose)
        try:  # Try to make a directory
            if not isdir('build-aux'):
                os.mkdir('build-aux')
        except Exception as error:
            pass
        args = [UTILS['autoconf']]
        constants.execute(args, verbose)
        args = [UTILS['automake'], '--add-missing', '--copy']
        constants.execute(args, verbose)
        sp.call(['rm', '-rf', self.config['tempdir']], shell=False)
