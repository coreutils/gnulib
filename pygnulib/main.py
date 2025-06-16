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
#

# This program is meant for authors or maintainers which want to import
# modules from gnulib into their packages.

# This program assumes a Python version >= 3.7.

# CODING STYLE for this file and its companions:
# Like PEP 8 <https://peps.python.org/pep-0008/>, except
# - Line length is not limited to 79 characters.
# - Line breaking before or after binary operators? Better before, like in GNU.
# - Place line breaks to help reading the code:
#   Avoid line breaks inside expressions, if they can be avoided.
#   Do use line breaks to separate the parts of [ ... for ... ] iterations.
# - String literals: Use 'single-quotes'.
#   Rationale: <https://lists.gnu.org/archive/html/bug-gnulib/2024-02/msg00226.html>
# - Use raw string syntax for regular expression pattern strings and repl
#   strings:
#     re.compile(r'...').sub(r'...', ...)
#     re.sub(r'...', r'...', ...)
#   Rationale: This avoids the need to double backslashes, making the pattern
#   and repl strings easier to understand.
# - Comparison operators:
#   Use == and != for variables which can contain only strings, numbers, lists,
#   and None.
#   Use 'is' and 'is not' for variables which can contain only class instances
#   (e.g. GLModule instances) and None.
#   Rationale: <https://lists.gnu.org/archive/html/bug-gnulib/2024-02/msg00225.html>
# - Avoid conditional expressions as in PEP 308 <https://peps.python.org/pep-0308/>.
#   Rationale: They violate the principle that in conditional code, the
#   condition comes first.
# - Use type hints to document functions and catch programming mistakes.
#   To allow for type annotations and union type specifiers, using the
#   syntax Type1 | Type2, for all versions of Python ≥ 3.7 use the
#   'from __future__ import annotations' import at the top of files.
#   Cf. <https://docs.python.org/3/library/__future__.html>
#   and <https://stackoverflow.com/questions/73879925/>.
# - Prefer the standard collection type hints over those provided in the
#   typing module. The latter are deprecated and may be removed.
#   Cf. <https://peps.python.org/pep-0585/>.
# - Never use the {} literal, because it denotes a dictionary,
#   as opposed to {x}, {x,y}, etc., which denote sets.
# - Use a leading underscore to indicate that functions and instance variables
#   should be considered private. This is a widely accepted convention from
#   PEP 8 <https://peps.python.org/pep-0008/#method-names-and-instance-variables>.

# You can use this command to check the style:
#   $ pycodestyle *.py
# The pycodestyle configuration is found in the file setup.cfg.
# Documentation:
# <https://pycodestyle.pycqa.org/en/latest/intro.html#error-codes>

# You can use this command to check for mistakes:
#   $ pylint *.py
# The pylint configuration is found in the file .pylintrc.
# Note: This configuration is only effective when you invoke pylint from this
# directory.
# Documentation:
# <https://pylint.readthedocs.io/en/latest/user_guide/messages/messages_overview.html>

from __future__ import annotations

#===============================================================================
# Define global imports
#===============================================================================
import os
import re
import sys
import random
import argparse
import subprocess as sp
import shlex
import tempfile
from pygnulib.constants import (
    APP,
    DIRS,
    TESTS,
    UTILS,
    MODES,
    ENCS,
    joinpath,
    lines_to_multiline,
    ensure_writable,
    copyfile,
    force_output,
    init_DIRS,
    rmtree,
    DEFAULT_AUTOCONF_MINVERSION,
)
from pygnulib.functions import rewrite_file_name
from pygnulib.enums import CopyAction
from pygnulib.GLConfig import GLConfig
from pygnulib.GLError import GLError
from pygnulib.GLFileSystem import GLFileSystem
from pygnulib.GLFileSystem import GLFileAssistant
from pygnulib.GLImport import GLImport
from pygnulib.GLInfo import GLInfo
from pygnulib.GLModuleSystem import GLModuleSystem
from pygnulib.GLTestDir import GLTestDir
from pygnulib.GLTestDir import GLMegaTestDir


