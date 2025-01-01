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
import filecmp
import shlex
import subprocess as sp
from .constants import (
    DIRS,
    ensure_writable,
    hardlink,
    joinpath,
    link_if_changed,
    movefile,
    copyfile,
    substart,
)
from .enums import CopyAction
from .GLError import GLError
from .GLConfig import GLConfig


#===============================================================================
# Define GLFileSystem class
#===============================================================================
class GLFileSystem:
    '''GLFileSystem class is used to create virtual filesystem, which is based
    on the gnulib directory and directories specified by localpath argument.
    Its main method lookup(file) is used to find file in these directories or
    combine it using Linux 'patch' utility.'''

    config: GLConfig

    def __init__(self, config: GLConfig) -> None:
        '''Create new GLFileSystem instance. The only argument is localpath,
        which can be an empty list.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config

    def __repr__(self) -> str:
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLFileSystem %s>' % hex(id(self))
        return result

    def lookup(self, name: str) -> tuple[str, bool]:
        '''Lookup a file in gnulib and localpath directories or combine it using
        'patch' utility. If file was found, method returns string, else it raises
        GLError telling that file was not found. Function also returns flag which
        indicates whether file is a temporary file.
        GLConfig: localpath.'''
        if type(name) is not str:
            raise TypeError('name must be a string, not %s'
                            % type(name).__name__)
        localpath = self.config['localpath']
        # Each element in localpath is a directory whose contents overrides
        # or amends the result of the lookup in the rest of localpath and
        # the gnulib dir. So, the first element of localpath is the highest
        # priority one.
        lookedupFile = None
        lookedupPatches = []
        for localdir in localpath:
            file_in_localdir = joinpath(localdir, name)
            if os.path.isfile(file_in_localdir):
                lookedupFile = file_in_localdir
                break
            diff_in_localdir = joinpath(localdir, '%s.diff' % name)
            if os.path.isfile(diff_in_localdir):
                lookedupPatches.append(diff_in_localdir)
        # Treat the gnulib dir like a lowest-priority --local-dir, except that
        # here we don't look for .diff files.
        if lookedupFile == None:
            file_in_localdir = joinpath(DIRS['root'], name)
            if os.path.isfile(file_in_localdir):
                lookedupFile = file_in_localdir
        if lookedupFile != None:
            if len(lookedupPatches) > 0:
                # Apply the patches, from lowest-priority to highest-priority.
                tempFile = joinpath(self.config['tempdir'], name)
                try:  # Try to create directories
                    os.makedirs(os.path.dirname(tempFile))
                except OSError:
                    pass  # Skip errors if directory exists
                if os.path.isfile(tempFile):
                    os.remove(tempFile)
                copyfile(lookedupFile, tempFile)
                ensure_writable(tempFile)
                for diff_in_localdir in reversed(lookedupPatches):
                    command = f'patch -s {shlex.quote(tempFile)} < {shlex.quote(diff_in_localdir)}'
                    try:  # Try to apply patch
                        sp.check_call(command, shell=True, stdout=sys.stderr)
                    except sp.CalledProcessError as exc:
                        raise GLError(2, name) from exc
                result = (tempFile, True)
            else:
                result = (lookedupFile, False)
        else:
            raise GLError(1, name)
        return result

    def shouldLink(self, original: str, lookedup: str) -> bool:
        '''GLFileSystem.shouldLink(original, lookedup)

        Determines whether the original file should be copied, symlinked,
          or hardlinked.
        Returns a CopyAction.'''
        copymode = self.config['copymode']
        lcopymode = self.config['lcopymode']
        localpath = self.config['localpath']

        # Don't bother checking the localpath's if we end up performing the same
        # action anyways.
        if copymode != lcopymode:
            for localdir in localpath:
                if lookedup == joinpath(localdir, original):
                    return lcopymode
        return copymode


#===============================================================================
# Define GLFileAssistant class
#===============================================================================
class GLFileAssistant:
    '''GLFileAssistant is used to help with file processing.'''

    original: str | None
    rewritten: str | None
    added: list[str]
    config: GLConfig
    transformers: dict[str, tuple[re.Pattern, str] | None]
    filesystem: GLFileSystem

    def __init__(self, config: GLConfig, transformers: dict[str, tuple[re.Pattern, str] | None] | None = None) -> None:
        '''Create GLFileAssistant instance.

        config stores information shared between classes.
        transformers is a dictionary which uses a file category as the key. The
          value accessed is a tuple containing arguments for re.sub() or None if
          no transformations are needed.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if transformers == None:
            transformers = dict()
        elif type(transformers) is not dict:
            raise TypeError('transformers must be a dict, not %s'
                            % type(transformers).__name__)
        for key in ['lib', 'aux', 'main', 'tests']:
            if key not in transformers:
                transformers[key] = None
            else:  # if key in transformers
                value = transformers[key]
                if type(value) is not tuple and value != None:
                    raise TypeError('transformers[%s] must be a tuple or None, not %s'
                                    % (key, type(value).__name__))
        self.original = None
        self.rewritten = None
        self.added = []
        self.config = config
        self.transformers = transformers
        self.filesystem = GLFileSystem(self.config)

    def __repr__(self) -> str:
        '''x.__repr__() <==> repr(x)'''
        result = '<pygnulib.GLFileAssistant %s>' % hex(id(self))
        return result

    def tmpfilename(self, path: str) -> str:
        '''Return the name of a temporary file (file is relative to destdir).'''
        if type(path) is not str:
            raise TypeError('path must be a string, not %s'
                            % (type(path).__name__))
        if not self.config['dryrun']:
            # Put the new contents of $file in a file in the same directory (needed
            # to guarantee that an 'mv' to "$destdir/$file" works).
            result = joinpath(self.config['destdir'], '%s.tmp' % path)
            dirname = os.path.dirname(result)
            if dirname and not os.path.isdir(dirname):
                os.makedirs(dirname)
        else:  # if self.config['dryrun']
            # Put the new contents of $file in a file in a temporary directory
            # (because the directory of "$file" might not exist).
            tempdir = self.config['tempdir']
            result = joinpath(tempdir, '%s.tmp' % os.path.basename(path))
            dirname = os.path.dirname(result)
            if not os.path.isdir(dirname):
                os.makedirs(dirname)
        return result

    def setOriginal(self, original: str) -> None:
        '''Set the name of the original file which will be used.'''
        if type(original) is not str:
            raise TypeError('original must be a string, not %s'
                            % (type(original).__name__))
        self.original = original

    def setRewritten(self, rewritten: str) -> None:
        '''Set the name of the rewritten file which will be used.'''
        if type(rewritten) is not str:
            raise TypeError('rewritten must be a string, not %s'
                            % type(rewritten).__name__)
        self.rewritten = rewritten

    def addFile(self, file: str) -> None:
        '''Add file to the list of added files.'''
        if file not in self.added:
            self.added.append(file)

    def getFiles(self) -> list[str]:
        '''Return list of the added files.'''
        return list(self.added)

    def add(self, lookedup: str, tmpflag: bool, tmpfile: str) -> None:
        '''This method copies a file from gnulib into the destination directory.
        The destination is known to exist. If tmpflag is True, then lookedup file
        is a temporary one.'''
        original = self.original
        rewritten = self.rewritten
        destdir = self.config['destdir']
        if original == None:
            raise TypeError('original must be set before applying the method')
        if rewritten == None:
            raise TypeError('rewritten must be set before applying the method')
        if not self.config['dryrun']:
            print('Copying file %s' % rewritten)
            if self.filesystem.shouldLink(original, lookedup) == CopyAction.Symlink \
                    and not tmpflag and filecmp.cmp(lookedup, tmpfile):
                link_if_changed(lookedup, joinpath(destdir, rewritten))
            else:  # if any of these conditions is not met
                if self.filesystem.shouldLink(original, lookedup) == CopyAction.Hardlink \
                   and not tmpflag and filecmp.cmp(lookedup, tmpfile):
                    hardlink(lookedup, joinpath(destdir, rewritten))
                else:  # Move instead of linking.
                    try:  # Try to move file
                        movefile(tmpfile, joinpath(destdir, rewritten))
                    except Exception as exc:
                        raise GLError(17, original) from exc
        else:  # if self.config['dryrun']
            print('Copy file %s' % rewritten)

    def update(self, lookedup: str, tmpflag: bool, tmpfile: str, already_present: bool) -> None:
        '''This method copies a file from gnulib into the destination directory.
        The destination is known to exist. If tmpflag is True, then lookedup file
        is a temporary one.'''
        original = self.original
        rewritten = self.rewritten
        destdir = self.config['destdir']
        if original == None:
            raise TypeError('original must be set before applying the method')
        if rewritten == None:
            raise TypeError('rewritten must be set before applying the method')
        if type(lookedup) is not str:
            raise TypeError('lookedup must be a string, not %s'
                            % type(lookedup).__name__)
        if type(already_present) is not bool:
            raise TypeError('already_present must be a bool, not %s'
                            % type(already_present).__name__)
        basename = rewritten
        backupname = '%s~' % basename
        basepath = joinpath(destdir, basename)
        backuppath = joinpath(destdir, backupname)
        if not filecmp.cmp(basepath, tmpfile):
            if not self.config['dryrun']:
                if already_present:
                    print('Updating file %s (backup in %s)' % (basename, backupname))
                else:  # if not already_present
                    message = 'Replacing file '
                    message += '%s (non-gnulib code backed up in ' % basename
                    message += '%s) !!' % backupname
                    print(message)
                if os.path.isfile(backuppath):
                    os.remove(backuppath)
                try:  # Try to replace the given file
                    movefile(basepath, backuppath)
                except Exception as exc:
                    raise GLError(17, original) from exc
                if self.filesystem.shouldLink(original, lookedup) == CopyAction.Symlink \
                        and not tmpflag and filecmp.cmp(lookedup, tmpfile):
                    link_if_changed(lookedup, basepath)
                else:  # if any of these conditions is not met
                    if self.filesystem.shouldLink(original, lookedup) == CopyAction.Hardlink \
                       and not tmpflag and filecmp.cmp(lookedup, tmpfile):
                        hardlink(lookedup, basepath)
                    else:  # Move instead of linking.
                        try:  # Try to move file
                            if os.path.exists(basepath):
                                os.remove(basepath)
                            copyfile(tmpfile, joinpath(destdir, rewritten))
                        except Exception as exc:
                            raise GLError(17, original) from exc
            else:  # if self.config['dryrun']
                if already_present:
                    print('Update file %s (backup in %s)' % (rewritten, backupname))
                else:  # if not already_present
                    print('Replace file %s (backup in %s)' % (rewritten, backupname))

    def add_or_update(self, already_present: bool) -> None:
        '''This method handles a file that ought to be present afterwards.'''
        original = self.original
        rewritten = self.rewritten
        if original == None:
            raise TypeError('original must be set before applying the method')
        elif rewritten == None:
            raise TypeError('rewritten must be set before applying the method')
        if type(already_present) is not bool:
            raise TypeError('already_present must be a bool, not %s'
                            % type(already_present).__name__)
        xoriginal = original
        if original.startswith('tests=lib/'):
            xoriginal = substart('tests=lib/', 'lib/', original)
        lookedup, tmpflag = self.filesystem.lookup(xoriginal)
        tmpfile = self.tmpfilename(rewritten)
        sed_transform_build_aux_file = self.transformers.get('aux')
        sed_transform_main_lib_file = self.transformers.get('main')
        sed_transform_testsrelated_lib_file = self.transformers.get('tests')
        try:  # Try to copy lookedup file to tmpfile
            copyfile(lookedup, tmpfile)
            ensure_writable(tmpfile)
        except Exception as exc:
            raise GLError(15, lookedup) from exc
        # Don't process binary files with sed.
        if not (original.endswith('.class') or original.endswith('.mo')):
            transformer = None
            if original.startswith('lib/'):
                if sed_transform_main_lib_file:
                    transformer = sed_transform_main_lib_file
            elif original.startswith('build-aux/'):
                if sed_transform_build_aux_file:
                    transformer = sed_transform_build_aux_file
            elif original.startswith('tests=lib/'):
                if sed_transform_testsrelated_lib_file:
                    transformer = sed_transform_testsrelated_lib_file
            if transformer != None:
                # Read the file that we looked up.
                with open(lookedup, mode='r', newline='\n', encoding='utf-8') as file:
                    src_data = file.read()
                # Write the transformed data to the temporary file.
                with open(tmpfile, mode='w', newline='\n', encoding='utf-8') as file:
                    file.write(re.sub(transformer[0], transformer[1], src_data))
        path = joinpath(self.config['destdir'], rewritten)
        if os.path.isfile(path):
            # The file already exists.
            self.update(lookedup, tmpflag, tmpfile, already_present)
        else:  # if not os.path.isfile(path)
            # Install the file.
            # Don't protest if the file should be there but isn't: it happens
            # frequently that developers don't put autogenerated files under version control.
            self.add(lookedup, tmpflag, tmpfile)
            self.addFile(rewritten)
        if os.path.isfile(tmpfile):
            os.remove(tmpfile)

    def super_update(self, basename: str, tmpfile: str) -> tuple[str, str, int]:
        '''Move tmpfile to destdir/basename path, making a backup of it.
        Returns tuple, which contains basename, backupname and status.
          0: tmpfile is the same as destfile;
          1: tmpfile was used to update destfile;
          2: destfile was created, because it didn't exist.'''
        backupname = '%s~' % basename
        basepath = joinpath(self.config['destdir'], basename)
        backuppath = joinpath(self.config['destdir'], backupname)
        if os.path.isfile(basepath):
            if filecmp.cmp(basepath, tmpfile):
                result_flag = 0
            else:  # if not filecmp.cmp(basepath, tmpfile)
                result_flag = 1
                if not self.config['dryrun']:
                    if os.path.isfile(backuppath):
                        os.remove(backuppath)
                    movefile(basepath, backuppath)
                    movefile(tmpfile, basepath)
                else:  # if self.config['dryrun']
                    os.remove(tmpfile)
        else:  # if not os.path.isfile(basepath)
            result_flag = 2
            if not self.config['dryrun']:
                if os.path.isfile(basepath):
                    os.remove(basepath)
                movefile(tmpfile, basepath)
            else:  # if self.config['dryrun']
                os.remove(tmpfile)
        result = tuple([basename, backupname, result_flag])
        return result
