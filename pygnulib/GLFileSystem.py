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
import codecs
import filecmp
import subprocess as sp
from enum import Enum
from . import constants
from .GLError import GLError
from .GLConfig import GLConfig


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
copyfile = constants.copyfile
movefile = constants.movefile
isdir = os.path.isdir
isfile = os.path.isfile


#===============================================================================
# Define CopyAction class
#===============================================================================
class CopyAction(Enum):
    Copy = 0
    Symlink = 1


#===============================================================================
# Define GLFileSystem class
#===============================================================================
class GLFileSystem(object):
    '''GLFileSystem class is used to create virtual filesystem, which is based
    on the gnulib directory and directories specified by localpath argument.
    Its main method lookup(file) is used to find file in these directories or
    combine it using Linux 'patch' utility.'''

    def __init__(self, config):
        '''Create new GLFileSystem instance. The only argument is localpath,
        which can be an empty list.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        self.config = config

    def __repr__(self):
        '''x.__repr__ <==> repr(x)'''
        result = '<pygnulib.GLFileSystem %s>' % hex(id(self))
        return result

    def lookup(self, name):
        '''GLFileSystem.lookup(name) -> tuple

        Lookup a file in gnulib and localpath directories or combine it using
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
            if isfile(file_in_localdir):
                lookedupFile = file_in_localdir
                break
            diff_in_localdir = joinpath(localdir, '%s.diff' % name)
            if isfile(diff_in_localdir):
                lookedupPatches.append(diff_in_localdir)
        # Treat the gnulib dir like a lowest-priority --local-dir, except that
        # here we don't look for .diff files.
        if lookedupFile == None:
            file_in_localdir = joinpath(DIRS['root'], name)
            if isfile(file_in_localdir):
                lookedupFile = file_in_localdir
        if lookedupFile != None:
            if len(lookedupPatches) > 0:
                # Apply the patches, from lowest-priority to highest-priority.
                tempFile = joinpath(self.config['tempdir'], name)
                try:  # Try to create directories
                    os.makedirs(os.path.dirname(tempFile))
                except OSError as error:
                    pass  # Skip errors if directory exists
                if isfile(tempFile):
                    os.remove(tempFile)
                copyfile(lookedupFile, tempFile)
                for diff_in_localdir in reversed(lookedupPatches):
                    command = 'patch -s "%s" < "%s" >&2' % (tempFile, diff_in_localdir)
                    try:  # Try to apply patch
                        sp.check_call(command, shell=True)
                    except sp.CalledProcessError as error:
                        raise GLError(2, name)
                result = (tempFile, True)
            else:
                result = (lookedupFile, False)
        else:
            raise GLError(1, name)
        return result

    def shouldLink(self, original, lookedup):
        '''GLFileSystem.shouldLink(original, lookedup)

        Determines whether the original file should be copied or symlinked.
        Returns a CopyAction.'''
        symbolic = self.config['symbolic']
        lsymbolic = self.config['lsymbolic']
        localpath = self.config['localpath']
        if symbolic:
            return CopyAction.Symlink
        if lsymbolic:
            for localdir in localpath:
                if lookedup == joinpath(localdir, original):
                    return CopyAction.Symlink
        return CopyAction.Copy