#===============================================================================
# Define main part
#===============================================================================
def main(temp_directory: str) -> None:
    info = GLInfo()
    parser = argparse.ArgumentParser(
        prog=APP['name'],
        usage='gnulib-tool.py --help',
        add_help=False)

    # Here we list the options in the order they are listed in the --help output.

    # list
    parser.add_argument('--list',
                        dest='mode_list',
                        default=None,
                        action='store_true')
    # find
    parser.add_argument('--find',
                        dest='mode_find',
                        default=None,
                        action='store_true')
    # import
    parser.add_argument('--import',
                        dest='mode_import',
                        default=None,
                        action='store_true')
    # add-import
    parser.add_argument('--add-import',
                        dest='mode_add_import',
                        default=None,
                        action='store_true')
    # remove-import
    parser.add_argument('--remove-import',
                        dest='mode_remove_import',
                        default=None,
                        action='store_true')
    # update
    parser.add_argument('--update',
                        dest='mode_update',
                        default=None,
                        action='store_true')
    # create-testdir
    parser.add_argument('--create-testdir',
                        dest='mode_create_testdir',
                        default=None,
                        action='store_true')
    # create-megatestdir
    parser.add_argument('--create-megatestdir',
                        dest='mode_create_megatestdir',
                        default=None,
                        action='store_true')
    # test
    parser.add_argument('--test',
                        dest='mode_test',
                        default=None,
                        action='store_true')
    # megatest
    parser.add_argument('--megatest',
                        dest='mode_megatest',
                        default=None,
                        action='store_true')
    # extract-*
    parser.add_argument('--extract-description',
                        dest='mode_xdescription',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-comment',
                        dest='mode_xcomment',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-status',
                        dest='mode_xstatus',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-notice',
                        dest='mode_xnotice',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-applicability',
                        dest='mode_xapplicability',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-usability-in-testdir',
                        dest='mode_xusability_in_testdir',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-filelist',
                        dest='mode_xfilelist',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-dependencies',
                        dest='mode_xdependencies',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-recursive-dependencies',
                        dest='mode_xrecursive_dependencies',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-dependents',
                        dest='mode_xdependents',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-recursive-dependents',
                        dest='mode_xrecursive_dependents',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-autoconf-snippet',
                        dest='mode_xautoconf',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-automake-snippet',
                        dest='mode_xautomake',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-include-directive',
                        dest='mode_xinclude',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-link-directive',
                        dest='mode_xlink',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-recursive-link-directive',
                        dest='mode_xrecursive_link',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-license',
                        dest='mode_xlicense',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-maintainer',
                        dest='mode_xmaintainer',
                        default=None,
                        action='store_true')
    parser.add_argument('--extract-tests-module',
                        dest='mode_xtests',
                        default=None,
                        action='store_true')
    # copy-file
    parser.add_argument('--copy-file',
                        dest='mode_copy_file',
                        default=None,
                        action='store_true')
    # help
    parser.add_argument('--help', '--hel', '--he', '--h',
                        dest='help',
                        default=None,
                        action='store_true')
    # version
    parser.add_argument('--version', '--versio', '--versi', '--vers',
                        dest='version',
                        default=None,
                        action='store_true')
    # no-changelog: a no-op for backward compatibility
    parser.add_argument('--no-changelog',
                        dest='changelog',
                        default=None,
                        action='store_false')
    # destdir
    parser.add_argument('--dir',
                        dest='destdir',
                        default=None,
                        nargs=1)
    # localpath
    parser.add_argument('--local-dir',
                        action='append',
                        dest='localpath',
                        default=None,
                        nargs=1)
    # cache-modules: a no-op for backward compatibility
    parser.add_argument('--cache-modules',
                        dest='cache_modules',
                        default=None,
                        action='store_true')
    parser.add_argument('--no-cache-modules',
                        dest='cache_modules',
                        default=None,
                        action='store_false')
    # verbose
    parser.add_argument('--verbose',
                        default=0,
                        action='count')
    # quiet
    parser.add_argument('--quiet',
                        default=0,
                        action='count')
    # dryrun
    parser.add_argument('--dry-run',
                        dest='dryrun',
                        default=None,
                        action='store_true')
    # inctests
    parser.add_argument('--with-tests',
                        dest='inctests',
                        default=None,
                        action='store_true')
    parser.add_argument('--without-tests',
                        dest='inctests',
                        default=None,
                        action='store_false')
    # obsolete
    parser.add_argument('--with-obsolete',
                        dest='obsolete',
                        default=None,
                        action='store_true')
    # c++-tests
    parser.add_argument('--with-c++-tests',
                        dest='inc_cxx_tests',
                        default=None,
                        action='store_true')
    parser.add_argument('--without-c++-tests',
                        dest='excl_cxx_tests',
                        default=None,
                        action='store_true')
    # longrunning-tests
    parser.add_argument('--with-longrunning-tests',
                        dest='inc_longrunning_tests',
                        default=None,
                        action='store_true')
    parser.add_argument('--without-longrunning-tests',
                        dest='excl_longrunning_tests',
                        default=None,
                        action='store_true')
    # privileged-tests
    parser.add_argument('--with-privileged-tests',
                        dest='inc_privileged_tests',
                        default=None,
                        action='store_true')
    parser.add_argument('--without-privileged-tests',
                        dest='excl_privileged_tests',
                        default=None,
                        action='store_true')
    # unportable-tests
    parser.add_argument('--with-unportable-tests',
                        dest='inc_unportable_tests',
                        default=None,
                        action='store_true')
    parser.add_argument('--without-unportable-tests',
                        dest='excl_unportable_tests',
                        default=None,
                        action='store_true')
    # all-tests
    parser.add_argument('--with-all-tests',
                        dest='alltests',
                        default=None,
                        action='store_true')
    # avoids
    parser.add_argument('--avoid',
                        dest='avoids',
                        default=None,
                        action='append',
                        nargs=1)
    # conditional-dependencies
    parser.add_argument('--conditional-dependencies',
                        dest='cond_dependencies',
                        default=None,
                        action='store_true')
    parser.add_argument('--no-conditional-dependencies',
                        dest='cond_dependencies',
                        default=None,
                        action='store_false')
    # libtool
    parser.add_argument('--libtool',
                        dest='libtool',
                        default=None,
                        action='store_true')
    parser.add_argument('--no-libtool',
                        dest='libtool',
                        default=None,
                        action='store_false')
    # libname
    parser.add_argument('--lib',
                        dest='libname',
                        default=None,
                        nargs=1)
    # sourcebase
    parser.add_argument('--source-base',
                        dest='sourcebase',
                        default=None,
                        nargs=1)
    # m4base
    parser.add_argument('--m4-base',
                        dest='m4base',
                        default=None,
                        nargs=1)
    # pobase
    parser.add_argument('--po-base',
                        dest='pobase',
                        default=None,
                        nargs=1)
    # docbase
    parser.add_argument('--doc-base',
                        dest='docbase',
                        default=None,
                        nargs=1)
    # testsbase
    parser.add_argument('--tests-base',
                        dest='testsbase',
                        default=None,
                        nargs=1)
    # auxdir
    parser.add_argument('--aux-dir',
                        dest='auxdir',
                        default=None,
                        nargs=1)
    # lgpl
    parser.add_argument('--lgpl',
                        dest='lgpl',
                        default=None,
                        action='append',
                        choices=['2', '3orGPLv2', '3'],
                        nargs='?')
    # gpl
    parser.add_argument('--gpl',
                        dest='gpl',
                        default=None,
                        choices=['2', '3'],
                        nargs=1)
    # gnu-make
    parser.add_argument('--gnu-make',
                        dest='gnu_make',
                        default=None,
                        action='store_true')
    # makefile-name
    parser.add_argument('--makefile-name',
                        dest='makefile_name',
                        default=None,
                        nargs=1)
    # tests-makefile-name
    parser.add_argument('--tests-makefile-name',
                        dest='tests_makefile_name',
                        default=None,
                        nargs=1)
    # automake-subdir
    parser.add_argument('--automake-subdir',
                        dest='automake_subdir',
                        default=None,
                        action='store_true')
    # automake-subdir-tests
    parser.add_argument('--automake-subdir-tests',
                        dest='automake_subdir_tests',
                        default=None,
                        action='store_true')
    # macro-prefix
    parser.add_argument('--macro-prefix',
                        dest='macro_prefix',
                        default=None,
                        nargs=1)
    # po-domain
    parser.add_argument('--po-domain',
                        dest='podomain',
                        default=None,
                        nargs=1)
    # witness-c-macro
    parser.add_argument('--witness-c-macro',
                        dest='witness_c_macro',
                        default=None,
                        nargs=1)
    # vc-files
    parser.add_argument('--vc-files',
                        dest='vc_files',
                        default=None,
                        action='store_true')
    parser.add_argument('--no-vc-files',
                        dest='vc_files',
                        default=None,
                        action='store_false')
    # two-configures, single-configure
    parser.add_argument('--two-configures',
                        dest='single_configure',
                        default=None,
                        action='store_false')
    parser.add_argument('--single-configure',
                        dest='single_configure',
                        default=None,
                        action='store_true')
    # symlink
    parser.add_argument('-s', '-S', '--symbolic', '--symlink', '--more-symlinks',
                        dest='copymode',
                        default=None,
                        action='store_const', const=CopyAction.Symlink)
    # local-symlink
    parser.add_argument('--local-symlink',
                        dest='lcopymode',
                        default=None,
                        action='store_const', const=CopyAction.Symlink)
    # hardlink
    parser.add_argument('-h', '-H', '--hardlink', '--more-hardlinks',
                        dest='copymode',
                        default=None,
                        action='store_const', const=CopyAction.Hardlink)
    # local-hardlink
    parser.add_argument('--local-hardlink',
                        dest='lcopymode',
                        default=None,
                        action='store_const', const=CopyAction.Hardlink)
    # Undocumented option. Only used for the gnulib-tool test suite.
    parser.add_argument('--gnulib-dir',
                        dest='gnulib_dir',
                        default=None,
                        nargs=1)
    # All other arguments are collected.
    parser.add_argument('non_option_arguments',
                        nargs='*')

    # Parse the given arguments. Don't signal an error if non-option arguments
    # occur between or after options.
    cmdargs, unhandled = parser.parse_known_args()

    # Handle --gnulib-dir and finalize DIRS.
    gnulib_dir = cmdargs.gnulib_dir
    if gnulib_dir != None:
        gnulib_dir = os.path.abspath(gnulib_dir[0])
    else:
        gnulib_dir = APP['root']
    init_DIRS(gnulib_dir)

    # Handle --help and --version, ignoring all other options.
    if cmdargs.help != None:
        print(info.usage())
        sys.exit(0)
    if cmdargs.version != None:
        version = info.version()
        if version != '':
            version = ' ' + version
        message = '''gnulib-tool (%s %s)%s\n%s\n%s\n\nWritten by %s.''' \
            % (info.package(), info.date(), version, info.copyright(),
               info.license(), info.authors())
        print(message)
        sys.exit(0)

    # Report unhandled arguments.
    for arg in unhandled:
        if arg.startswith('-'):
            message = '%s: Unrecognized option \'%s\'.\n' % (APP['name'], arg)
            message += 'Try \'gnulib-tool --help\' for more information.\n'
            sys.stderr.write(message)
            sys.exit(1)
    # By now, all unhandled arguments were non-options.
    cmdargs.non_option_arguments += unhandled

    # Determine when user tries to combine modes.
    args = [
        cmdargs.mode_list,
        cmdargs.mode_find,
        cmdargs.mode_import,
        cmdargs.mode_add_import,
        cmdargs.mode_remove_import,
        cmdargs.mode_update,
        cmdargs.mode_create_testdir,
        cmdargs.mode_create_megatestdir,
        cmdargs.mode_test,
        cmdargs.mode_megatest,
        cmdargs.mode_xdescription,
        cmdargs.mode_xcomment,
        cmdargs.mode_xstatus,
        cmdargs.mode_xnotice,
        cmdargs.mode_xapplicability,
        cmdargs.mode_xusability_in_testdir,
        cmdargs.mode_xfilelist,
        cmdargs.mode_xdependencies,
        cmdargs.mode_xrecursive_dependencies,
        cmdargs.mode_xdependents,
        cmdargs.mode_xrecursive_dependents,
        cmdargs.mode_xautoconf,
        cmdargs.mode_xautomake,
        cmdargs.mode_xinclude,
        cmdargs.mode_xlink,
        cmdargs.mode_xlicense,
        cmdargs.mode_xmaintainer,
        cmdargs.mode_xtests,
        cmdargs.mode_copy_file,
    ]
    overflow = [ arg
                 for arg in args
                 if arg != None ]
    if len(overflow) > 1:
        message = '%s: Unable to combine different modes of work.\n' % APP['name']
        message += 'Try \'gnulib-tool --help\' for more information.\n'
        sys.stderr.write(message)
        sys.exit(1)

    # Determine selected mode.
    mode = None
    modules = None
    files = None
    if cmdargs.mode_list != None:
        mode = 'list'
    if cmdargs.mode_find != None:
        mode = 'find'
        files = list(cmdargs.non_option_arguments)
    if cmdargs.mode_import != None:
        mode = 'import'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_add_import != None:
        mode = 'add-import'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_remove_import != None:
        mode = 'remove-import'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_update != None:
        mode = 'update'
        if len(cmdargs.non_option_arguments) > 0:
            message = '%s: too many arguments in \'update\' mode\n'
            message += 'Try \'gnulib-tool --help\' for more information.\n'
            message += 'If you really want to modify the gnulib configuration of your project,\n'
            message += 'you need to use \'gnulib-tool --import\' - at your own risk!\n'
            sys.stderr.write(message)
            sys.exit(1)
    if cmdargs.mode_create_testdir != None:
        mode = 'create-testdir'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_create_megatestdir != None:
        mode = 'create-megatestdir'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_test != None:
        mode = 'test'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_megatest != None:
        mode = 'megatest'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xdescription != None:
        mode = 'extract-description'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xcomment != None:
        mode = 'extract-comment'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xstatus != None:
        mode = 'extract-status'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xnotice != None:
        mode = 'extract-notice'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xapplicability != None:
        mode = 'extract-applicability'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xusability_in_testdir != None:
        mode = 'extract-usability-in-testdir'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xfilelist != None:
        mode = 'extract-filelist'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xautoconf != None:
        mode = 'extract-autoconf-snippet'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xautomake != None:
        mode = 'extract-automake-snippet'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xdependencies != None:
        mode = 'extract-dependencies'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xrecursive_dependencies != None:
        mode = 'extract-recursive-dependencies'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xdependents != None:
        mode = 'extract-dependents'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xrecursive_dependents != None:
        mode = 'extract-recursive-dependents'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xinclude != None:
        mode = 'extract-include-directive'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xlink != None:
        mode = 'extract-link-directive'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xrecursive_link != None:
        mode = 'extract-recursive-link-directive'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xlicense != None:
        mode = 'extract-license'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xmaintainer != None:
        mode = 'extract-maintainer'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_xtests != None:
        mode = 'extract-tests-module'
        modules = list(cmdargs.non_option_arguments)
    if cmdargs.mode_copy_file != None:
        mode = 'copy-file'
        if len(cmdargs.non_option_arguments) < 1 or len(cmdargs.non_option_arguments) > 2:
            message = '%s: *** ' % APP['name']
            message += 'invalid number of arguments for --%s\n' % mode
            message += 'Try \'gnulib-tool --help\' for more information.\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        files = list(cmdargs.non_option_arguments)

    if ((mode in ['import', 'add-import', 'remove-import']
         and (cmdargs.excl_cxx_tests or cmdargs.excl_longrunning_tests
              or cmdargs.excl_privileged_tests or cmdargs.excl_unportable_tests
              or cmdargs.single_configure != None))
        or (mode == 'update'
            and (cmdargs.localpath != None or cmdargs.libname != None
                 or cmdargs.sourcebase != None or cmdargs.m4base != None
                 or cmdargs.pobase != None or cmdargs.docbase != None
                 or cmdargs.testsbase != None or cmdargs.auxdir != None
                 or cmdargs.inctests != None or cmdargs.obsolete != None
                 or cmdargs.inc_cxx_tests != None
                 or cmdargs.inc_longrunning_tests != None
                 or cmdargs.inc_privileged_tests != None
                 or cmdargs.inc_unportable_tests != None
                 or cmdargs.alltests != None
                 or cmdargs.excl_cxx_tests != None
                 or cmdargs.excl_longrunning_tests != None
                 or cmdargs.excl_privileged_tests != None
                 or cmdargs.excl_unportable_tests != None
                 or cmdargs.avoids != None or cmdargs.lgpl != None
                 or cmdargs.gpl != None or cmdargs.makefile_name != None
                 or cmdargs.tests_makefile_name != None
                 or cmdargs.automake_subdir != None
                 or cmdargs.automake_subdir_tests != None
                 or cmdargs.macro_prefix != None or cmdargs.podomain != None
                 or cmdargs.witness_c_macro != None or cmdargs.vc_files != None))):
        message = '%s: *** ' % APP['name']
        message += 'invalid options for --%s mode\n' % mode
        message += 'Try \'gnulib-tool --help\' for more information.\n'
        message += '%s: *** Stop.\n' % APP['name']
        sys.stderr.write(message)
        sys.exit(1)
    if cmdargs.pobase != None and cmdargs.podomain == None:
        message = '%s: *** ' % APP['name']
        message += 'together with --po-base, you need to specify --po-domain\n'
        message += 'Try \'gnulib-tool --help\' for more information.\n'
        message += '%s: *** Stop.\n' % APP['name']
        sys.stderr.write(message)
        sys.exit(1)
    if cmdargs.lgpl != None and cmdargs.gpl != None:
        message = '%s: *** ' % APP['name']
        message += 'invalid options: cannot specify both --lgpl and --gpl\n'
        message += 'Try \'gnulib-tool --help\' for more information.\n'
        message += '%s: *** Stop.\n' % APP['name']
        sys.stderr.write(message)
        sys.exit(1)
    if cmdargs.pobase == None and cmdargs.podomain != None:
        message = 'gnulib-tool: warning: --po-domain has no effect without a --po-base option\n'
        sys.stderr.write(message)
    if mode != None and 'test' in mode and cmdargs.gnu_make:
        message = '%s: --gnu-make not supported when including tests\n' % APP['name']
        sys.stderr.write(message)
        sys.exit(1)

    # Determine specific settings.
    destdir = cmdargs.destdir
    if destdir != None:
        destdir = cmdargs.destdir[0]
    localpath = cmdargs.localpath
    if localpath != None:
        localpath = [ dir
                      for list1 in localpath
                      for dir in list1 ]
    libname = cmdargs.libname
    if libname != None:
        libname = cmdargs.libname[0]
    auxdir = cmdargs.auxdir
    if auxdir != None:
        auxdir = cmdargs.auxdir[0]
    sourcebase = cmdargs.sourcebase
    if sourcebase != None:
        sourcebase = cmdargs.sourcebase[0]
    m4base = cmdargs.m4base
    if m4base != None:
        m4base = cmdargs.m4base[0]
    pobase = cmdargs.pobase
    if pobase != None:
        pobase = cmdargs.pobase[0]
    docbase = cmdargs.docbase
    if docbase != None:
        docbase = cmdargs.docbase[0]
    testsbase = cmdargs.testsbase
    if testsbase != None:
        testsbase = cmdargs.testsbase[0]
    dryrun = cmdargs.dryrun
    verbose = -cmdargs.quiet + cmdargs.verbose
    incobsolete = cmdargs.obsolete
    inctests = cmdargs.inctests
    # Canonicalize the inctests variable.
    if inctests == None:
        if mode in ['import', 'add-import', 'remove-import', 'update']:
            inctests = False
        elif mode in ['create-testdir', 'create-megatestdir', 'test', 'megatest']:
            inctests = True
    incl_test_categories = []
    if inctests:
        incl_test_categories.append(TESTS['tests'])
    if cmdargs.inc_cxx_tests:
        incl_test_categories.append(TESTS['cxx-tests'])
    if cmdargs.inc_longrunning_tests:
        incl_test_categories.append(TESTS['longrunning-tests'])
    if cmdargs.inc_privileged_tests:
        incl_test_categories.append(TESTS['privileged-tests'])
    if cmdargs.inc_unportable_tests:
        incl_test_categories.append(TESTS['unportable-tests'])
    if cmdargs.alltests:
        incl_test_categories.append(TESTS['all-tests'])
    excl_test_categories = []
    if cmdargs.excl_cxx_tests:
        excl_test_categories.append(TESTS['cxx-tests'])
    if cmdargs.excl_longrunning_tests:
        excl_test_categories.append(TESTS['longrunning-tests'])
    if cmdargs.excl_privileged_tests:
        excl_test_categories.append(TESTS['privileged-tests'])
    if cmdargs.excl_unportable_tests:
        excl_test_categories.append(TESTS['unportable-tests'])
    lgpl = cmdargs.lgpl
    if lgpl != None:
        lgpl = lgpl[-1]
        if lgpl == None:
            lgpl = True
    gpl = cmdargs.gpl
    if gpl != None:
        gpl = gpl[0]
    cond_dependencies = cmdargs.cond_dependencies
    libtool = cmdargs.libtool
    gnu_make = cmdargs.gnu_make == True
    makefile_name = cmdargs.makefile_name
    if makefile_name != None:
        makefile_name = makefile_name[0]
    tests_makefile_name = cmdargs.tests_makefile_name
    if tests_makefile_name != None:
        tests_makefile_name = tests_makefile_name[0]
    automake_subdir = cmdargs.automake_subdir == True
    automake_subdir_tests = cmdargs.automake_subdir_tests == True
    macro_prefix = cmdargs.macro_prefix
    if macro_prefix != None:
        macro_prefix = macro_prefix[0]
    podomain = cmdargs.podomain
    if podomain != None:
        podomain = podomain[0]
    witness_c_macro = cmdargs.witness_c_macro
    if witness_c_macro != None:
        witness_c_macro = witness_c_macro[0]
    vc_files = cmdargs.vc_files
    avoids = cmdargs.avoids
    if avoids != None:
        avoids = [ module
                   for list1 in avoids
                   for module in list1 ]
    copymode = cmdargs.copymode
    lcopymode = cmdargs.lcopymode
    single_configure = cmdargs.single_configure
    # Canonicalize the single_configure variable.
    if single_configure == None:
        single_configure = True

    # Create pygnulib configuration.
    config = GLConfig(
        tempdir=temp_directory,
        destdir=destdir,
        localpath=localpath,
        m4base=m4base,
        auxdir=auxdir,
        modules=modules,
        avoids=avoids,
        sourcebase=sourcebase,
        pobase=pobase,
        docbase=docbase,
        testsbase=testsbase,
        incobsolete=incobsolete,
        incl_test_categories=incl_test_categories,
        excl_test_categories=excl_test_categories,
        libname=libname,
        lgpl=lgpl,
        gpl=gpl,
        gnu_make=gnu_make,
        makefile_name=makefile_name,
        tests_makefile_name=tests_makefile_name,
        automake_subdir=automake_subdir,
        automake_subdir_tests=automake_subdir_tests,
        libtool=libtool,
        conddeps=cond_dependencies,
        macro_prefix=macro_prefix,
        podomain=podomain,
        witness_c_macro=witness_c_macro,
        vc_files=vc_files,
        copymode=copymode,
        lcopymode=lcopymode,
        single_configure=single_configure,
        verbose=verbose,
        dryrun=dryrun,
    )

    # Work in the given mode.
    if mode == 'list':
        modulesystem = GLModuleSystem(config)
        listing = modulesystem.list()
        result = lines_to_multiline(listing)
        print(result, end='')

    elif mode == 'find':
        modulesystem = GLModuleSystem(config)
        for filename in files:
            if (os.path.isfile(joinpath(DIRS['root'], filename))
                    or (localpath != None
                        and any([ os.path.isfile(joinpath(localdir, filename))
                                  for localdir in localpath ]))):
                # Convert the file name to a POSIX basic regex.
                # Needs to handle . [ \ * ^ $.
                filename_regex = filename.replace('\\', '\\\\').replace('[', '\\[').replace('^', '\\^')
                filename_regex = re.compile(r'([.*$])').sub(r'[\1]', filename_regex)
                filename_line_regex = '^' + filename_regex + '$'
                # Read module candidates from gnulib root directory.
                command = "find modules -type f -print | xargs -n 100 grep -l %s /dev/null | sed -e 's,^modules/,,'" % shlex.quote(filename_line_regex)
                with sp.Popen(command, shell=True, cwd=DIRS['root'], stdout=sp.PIPE) as proc:
                    result = proc.stdout.read().decode('UTF-8')
                # Read module candidates from local directories.
                if localpath != None and len(localpath) > 0:
                    command = "find modules -type f -print | xargs -n 100 grep -l %s /dev/null | sed -e 's,^modules/,,' -e 's,\\.diff$,,'" % shlex.quote(filename_line_regex)
                    for localdir in localpath:
                        with sp.Popen(command, shell=True, cwd=localdir, stdout=sp.PIPE) as proc:
                            result += proc.stdout.read().decode('UTF-8')
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
                            if modulesystem.file_is_module(line) ]
                candidates = sorted(set(listing))
                for name in candidates:
                    module = modulesystem.find(name)
                    if module:  # Ignore module candidates that don't actually exist.
                        if module.getFiles():
                            print(name)
            else:
                message = 'gnulib-tool: warning: file %s does not exist\n' % filename
                sys.stderr.write(message)

    elif mode in ['import', 'add-import', 'remove-import', 'update']:
        mode = MODES[mode]
        if not destdir:
            destdir = '.'
        config.setDestDir(destdir)

        # Prefer configure.ac but also look for configure.in.
        # NOTE: Use os.path.join so the leading './' is not removed. This
        # is to make the gnulib-tool test suite happy.
        if os.path.isfile(os.path.join(destdir, 'configure.ac')):
            configure_ac = os.path.join(destdir, 'configure.ac')
        elif os.path.isfile(os.path.join(destdir, 'configure.in')):
            configure_ac = os.path.join(destdir, 'configure.in')
        else:
            raise GLError(3, os.path.join(destdir, 'configure.ac'))

        # Save the Autoconf file path for the rest of the import.
        config.setAutoconfFile(configure_ac)

        # Analyze configure.ac.
        with open(configure_ac, mode='r', newline='\n', encoding='utf-8') as file:
            configure_ac_data = file.read()

        guessed_m4dirs = []
        pattern = re.compile(r'^.*AC_CONFIG_MACRO_DIRS?\([\[ ]*([^]"$`\\)]*).*$', re.MULTILINE)
        match = pattern.findall(configure_ac_data)
        # Append the match to guessed_m4dirs.
        if match:
            guessed_m4dirs += match

        if mode == MODES['import']:
            # Set variables to default values.
            if not sourcebase:
                sourcebase = 'lib'
            if not m4base:
                m4base = 'm4'
            if not docbase:
                docbase = 'doc'
            if not testsbase:
                testsbase = 'tests'
            if not macro_prefix:
                macro_prefix = 'gl'
            config.setSourceBase(sourcebase)
            config.setM4Base(m4base)
            config.setDocBase(docbase)
            config.setTestsBase(testsbase)
            config.setMacroPrefix(macro_prefix)

            # Perform GLImport actions.
            importer = GLImport(config, mode, guessed_m4dirs)
            filetable, transformers = importer.prepare()
            importer.execute(filetable, transformers)

        else:  # if mode != MODE['--import']
            if m4base:
                # Apply func_import to a particular gnulib directory.
                # Any number of additional modules can be given.
                if not os.path.isfile(joinpath(destdir, m4base, 'gnulib-cache.m4')):
                    # First use of gnulib in the given m4base.
                    if not sourcebase:
                        sourcebase = 'lib'
                    if not docbase:
                        docbase = 'doc'
                    if not testsbase:
                        testsbase = 'tests'
                    if not macro_prefix:
                        macro_prefix = 'gl'
                    config.setSourceBase(sourcebase)
                    config.setM4Base(m4base)
                    config.setDocBase(docbase)
                    config.setTestsBase(testsbase)
                    config.setMacroPrefix(macro_prefix)
                # Perform GLImport actions.
                importer = GLImport(config, mode, guessed_m4dirs)
                filetable, transformers = importer.prepare()
                importer.execute(filetable, transformers)
            else:  # if not m4base
                # Apply func_import to all gnulib directories.
                # To get this list of directories, look at Makefile.am. (Not at
                # configure, because it may be omitted from version control. Also,
                # don't run "find $destdir -name gnulib-cache.m4", as it might be
                # too expensive.)
                m4dirs = []
                dirisnext = False
                filepath = joinpath(destdir, 'Makefile.am')
                if os.path.isfile(filepath):
                    with open(filepath, mode='r', newline='\n', encoding='utf-8') as file:
                        data = file.read()
                    pattern = re.compile(r'^ACLOCAL_AMFLAGS[\t ]*=[\t ]*(.+?)[\t ]*(?:#|$)', re.MULTILINE)
                    match = re.search(pattern, data)
                    if match:
                        aclocal_amflags = match.group(1).split()
                    else:
                        aclocal_amflags = []
                    for aclocal_amflag in aclocal_amflags:
                        if dirisnext:
                            # Ignore absolute directory pathnames, like /usr/local/share/aclocal.
                            if not os.path.isabs(aclocal_amflag):
                                if os.path.isfile(joinpath(destdir, aclocal_amflag, 'gnulib-cache.m4')):
                                    m4dirs.append(aclocal_amflag)
                            dirisnext = False
                        else:  # if not dirisnext
                            if aclocal_amflag == '-I':
                                dirisnext = True
                            else:  # if aclocal_amflag != '-I'
                                dirisnext = False
                    for arg in guessed_m4dirs:
                        # Ignore absolute directory pathnames, like /usr/local/share/aclocal.
                        if not os.path.isabs(arg):
                            if os.path.isfile(joinpath(destdir, arg, 'gnulib-cache.m4')):
                                m4dirs.append(arg)
                else:  # if not os.path.isfile(filepath)
                    # No Makefile.am! Oh well. Look at the last generated aclocal.m4.
                    filepath = joinpath(destdir, 'aclocal.m4')
                    if os.path.isfile(filepath):
                        pattern = re.compile(r'm4_include\(\[(.*?)]\)')
                        with open(filepath, mode='r', newline='\n', encoding='utf-8') as file:
                            m4dirs = pattern.findall(file.read())
                        m4dirs = [ os.path.dirname(m4dir)
                                   for m4dir in m4dirs ]
                        m4dirs = [ m4dir
                                   for m4dir in m4dirs
                                   if os.path.isfile(joinpath(destdir, m4dir, 'gnulib-cache.m4')) ]
                        m4dirs = sorted(set(m4dirs))
                if len(m4dirs) == 0:
                    # First use of gnulib in a package.
                    # Any number of additional modules can be given.
                    if not sourcebase:
                        sourcebase = 'lib'
                    m4base = 'm4'
                    if not docbase:
                        docbase = 'doc'
                    if not testsbase:
                        testsbase = 'tests'
                    if not macro_prefix:
                        macro_prefix = 'gl'
                    config.setSourceBase(sourcebase)
                    config.setM4Base(m4base)
                    config.setDocBase(docbase)
                    config.setTestsBase(testsbase)
                    config.setMacroPrefix(macro_prefix)
                    # Perform GLImport actions.
                    importer = GLImport(config, mode, guessed_m4dirs)
                    filetable, transformers = importer.prepare()
                    importer.execute(filetable, transformers)
                elif len(m4dirs) == 1:
                    # There's only one use of gnulib here. Assume the user means it.
                    # Any number of additional modules can be given.
                    m4base = m4dirs[-1]
                    config.setM4Base(m4base)
                    # Perform GLImport actions.
                    importer = GLImport(config, mode, guessed_m4dirs)
                    filetable, transformers = importer.prepare()
                    importer.execute(filetable, transformers)
                else:  # if len(m4dirs) > 1
                    # No further arguments. Guess the user wants to update all of them.
                    for m4base in m4dirs:
                        config.setM4Base(m4base)
                        # Perform GLImport actions.
                        importer = GLImport(config, mode, guessed_m4dirs)
                        filetable, transformers = importer.prepare()
                        importer.execute(filetable, transformers)

    elif mode == 'create-testdir':
        if not destdir:
            message = '%s: *** ' % APP['name']
            message += 'please specify --dir option\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        if not auxdir:
            auxdir = 'build-aux'
        config.setAuxDir(auxdir)
        testdir = GLTestDir(config, destdir)
        testdir.execute()

    elif mode == 'create-megatestdir':
        if not destdir:
            message = '%s: *** ' % APP['name']
            message += 'please specify --dir option\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        if not auxdir:
            auxdir = 'build-aux'
        config.setAuxDir(auxdir)
        testdir = GLMegaTestDir(config, destdir)
        testdir.execute()

    elif mode == 'test':
        if not destdir:
            destdir = 'testdir%04d' % random.randrange(0, 9999)
        if not auxdir:
            auxdir = 'build-aux'
        config.setAuxDir(auxdir)
        testdir = GLTestDir(config, destdir)
        testdir.execute()
        force_output()
        os.chdir(destdir)
        os.mkdir('build')
        os.chdir('build')
        try:  # Try to execute commands
            sp.run(['../configure'], check=True)
            sp.run([UTILS['make']], check=True)
            sp.run([UTILS['make'], 'check'], check=True)
            sp.run([UTILS['make'], 'distclean'], check=True)
        except Exception:
            sys.exit(1)
        args = ['find', '.', '-type', 'f', '-print']
        remaining = sp.check_output(args).decode(ENCS['shell'])
        lines = [ line.strip()
                  for line in remaining.split('\n')
                  if line.strip() ]
        remaining = ' '.join(lines)
        if remaining:
            message = 'Remaining files: %s\n' % remaining
            message += 'gnulib-tool: *** Stop.\n'
            sys.stderr.write(message)
            sys.exit(1)
        os.chdir('../..')
        rmtree(destdir)

    elif mode == 'megatest':
        if not destdir:
            destdir = 'testdir%04d' % random.randrange(0, 9999)
        if not auxdir:
            auxdir = 'build-aux'
        config.setAuxDir(auxdir)
        testdir = GLMegaTestDir(config, destdir)
        testdir.execute()
        force_output()
        os.chdir(destdir)
        os.mkdir('build')
        os.chdir('build')
        try:  # Try to execute commands
            sp.run(['../configure'], check=True)
            sp.run([UTILS['make']], check=True)
            sp.run([UTILS['make'], 'check'], check=True)
            sp.run([UTILS['make'], 'distclean'], check=True)
        except Exception:
            sys.exit(1)
        args = ['find', '.', '-type', 'f', '-print']
        remaining = sp.check_output(args).decode(ENCS['shell'])
        lines = [ line.strip()
                  for line in remaining.split('\n')
                  if line.strip() ]
        remaining = ' '.join(lines)
        if remaining:
            message = 'Remaining files: %s\n' % remaining
            message += 'gnulib-tool: *** Stop.\n'
            sys.stderr.write(message)
            sys.exit(1)
        os.chdir('../..')
        rmtree(destdir)

    elif mode == 'extract-description':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getDescription())

    elif mode == 'extract-comment':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getComment())

    elif mode == 'extract-status':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getStatus())

    elif mode == 'extract-notice':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getNotice())

    elif mode == 'extract-applicability':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                print(module.getApplicability())

    elif mode == 'extract-usability-in-testdir':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                print(module.getUsabilityInTestdir())

    elif mode == 'extract-filelist':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                files = module.getFiles_Raw()
                files += 'm4/00gnulib.m4\n'
                files += 'm4/zzgnulib.m4\n'
                files += 'm4/gnulib-common.m4\n'
                print(files, end='')

    elif mode == 'extract-dependencies':
        if avoids:
            message = '%s: *** ' % APP['name']
            message += 'cannot combine --avoid and --extract-dependencies\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getDependencies())

    elif mode == 'extract-recursive-dependencies':
        if avoids:
            message = '%s: *** ' % APP['name']
            message += 'cannot combine --avoid and --extract-recursive-dependencies\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                dependencies = module.getDependenciesRecursively()
                dependencies_names = sorted([ m.name
                                              for m in dependencies ])
                sys.stdout.write(lines_to_multiline(dependencies_names))

    elif mode == 'extract-dependents':
        if avoids:
            message = '%s: *** ' % APP['name']
            message += 'cannot combine --avoid and --extract-dependents\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                dependents = module.getDependents()
                dependents_names = sorted([ m.name
                                            for m in dependents ])
                sys.stdout.write(lines_to_multiline(dependents_names))

    elif mode == 'extract-recursive-dependents':
        if avoids:
            message = '%s: *** ' % APP['name']
            message += 'cannot combine --avoid and --extract-recursive-dependents\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                dependents = module.getDependentsRecursively()
                dependents_names = sorted([ m.name
                                            for m in dependents ])
                sys.stdout.write(lines_to_multiline(dependents_names))

    elif mode == 'extract-autoconf-snippet':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getAutoconfSnippet())

    elif mode == 'extract-automake-snippet':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getAutomakeSnippet())

    elif mode == 'extract-include-directive':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getInclude())

    elif mode == 'extract-link-directive':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getLink())

    elif mode == 'extract-recursive-link-directive':
        if avoids:
            message = '%s: *** ' % APP['name']
            message += 'cannot combine --avoid and --extract-recursive-link-directive\n'
            message += '%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getLinkDirectiveRecursively())

    elif mode == 'extract-license':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                print(module.getLicense())

    elif mode == 'extract-maintainer':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module:
                sys.stdout.write(module.getMaintainer())

    elif mode == 'extract-tests-module':
        modulesystem = GLModuleSystem(config)
        for name in modules:
            module = modulesystem.find(name)
            if module and modulesystem.exists(module.getTestsName()):
                print(module.getTestsName())

    elif mode == 'copy-file':
        srcpath = files[0]
        # The second argument is the destination; either a directory or a file.
        # It defaults to the current directory.
        if len(files) == 2:
            dest = files[1]
        else:  # if len(files) < 2
            dest = '.'
        if not auxdir:
            auxdir = 'build-aux'
        if not sourcebase:
            sourcebase = 'lib'
        if not m4base:
            m4base = 'm4'
        if not docbase:
            docbase = 'doc'
        if not testsbase:
            testsbase = 'tests'
        config.setAuxDir(auxdir)
        config.setSourceBase(sourcebase)
        config.setM4Base(m4base)
        config.setDocBase(docbase)
        config.setTestsBase(testsbase)
        filesystem = GLFileSystem(config)
        lookedup, flag = filesystem.lookup(srcpath)
        if os.path.isdir(dest):
            destdir = dest
            destpath = rewrite_file_name(srcpath, config)
        else:  # if not os.path.isdir(dest)
            destdir = os.path.dirname(dest)
            destpath = os.path.basename(dest)
        # Create the directory for destfile.
        dirname = os.path.dirname(joinpath(destdir, destpath))
        if not config['dryrun']:
            if dirname and not os.path.isdir(dirname):
                try:  # Try to create directories
                    os.makedirs(dirname)
                except FileExistsError:
                    pass
        # Copy the file.
        config.setDestDir(destdir)
        assistant = GLFileAssistant(config)
        tmpfile = assistant.tmpfilename(destpath)
        copyfile(lookedup, tmpfile)
        ensure_writable(tmpfile)
        assistant.setOriginal(srcpath)
        assistant.setRewritten(destpath)
        if os.path.isfile(joinpath(destdir, destpath)):
            # The file already exists.
            assistant.update(lookedup, flag, tmpfile, True)
        else:  # if not os.path.isfile(joinpath(destdir, destpath))
            # Install the file.
            # Don't protest if the file should be there but isn't: it happens
            # frequently that developers don't put autogenerated files under
            # version control.
            assistant.add(lookedup, flag, tmpfile)
        if os.path.isfile(tmpfile):
            os.remove(tmpfile)

    else:
        message = '%s: *** ' % APP['name']
        message += 'no mode specified\n'
        message += '%s: *** Stop.\n' % APP['name']
        sys.stderr.write(message)
        sys.exit(1)

    if copymode == CopyAction.Hardlink or lcopymode == CopyAction.Hardlink:
        # Setting hard links modifies the ctime of files in the gnulib checkout.
        # This disturbs the result of the next "gitk" invocation.
        # Workaround: Let git scan the files. This can be done through
        # "git update-index --refresh" or "git status" or "git diff".
        if os.path.isdir(joinpath(DIRS['root'], '.git')):
            try:
                sp.run(['git', 'update-index', '--refresh'],
                       cwd=DIRS['root'], stdout=sp.DEVNULL)
            except FileNotFoundError:
                # No 'git' program was found.
                pass


