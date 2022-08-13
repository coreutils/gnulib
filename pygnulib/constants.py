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

'''An easy access to pygnulib constants.'''

from __future__ import unicode_literals
#===============================================================================
# Define global imports
#===============================================================================
import re
import os
import sys
import platform
import shutil
import tempfile
import codecs
import subprocess as sp
import __main__ as interpreter


#===============================================================================
# Define module information
#===============================================================================
__all__ = list()
__author__ = \
    [
        'Bruno Haible',
        'Paul Eggert',
        'Simon Josefsson',
        'Dmitriy Selyutin',
    ]
__license__ = 'GNU GPLv3+'
__copyright__ = '2002-2022 Free Software Foundation, Inc.'


#===============================================================================
# Define global constants
#===============================================================================
# Declare necessary variables
APP = dict()  # Application
DIRS = dict()  # Directories
UTILS = dict()  # Utilities
ENCS = dict()  # Encodings
MODES = dict()  # Modes
TESTS = dict()  # Tests
NL = '''
'''  # Newline character

# Set ENCS dictionary
if not hasattr(interpreter, '__file__'):
    if sys.stdout.encoding != None:
        ENCS['default'] = sys.stdout.encoding
    else:  # sys.stdout.encoding == None
        ENCS['default'] = 'UTF-8'
else:  # if hasattr(interpreter, '__file__'):
    ENCS['default'] = 'UTF-8'
ENCS['system'] = sys.getfilesystemencoding()
ENCS['shell'] = sys.stdout.encoding
if ENCS['shell'] == None:
    ENCS['shell'] = 'UTF-8'

# Set APP dictionary
APP['name'] = sys.argv[0]
if not APP['name']:
    APP['name'] = 'gnulib-tool.py'
APP['path'] = os.path.realpath(sys.argv[0])

# Set DIRS dictionary
DIRS['root'] = os.path.dirname(APP['path'])
DIRS['cwd'] = os.getcwd()
DIRS['build-aux'] = os.path.join(DIRS['root'], 'build-aux')
DIRS['config'] = os.path.join(DIRS['root'], 'config')
DIRS['doc'] = os.path.join(DIRS['root'], 'doc')
DIRS['lib'] = os.path.join(DIRS['root'], 'lib')
DIRS['m4'] = os.path.join(DIRS['root'], 'm4')
DIRS['modules'] = os.path.join(DIRS['root'], 'modules')
DIRS['tests'] = os.path.join(DIRS['root'], 'tests')
DIRS['git'] = os.path.join(DIRS['root'], '.git')
DIRS['cvs'] = os.path.join(DIRS['root'], 'CVS')

# Set MODES dictionary
MODES = \
    {
        'import': 0,
        'add-import': 1,
        'remove-import': 2,
        'update': 3,
    }
MODES['verbose-min'] = -2
MODES['verbose-default'] = 0
MODES['verbose-max'] = 2

# Define TESTS categories
TESTS = \
    {
        'tests':             0,
        'obsolete':          1,
        'c++-test':          2,
        'cxx-test':          2,
        'c++-tests':         2,
        'cxx-tests':         2,
        'longrunning-test':  3,
        'longrunning-tests': 3,
        'privileged-test':   4,
        'privileged-tests':  4,
        'unportable-test':   5,
        'unportable-tests':  5,
        'all-test':          6,
        'all-tests':         6,
    }

# Define AUTOCONF minimum version
DEFAULT_AUTOCONF_MINVERSION = 2.59
# You can set AUTOCONFPATH to empty if autoconf 2.57 is already in your PATH
AUTOCONFPATH = ''
# You can set AUTOMAKEPATH to empty if automake 1.9.x is already in your PATH
AUTOMAKEPATH = ''
# You can set GETTEXTPATH to empty if autopoint 0.15 is already in your PATH
GETTEXTPATH = ''
# You can set LIBTOOLPATH to empty if libtoolize 2.x is already in your PATH
LIBTOOLPATH = ''

# You can also set the variable AUTOCONF individually
if AUTOCONFPATH:
    UTILS['autoconf'] = AUTOCONFPATH + 'autoconf'
else:
    if os.getenv('AUTOCONF'):
        UTILS['autoconf'] = os.getenv('AUTOCONF')
    else:
        UTILS['autoconf'] = 'autoconf'

