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
import codecs
import subprocess as sp
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
DIRS = constants.DIRS
joinpath = constants.joinpath
isdir = os.path.isdir


#===============================================================================
# Define GLInfo class
#===============================================================================
class GLInfo(object):
    '''This class is used to get fromatted information about gnulib-tool.
    This information is mainly used in stdout messages, but can be used
    anywhere else. The return values are not the same as for the module,
    but still depends on them.'''

    def __repr__(self):
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLInfo %s>' % hex(id(self))
        return result

    def package(self):
        '''Return formatted string which contains name of the package.'''
        result = 'GNU gnulib'
        return result

    def authors(self):
        '''Return formatted string which contains authors.
        The special __author__ variable is used (type is list).'''
        result = ''
        for item in __author__:
            if item == __author__[-1]:
                result += 'and %s' % item
            else:
                result += '%s, ' % item
        return result

    def license(self):
        '''Return formatted string which contains license and its description.'''
        result = 'License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n'
        result += 'This is free software: you are free to change and redistribute it.\n'
        result += 'There is NO WARRANTY, to the extent permitted by law.'
        return result

    def copyright(self):
        '''Return formatted string which contains copyright.
        The special __copyright__ variable is used (type is str).'''
        copyright = __copyright__
        # Per the GNU Coding Standards, show only the last year.
        copyright = re.compile('^[0-9]*-').sub('', copyright)
        result = 'Copyright (C) %s' % copyright
        return result

    def date(self):
        '''Return formatted string which contains date and time in GMT format.'''
        if isdir(DIRS['git']):
            have_git = None
            try:
                sp.check_call(['git', '--version'], stdout=sp.DEVNULL)
                have_git = True
            except:
                have_git = False
            if have_git:
                have_GNU_date = None
                try:
                    sp.check_call(['date', '--version'], stdout=sp.DEVNULL)
                    have_GNU_date = True
                except:
                    have_GNU_date = False
                if have_GNU_date:
                    args = ['git', 'log', '-n', '1', 'ChangeLog']
                    result = sp.check_output(args, cwd=DIRS['root']).decode("UTF-8")
                    # Get date as "Fri Mar 21 07:16:51 2008 -0600" from string
                    pattern = re.compile('^Date:[\t ]*(.*?)$', re.M)
                    result = pattern.findall(result)[0]
                    # Turn "Fri Mar 21 07:16:51 2008 -0600" into "Mar 21 2008 07:16:51 -0600"
                    pattern = re.compile('^[^ ]* ([^ ]*) ([0-9]*) ([0-9:]*) ([0-9]*) ')
                    result = pattern.sub('\\1 \\2 \\4 \\3 ', result)
                    # Use GNU date to compute the time in GMT
                    args = ['date', '-d', result, '-u', '+%Y-%m-%d %H:%M:%S']
                    proc = sp.check_output(args)
                    result = str(proc, "UTF-8")
                    result = result.rstrip(os.linesep)
                    return result
        # gnulib copy without versioning information.
        first_changelog_line = None
        with codecs.open(os.path.join(DIRS['root'], 'ChangeLog'), 'rb', 'UTF-8') as file:
            line = file.readline()
            first_changelog_line = line.rstrip()
        result = re.compile(' .*').sub('', first_changelog_line)
        return result

    def usage(self):
        '''Show help message.'''
        result = '''\
Usage: gnulib-tool --list
       gnulib-tool --find filename
       gnulib-tool --import [module1 ... moduleN]
       gnulib-tool --add-import [module1 ... moduleN]
       gnulib-tool --remove-import [module1 ... moduleN]
       gnulib-tool --update
       gnulib-tool --create-testdir --dir=directory [module1 ... moduleN]
       gnulib-tool --create-megatestdir --dir=directory [module1 ... moduleN]
       gnulib-tool --test --dir=directory [module1 ... moduleN]
       gnulib-tool --megatest --dir=directory [module1 ... moduleN]
       gnulib-tool --extract-description module
       gnulib-tool --extract-comment module
       gnulib-tool --extract-status module
       gnulib-tool --extract-notice module
       gnulib-tool --extract-applicability module
       gnulib-tool --extract-filelist module
       gnulib-tool --extract-dependencies module
       gnulib-tool --extract-autoconf-snippet module
       gnulib-tool --extract-automake-snippet module
       gnulib-tool --extract-include-directive module
       gnulib-tool --extract-link-directive module
       gnulib-tool --extract-license module
       gnulib-tool --extract-maintainer module
       gnulib-tool --extract-tests-module module
       gnulib-tool --copy-file file [destination]

Operation modes:

      --list                print the available module names
      --find                find the modules which contain the specified file
      --import              import the given modules into the current package
      --add-import          augment the list of imports from gnulib into the
                            current package, by adding the given modules;
                            if no modules are specified, update the current
                            package from the current gnulib
      --remove-import       reduce the list of imports from gnulib into the
                            current package, by removing the given modules
      --update              update the current package, restore files omitted
                            from version control
      --create-testdir      create a scratch package with the given modules
      --create-megatestdir  create a mega scratch package with the given modules
                            one by one and all together
      --test                test the combination of the given modules
                            (recommended to use CC=\"gcc -Wall\" here)
      --megatest            test the given modules one by one and all together
                            (recommended to use CC=\"gcc -Wall\" here)
      --extract-description        extract the description
      --extract-comment            extract the comment
      --extract-status             extract the status (obsolete etc.)
      --extract-notice             extract the notice or banner
      --extract-applicability      extract the applicability
      --extract-filelist           extract the list of files
      --extract-dependencies       extract the dependencies
      --extract-autoconf-snippet   extract the snippet for configure.ac
      --extract-automake-snippet   extract the snippet for library makefile
      --extract-include-directive  extract the #include directive
      --extract-link-directive     extract the linker directive
      --extract-license            report the license terms of the source files
                                   under lib/
      --extract-maintainer         report the maintainer(s) inside gnulib
      --extract-tests-module       report the unit test module, if it exists
      --copy-file                  copy a file that is not part of any module
      --help                Show this help text.
      --version             Show version and authorship information.

General options:

      --dir=DIRECTORY       Specify the target directory.
                            For --import, this specifies where your
                            configure.ac can be found.  Defaults to current
                            directory.
      --local-dir=DIRECTORY  Specify a local override directory where to look
                            up files before looking in gnulib's directory.
      --verbose             Increase verbosity. May be repeated.
      --quiet               Decrease verbosity. May be repeated.

Options for --import, --add/remove-import, --update:

      --dry-run             Only print what would have been done.

Options for --import, --add/remove-import:

      --with-tests          Include unit tests for the included modules.

Options for --create-[mega]testdir, --[mega]test:

      --without-tests       Don't include unit tests for the included modules.

Options for --import, --add/remove-import,
            --create-[mega]testdir, --[mega]test:

      --with-obsolete       Include obsolete modules when they occur among the
                            dependencies. By default, dependencies to obsolete
                            modules are ignored.
      --with-c++-tests      Include even unit tests for C++ interoperability.
      --without-c++-tests   Exclude unit tests for C++ interoperability.
      --with-longrunning-tests
                            Include even unit tests that are long-runners.
      --without-longrunning-tests
                            Exclude unit tests that are long-runners.
      --with-privileged-tests
                            Include even unit tests that require root
                            privileges.
      --without-privileged-tests
                            Exclude unit tests that require root privileges.
      --with-unportable-tests
                            Include even unit tests that fail on some platforms.
      --without-unportable-tests
                            Exclude unit tests that fail on some platforms.
      --with-all-tests      Include all kinds of problematic unit tests.
      --avoid=MODULE        Avoid including the given MODULE. Useful if you
                            have code that provides equivalent functionality.
                            This option can be repeated.
      --conditional-dependencies
                            Support conditional dependencies (may save configure
                            time and object code).
      --no-conditional-dependencies
                            Don't use conditional dependencies.
      --libtool             Use libtool rules.
      --no-libtool          Don't use libtool rules.

Options for --import, --add/remove-import:

      --lib=LIBRARY         Specify the library name.  Defaults to 'libgnu'.
      --source-base=DIRECTORY
                            Directory relative to --dir where source code is
                            placed (default \"lib\").
      --m4-base=DIRECTORY   Directory relative to --dir where *.m4 macros are
                            placed (default \"m4\").
      --po-base=DIRECTORY   Directory relative to --dir where *.po files are
                            placed (default \"po\").
      --doc-base=DIRECTORY  Directory relative to --dir where doc files are
                            placed (default \"doc\").
      --tests-base=DIRECTORY
                            Directory relative to --dir where unit tests are
                            placed (default \"tests\").
      --aux-dir=DIRECTORY   Directory relative to --dir where auxiliary build
                            tools are placed (default comes from configure.ac).
      --lgpl[=2|=3orGPLv2|=3]
                            Abort if modules aren't available under the LGPL.
                            Also modify license template from GPL to LGPL.
                            The version number of the LGPL can be specified;
                            the default is currently LGPLv3.
      --makefile-name=NAME  Name of makefile in automake syntax in the
                            source-base and tests-base directories
                            (default \"Makefile.am\").
      --macro-prefix=PREFIX  Specify the prefix of the macros 'gl_EARLY' and
                            'gl_INIT'. Default is 'gl'.
      --po-domain=NAME      Specify the prefix of the i18n domain. Usually use
                            the package name. A suffix '-gnulib' is appended.
      --witness-c-macro=NAME  Specify the C macro that is defined when the
                            sources in this directory are compiled or used.
      --vc-files            Update version control related files.
      --no-vc-files         Don't update version control related files
                            (.gitignore and/or .cvsignore).

Options for --create-[mega]testdir, --[mega]test:

      --single-configure    Generate a single configure file, not a separate
                            configure file for the tests directory.

Options for --import, --add/remove-import, --update,
            --create-[mega]testdir, --[mega]test:

  -s, --symbolic, --symlink Make symbolic links instead of copying files.
      --local-symlink       Make symbolic links instead of copying files, only
                            for files from the local override directory.

Options for --import, --add/remove-import, --update:

  -S, --more-symlinks       Deprecated; equivalent to --symlink.

Report bugs to <bug-gnulib@gnu.org>.'''
        return result

    def version(self):
        '''Return formatted string which contains git version.'''
        if isdir(DIRS['git']):
            have_git = None
            try:
                sp.check_call(['git', '--version'], stdout=sp.DEVNULL)
                have_git = True
            except:
                have_git = False
            if have_git:
                version_gen = joinpath(DIRS['build-aux'], 'git-version-gen')
                args = [version_gen, '/dev/null']
                result = sp.check_output(args, cwd=DIRS['root']).decode("UTF-8")
                result = result.strip()
                result = result.replace('-dirty', '-modified')
                if result == 'UNKNOWN':
                    result = ''
                return result
        # gnulib copy without versioning information.
        return ''