def cli_exception(exc_type, exc_value, exc_traceback) -> None:
    '''Exception hook that does not print a traceback for KeyboardInterrupts
    thrown when Ctrl-C is pressed.'''
    if not issubclass(exc_type, KeyboardInterrupt):
        sys.__excepthook__(exc_type, exc_value, exc_traceback)


def main_with_exception_handling() -> None:
    # Don't print tracebacks for KeyboardInterrupts when stdin is a tty.
    if sys.stdin and sys.stdin.isatty():
        sys.excepthook = cli_exception

    try:  # Try to execute
        with tempfile.TemporaryDirectory(prefix='glpy') as temporary_directory:
            main(temporary_directory)
    except KeyboardInterrupt:
        sys.stderr.write('%s: *** Stop.\n' % APP['name'])
        sys.exit(1)
    except GLError as error:
        errmode = 0  # gnulib-style errors
        errno = error.errno
        errinfo = error.errinfo
        if errmode == 0:
            message = '%s: *** ' % APP['name']
            if errinfo == None:
                errinfo = ''
            if errno == 1:
                message += 'file %s not found' % errinfo
            elif errno == 2:
                message += 'patch file %s didn\'t apply cleanly' % errinfo
            elif errno == 3:
                message += 'cannot find %s - make sure you run gnulib-tool from within your package\'s directory' % errinfo
            elif errno == 4:
                message += 'minimum supported autoconf version is 2.64. Try adding'
                message += ' AC_PREREQ([%s])' % DEFAULT_AUTOCONF_MINVERSION
                message += ' to your configure.ac.'
            elif errno == 5:
                message += '%s is expected to contain gl_M4_BASE([%s])' % (repr(os.path.join(errinfo, 'gnulib-comp.m4')), repr(errinfo))
            elif errno == 6:
                message += 'missing --source-base option'
            elif errno == 7:
                message += 'missing --doc-base option. --doc-base has been introduced '
                message += 'on 2006-07-11; if your last invocation of \'gnulib-tool '
                message += '--import\' is before that date, you need to run'
                message += '\'gnulib-tool --import\' once, with a --doc-base option.'
            elif errno == 8:
                message += 'missing --tests-base option'
            elif errno == 9:
                message += 'missing --lib option'
            elif errno == 11:
                incompatibilities = ''
                message += 'incompatible license on modules:\n'
                for pair in errinfo:
                    incompatibilities += pair[0]
                    incompatibilities += ' %s' % pair[1]
                    incompatibilities += '\n'
                tempname = tempfile.mktemp()
                with open(tempname, mode='w', newline='\n', encoding='utf-8') as file:
                    file.write(incompatibilities)
                sed_table = 's,^\\([^ ]*\\) ,\\1' + ' ' * 51 + ',\n'
                sed_table += 's,^\\(' + '.' * 49 + '[^ ]*\\) *,' + ' ' * 17 + '\\1 ,'
                args = ['sed', '-e', sed_table, tempname]
                incompatibilities = sp.check_output(args).decode(ENCS['default'])
                message += incompatibilities
                os.remove(tempname)
            elif errno == 12:
                message += 'refusing to do nothing'
            elif errno == 13:
                message += 'could not create directory %s' % errinfo
            elif errno == 14:
                message += 'could not delete file %s' % errinfo
            elif errno == 15:
                message += 'could not create file %s' % errinfo
            elif errno == 16:
                message += 'could not transform file %s' % errinfo
            elif errno == 17:
                message += 'could not update file %s' % errinfo
            elif errno == 18:
                message += 'module %s lacks a license' % errinfo
            elif errno == 19:
                message += 'could not create destination directory: %s' % errinfo
            elif errno == 20:
                message += 'could not patch test-driver script'
            elif errno == 21:
                message += ('Option --automake-subdir/--automake-subdir-tests are only '
                            'supported if the definition of AUTOMAKE_OPTIONS in '
                            'Makefile.am contains \'subdir-objects\'.')
            elif errno == 22:
                message += 'not overwriting destination directory: %s' % errinfo
            elif errno == 23:
                message += "module %s doesn't exist" % errinfo
            elif errno == 24:
                message += 'module %s cannot be used in a testdir' % errinfo
            message += '\n%s: *** Stop.\n' % APP['name']
            sys.stderr.write(message)
            sys.exit(1)