# You can also set the variable AUTORECONF individually
if AUTOCONFPATH:
    UTILS['autoreconf'] = AUTOCONFPATH + 'autoreconf'
else:
    if os.getenv('AUTORECONF'):
        UTILS['autoreconf'] = os.getenv('AUTORECONF')
    else:
        UTILS['autoreconf'] = 'autoreconf'

# You can also set the variable AUTOHEADER individually
if AUTOCONFPATH:
    UTILS['autoheader'] = AUTOCONFPATH + 'autoheader'
else:
    if os.getenv('AUTOHEADER'):
        UTILS['autoheader'] = os.getenv('AUTOHEADER')
    else:
        UTILS['autoheader'] = 'autoheader'

# You can also set the variable AUTOMAKE individually
if AUTOMAKEPATH:
    UTILS['automake'] = AUTOMAKEPATH + 'automake'
else:
    if os.getenv('AUTOMAKE'):
        UTILS['automake'] = os.getenv('AUTOMAKE')
    else:
        UTILS['automake'] = 'automake'

# You can also set the variable ACLOCAL individually
if AUTOMAKEPATH:
    UTILS['aclocal'] = AUTOMAKEPATH + 'aclocal'
else:
    if os.getenv('ACLOCAL'):
        UTILS['aclocal'] = os.getenv('ACLOCAL')
    else:
        UTILS['aclocal'] = 'aclocal'

# You can also set the variable AUTOPOINT individually
if GETTEXTPATH:
    UTILS['autopoint'] = GETTEXTPATH + 'autopoint'
else:
    if os.getenv('AUTOPOINT'):
        UTILS['autopoint'] = os.getenv('AUTOPOINT')
    else:
        UTILS['autopoint'] = 'autopoint'

# You can also set the variable LIBTOOLIZE individually
if LIBTOOLPATH:
    UTILS['libtoolize'] = LIBTOOLPATH + 'libtoolize'
else:
    if os.getenv('LIBTOOLIZE'):
        UTILS['libtoolize'] = os.getenv('LIBTOOLIZE')
    else:
        UTILS['libtoolize'] = 'libtoolize'

# You can also set the variable MAKE
if os.getenv('MAKE'):
    UTILS['make'] = os.getenv('MAKE')
else:
    UTILS['make'] = 'make'


#===============================================================================
# Define global functions
#===============================================================================
def execute(args, verbose):
    '''Execute the given shell command.'''
    if verbose >= 0:
        print("executing %s" % ' '.join(args))
        try:  # Try to run
            retcode = sp.call(args)
        except Exception as error:
            sys.stderr.write(str(error) + '\n')
            sys.exit(1)
    else:
        # Commands like automake produce output to stderr even when they succeed.
        # Turn this output off if the command succeeds.
        temp = tempfile.mktemp()
        xargs = '%s > %s 2>&1' % (' '.join(args), temp)
        try:  # Try to run
            retcode = sp.call(xargs, shell=True)
        except Exception as error:
            sys.stderr.write(str(error) + '\n')
            sys.exit(1)
        if retcode == 0:
            os.remove(temp)
        else:
            print("executing %s" % ' '.join(args))
            with codecs.open(temp, 'rb') as file:
                cmdout = file.read()
            print(cmdout)
            os.remove(temp)
            sys.exit(retcode)


def cleaner(sequence):
    '''Clean string or list of strings after using regex.'''
    if type(sequence) is str:
        sequence = sequence.replace('[', '')
        sequence = sequence.replace(']', '')
    elif type(sequence) is list:
        sequence = [ value.replace('[', '').replace(']', '')
                     for value in sequence]
        sequence = [ value.replace('(', '').replace(')', '')
                     for value in sequence]
        sequence = [ False if value == 'false' else value
                     for value in sequence ]
        sequence = [ True if value == 'true' else value
                     for value in sequence ]
        sequence = [ value.strip()
                     for value in sequence ]
    return sequence


def joinpath(head, *tail):
    '''joinpath(head, *tail) -> str

    Join two or more pathname components, inserting '/' as needed. If any
    component is an absolute path, all previous path components will be
    discarded.'''
    newtail = list()
    for item in tail:
        newtail += [item]
    result = os.path.normpath(os.path.join(head, *tail))
    return result