#===============================================================================
# Define GLFileAssistant class
#===============================================================================
class GLFileAssistant(object):
    '''GLFileAssistant is used to help with file processing.'''

    def __init__(self, config, transformers=dict()):
        '''Create GLFileAssistant instance.'''
        if type(config) is not GLConfig:
            raise TypeError('config must be a GLConfig, not %s'
                            % type(config).__name__)
        if type(transformers) is not dict:
            raise TypeError('transformers must be a dict, not %s'
                            % type(transformers).__name__)
        for key in ['lib', 'aux', 'main', 'tests']:
            if key not in transformers:
                transformers[key] = 's,x,x,'
            else:  # if key in transformers
                value = transformers[key]
                if type(value) is not str:
                    raise TypeError('transformers[%s] must be a string, not %s'
                                    % (key, type(value).__name__))
        self.original = None
        self.rewritten = None
        self.added = list()
        self.config = config
        self.transformers = transformers
        self.filesystem = GLFileSystem(self.config)

    def __repr__(self):
        '''x.__repr__() <==> repr(x)'''
        result = '<pygnulib.GLFileAssistant %s>' % hex(id(self))
        return result

    def tmpfilename(self, path):
        '''GLFileAssistant.tmpfilename() -> str

        Return the name of a temporary file (file is relative to destdir).'''
        if type(path) is not str:
            raise TypeError('path must be a string, not %s'
                            % (type(path).__name__))
        if not self.config['dryrun']:
            # Put the new contents of $file in a file in the same directory (needed
            # to guarantee that an 'mv' to "$destdir/$file" works).
            result = joinpath(self.config['destdir'], '%s.tmp' % path)
            dirname = os.path.dirname(result)
            if dirname and not isdir(dirname):
                os.makedirs(dirname)
        else:  # if self.config['dryrun']
            # Put the new contents of $file in a file in a temporary directory
            # (because the directory of "$file" might not exist).
            tempdir = self.config['tempdir']
            result = joinpath(tempdir, '%s.tmp' % os.path.basename(path))
            dirname = os.path.dirname(result)
            if not isdir(dirname):
                os.makedirs(dirname)
        return result

    def setOriginal(self, original):
        '''GLFileAssistant.setOriginal(original)

        Set the name of the original file which will be used.'''
        if type(original) is not str:
            raise TypeError('original must be a string, not %s'
                            % (type(original).__name__))
        self.original = original

    def setRewritten(self, rewritten):
        '''GLFileAssistant.setRewritten(rewritten)

        Set the name of the rewritten file which will be used.'''
        if type(rewritten) is not str:
            raise TypeError('rewritten must be a string, not %s'
                            % type(rewritten).__name__)
        self.rewritten = rewritten

    def addFile(self, file):
        '''GLFileAssistant.addFile(file)

        Add file to the list of added files.'''
        if file not in self.added:
            self.added += [file]

    def removeFile(self, file):
        '''GLFileAssistant.removeFile(file)

        Remove file from the list of added files.'''
        if file in self.added:
            self.added.pop(file)

    def getFiles(self):
        '''Return list of the added files.'''
        return list(self.added)

    def add(self, lookedup, tmpflag, tmpfile):
        '''GLFileAssistant.add(lookedup, tmpflag, tmpfile)

        This method copies a file from gnulib into the destination directory.
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
                constants.link_if_changed(lookedup, joinpath(destdir, rewritten))
            else:  # if any of these conditions is not met
                try:  # Try to move file
                    movefile(tmpfile, joinpath(destdir, rewritten))
                except Exception as error:
                    raise GLError(17, original)
        else:  # if self.config['dryrun']
            print('Copy file %s' % rewritten)

    def update(self, lookedup, tmpflag, tmpfile, already_present):
        '''GLFileAssistant.update(lookedup, tmpflag, tmpfile, already_present)

        This method copies a file from gnulib into the destination directory.
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
                if isfile(backuppath):
                    os.remove(backuppath)
                try:  # Try to replace the given file
                    movefile(basepath, backuppath)
                except Exception as error:
                    raise GLError(17, original)
                if self.filesystem.shouldLink(original, lookedup) == CopyAction.Symlink \
                        and not tmpflag and filecmp.cmp(lookedup, tmpfile):
                    constants.link_if_changed(lookedup, basepath)
                else:  # if any of these conditions is not met
                    try:  # Try to move file
                        if os.path.exists(basepath):
                            os.remove(basepath)
                        copyfile(tmpfile, rewritten)
                    except Exception as error:
                        raise GLError(17, original)
            else:  # if self.config['dryrun']
                if already_present:
                    print('Update file %s (backup in %s)' % (rewritten, backupname))
                else:  # if not already_present
                    print('Replace file %s (backup in %s)' % (rewritten, backupname))

    def add_or_update(self, already_present):
        '''GLFileAssistant.add_or_update(already_present)

        This method handles a file that ought to be present afterwards.'''
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
            xoriginal = constants.substart('tests=lib/', 'lib/', original)
        lookedup, tmpflag = self.filesystem.lookup(xoriginal)
        tmpfile = self.tmpfilename(rewritten)
        sed_transform_lib_file = self.transformers.get('lib', '')
        sed_transform_build_aux_file = self.transformers.get('aux', '')
        sed_transform_main_lib_file = self.transformers.get('main', '')
        sed_transform_testsrelated_lib_file = self.transformers.get('tests', '')
        try:  # Try to copy lookedup file to tmpfile
            copyfile(lookedup, tmpfile)
        except Exception as error:
            raise GLError(15, lookedup)
        # Don't process binary files with sed.
        if not (original.endswith(".class") or original.endswith(".mo")):
            transformer = ''
            if original.startswith('lib/'):
                if sed_transform_main_lib_file:
                    transformer = sed_transform_main_lib_file
            elif original.startswith('build-aux/'):
                if sed_transform_build_aux_file:
                    transformer = sed_transform_build_aux_file
            elif original.startswith('tests=lib/'):
                if sed_transform_testsrelated_lib_file:
                    transformer = sed_transform_testsrelated_lib_file
            if transformer:
                args = ['sed', '-e', transformer]
                stdin = codecs.open(lookedup, 'rb', 'UTF-8')
                try:  # Try to transform file
                    data = sp.check_output(args, stdin=stdin, shell=False)
                    data = data.decode("UTF-8")
                except Exception as error:
                    raise GLError(16, lookedup)
                with codecs.open(tmpfile, 'wb', 'UTF-8') as file:
                    file.write(data)
        path = joinpath(self.config['destdir'], rewritten)
        if isfile(path):
            self.update(lookedup, tmpflag, tmpfile, already_present)
            os.remove(tmpfile)
        else:  # if not isfile(path)
            self.add(lookedup, tmpflag, tmpfile)
            self.addFile(rewritten)

    def super_update(self, basename, tmpfile):
        '''GLFileAssistant.super_update(basename, tmpfile) -> tuple

        Move tmpfile to destdir/basename path, making a backup of it.
        Returns tuple, which contains basename, backupname and status.
          0: tmpfile is the same as destfile;
          1: tmpfile was used to update destfile;
          2: destfile was created, because it didn't exist.'''
        backupname = '%s~' % basename
        basepath = joinpath(self.config['destdir'], basename)
        backuppath = joinpath(self.config['destdir'], backupname)
        if isfile(basepath):
            if filecmp.cmp(basepath, tmpfile):
                result_flag = 0
            else:  # if not filecmp.cmp(basepath, tmpfile)
                result_flag = 1
                if not self.config['dryrun']:
                    if isfile(backuppath):
                        os.remove(backuppath)
                    movefile(basepath, backuppath)
                    movefile(tmpfile, basepath)
                else:  # if self.config['dryrun']
                    os.remove(tmpfile)
        else:  # if not isfile(basepath)
            result_flag = 2
            if not self.config['dryrun']:
                if isfile(basepath):
                    os.remove(basepath)
                movefile(tmpfile, basepath)
            else:  # if self.config['dryrun']
                os.remove(tmpfile)
        result = tuple([basename, backupname, result_flag])
        return result