def relativize(dir1, dir2):
    '''Compute a relative pathname reldir such that dir1/reldir = dir2.
    dir1 and dir2 must be relative pathnames.'''
    dir0 = os.getcwd()
    while dir1:
        dir1 = '%s%s' % (os.path.normpath(dir1), os.path.sep)
        dir2 = '%s%s' % (os.path.normpath(dir2), os.path.sep)
        first = dir1[:dir1.find(os.path.sep)]
        if first != '.':
            if first == '..':
                dir2 = joinpath(os.path.basename(dir0), dir2)
                dir0 = os.path.dirname(dir0)
            else:  # if first != '..'
                # Get first component of dir2
                first2 = dir2[:dir2.find(os.path.sep)]
                if first == first2:
                    dir2 = dir2[dir2.find(os.path.sep) + 1:]
                else:  # if first != first2
                    dir2 = joinpath('..', dir2)
                dir0 = joinpath(dir0, first)
        dir1 = dir1[dir1.find(os.path.sep) + 1:]
    result = os.path.normpath(dir2)
    return result


def relconcat(dir1, dir2):
    '''Compute a relative pathname dir1/dir2, with obvious simplifications.
    dir1 and dir2 must be relative pathnames.
    dir2 is considered to be relative to dir1.'''
    return os.path.normpath(os.path.join(dir1, dir2))


def relinverse(dir):
    '''Compute the inverse of dir. Namely, a relative pathname consisting only
    of '..' components, such that dir/relinverse = '.'.
    dir must be a relative pathname.'''
    if False:
        # This should work too.
        return relativize(dir, '.')
    else:
        inverse = ''
        for component in dir.split('/'):
            if component != '':
                inverse += '../'
        return os.path.normpath(inverse)


def copyfile(src, dest):
    '''Copy file src to file dest. Like shutil.copy, but ignore errors e.g. on
    VFAT file systems.'''
    shutil.copyfile(src, dest)
    try:
        shutil.copymode(src, dest)
    except PermissionError:
        pass


def copyfile2(src, dest):
    '''Copy file src to file dest, preserving modification time. Like
    shutil.copy2, but ignore errors e.g. on VFAT file systems. This function
    is to be used for backup files.'''
    shutil.copyfile(src, dest)
    try:
        shutil.copystat(src, dest)
    except PermissionError:
        pass


def movefile(src, dest):
    '''Move/rename file src to file dest. Like shutil.move, but gracefully
    handle common errors.'''
    try:
        shutil.move(src, dest)
    except PermissionError:
        # shutil.move invokes os.rename, catches the resulting OSError for
        # errno=EXDEV, attempts a copy instead, and encounters a PermissionError
        # while doing that.
        copyfile2(src, dest)
        os.remove(src)


def symlink_relative(src, dest):
    '''Like ln -s, except use cp -p if ln -s fails.
    src is either absolute or relative to the directory of dest.'''
    try:
        os.symlink(src, dest)
    except PermissionError:
        sys.stderr.write('%s: ln -s failed; falling back on cp -p\n' % APP['name'])
        if src.startswith('/') or (len(src) >= 2 and src[1] == ':'):
            # src is absolute.
            cp_src = src
        else:
            # src is relative to the directory of dest.
            last_slash = dest.rfind('/')
            if last_slash >= 0:
                cp_src = joinpath(dest[0:last_slash-1], src)
            else:
                cp_src = src
        copyfile2(cp_src, dest)


def as_link_value_at_dest(src, dest):
    '''Compute the symbolic link value to place at dest, such that the
    resulting symbolic link points to src. src is given relative to the
    current directory (or absolute).'''
    if type(src) is not str:
        raise TypeError('src must be a string, not %s' % (type(src).__name__))
    if type(dest) is not str:
        raise TypeError('dest must be a string, not %s' % (type(dest).__name__))
    if src.startswith('/') or (len(src) >= 2 and src[1] == ':'):
        return src
    else:  # if src is not absolute
        if dest.startswith('/') or (len(dest) >= 2 and dest[1] == ':'):
            cwd = os.getcwd()
            return joinpath(cwd, src)
        else:  # if dest is not absolute
            destdir = os.path.dirname(dest)
            if not destdir:
                destdir = '.'
            return relativize(destdir, src)


def link_relative(src, dest):
    '''Like ln -s, except that src is given relative to the current directory
    (or absolute), not given relative to the directory of dest.'''
    if type(src) is not str:
        raise TypeError('src must be a string, not %s' % (type(src).__name__))
    if type(dest) is not str:
        raise TypeError('dest must be a string, not %s' % (type(dest).__name__))
    link_value = as_link_value_at_dest(src, dest)
    symlink_relative(link_value, dest)


def link_if_changed(src, dest):
    '''Create a symlink, but avoids munging timestamps if the link is correct.'''
    link_value = as_link_value_at_dest(src, dest)
    if not (os.path.islink(dest) and os.readlink(dest) == link_value):
        try:
            os.remove(dest)
        except FileNotFoundError:
            pass
        # Equivalent to link_relative(src, dest):
        symlink_relative(link_value, dest)


def filter_filelist(separator, filelist,
                    prefix, suffix, removed_prefix, removed_suffix,
                    added_prefix='', added_suffix=''):
    '''filter_filelist(*args) -> list

    Filter the given list of files. Filtering: Only the elements starting with
    prefix and ending with suffix are considered. Processing: removed_prefix
    and removed_suffix are removed from each element, added_prefix and
    added_suffix are added to each element.'''
    listing = list()
    for filename in filelist:
        if filename.startswith(prefix) and filename.endswith(suffix):
            pattern = re.compile('^%s(.*)%s$'
                                 % (removed_prefix, removed_suffix))
            result = pattern.sub('%s\\1%s'
                                 % (added_prefix, added_suffix), filename)
            listing += [result]
    result = separator.join(listing)
    return result


def substart(orig, repl, data):
    '''Replaces the start portion of a string.

    Returns data with orig replaced by repl, but only at the beginning of data.
    Like data.replace(orig,repl), except only at the beginning of data.'''
    result = data
    if data.startswith(orig):
        result = repl + data[len(orig):]
    return result


def subend(orig, repl, data):
    '''Replaces the end portion of a string.

    Returns data with orig replaced by repl, but only at the end of data.
    Like data.replace(orig,repl), except only at the end of data.'''
    result = data
    if data.endswith(orig):
        result = data[:-len(orig)] + repl
    return result


def nlconvert(text):
    '''Convert line-endings to specific for this platform.'''
    system = platform.system().lower()
    text = text.replace('\r\n', '\n')
    if system == 'windows':
        text = text.replace('\n', '\r\n')
    return text


def nlremove(text):
    '''Remove empty lines from the source text.'''
    text = nlconvert(text)
    text = text.replace('\r\n', '\n')
    lines = [ line
              for line in text.split('\n')
              if line != '' ]
    text = '\n'.join(lines)
    text = nlconvert(text)
    return text


def remove_trailing_slashes(text):
    '''Remove trailing slashes from a file name, except when the file name
    consists only of slashes.'''
    result = text
    while result.endswith('/'):
        result = result[:-1]
        if result == '':
            result = text
            break
    return result


def remove_backslash_newline(text):
    '''Given a multiline string text, join lines:
    When a line ends in a backslash, remove the backslash and join the next
    line to it.'''
    return text.replace('\\\n', '')


def combine_lines(text):
    '''Given a multiline string text, join lines by spaces:
    When a line ends in a backslash, remove the backslash and join the next
    line to it, inserting a space between them.'''
    return text.replace('\\\n', ' ')


def combine_lines_matching(pattern, text):
    '''Given a multiline string text, join lines by spaces, when the first
    such line matches a given RegexObject pattern.
    When a line that matches the pattern ends in a backslash, remove the
    backslash and join the next line to it, inserting a space between them.
    When a line that is the result of such a join ends in a backslash,
    proceed likewise.'''
    outerpos = 0
    match = pattern.search(text, outerpos)
    while match:
        (startpos, pos) = match.span()
        # Look how far the continuation lines extend.
        pos = text.find('\n', pos)
        while pos > 0 and text[pos - 1] == '\\':
            pos = text.find('\n', pos + 1)
        if pos < 0:
            pos = len(text)
        # Perform a combine_lines throughout the continuation lines.
        partdone = text[:startpos] + combine_lines(text[startpos:pos])
        outerpos = len(partdone)
        text = partdone + text[pos:]
        # Next round.
        match = pattern.search(text, outerpos)
    return text


__all__ += ['APP', 'DIRS', 'MODES', 'UTILS']
