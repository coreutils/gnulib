#!/usr/bin/python
# encoding: UTF-8
"""gnulib virtual filesystem API"""



import codecs as _codecs
import os as _os
import shutil as _shutil
import tempfile as _tempfile
import subprocess as _sp


from .error import type_assert as _type_assert
from .error import UnknownModuleError as _UnknownModuleError
from .module import Base as _BaseModule
from .module import File as _FileModule



class Base:
    """gnulib generic virtual file system"""
    def __init__(self, prefix, **table):
        _type_assert("prefix", prefix, str)
        self.__table = {}
        for (key, value) in table.items():
            _type_assert(key, value, str)
            self.__table[key] = _os.path.normpath(value)
        self.__prefix = _os.path.abspath(prefix)


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{{{}}}".format(module, name, repr(self.__prefix))


    def __contains__(self, name):
        path = _os.path.normpath(name)
        if _os.path.isabs(name):
            raise ValueError("name must be a relative path")
        path = _os.path.join(self.__prefix, name)
        return _os.path.exists(path)


    def __getitem__(self, name):
        _type_assert("name", name, str)
        parts = []
        replaced = False
        path = _os.path.normpath(name)
        if _os.path.isabs(path):
            raise ValueError("name cannot be an absolute path")
        for part in path.split(_os.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced and part in self.__table:
                part = self.__table[part]
                replaced = True
            parts += [part]
        path = _os.path.sep.join([self.__prefix] + parts)
        return _os.path.normpath(path)


    @property
    def base_prefix(self):
        """base path prefix"""
        return self.__prefix


    @property
    def full_prefix(self):
        """absolute path prefix"""
        return _os.path.abspath(self.__prefix)



class Project(Base):
    """project virtual file system"""
    def __init__(self, name, **table):
        path = _os.path.realpath(name)
        if not _os.path.exists(path):
            raise FileNotFoundError(path)
        if not _os.path.isdir(path):
            raise NotADirectoryError(path)
        super().__init__(name, **table)
        self.__patch = None


    @property
    def patch(self):
        """path to patch binary"""
        if self.__patch is None:
            raise AttributeError("patch")
        return self.__patch

    @patch.setter
    def patch(self, path):
        _type_assert("path", path, str)
        if not _os.path.isabs(path):
            if _shutil.which(path) is None:
                raise FileNotFoundError("patch")
        self.__patch = path


    def backup(self, name):
        """Backup the given file."""
        backup = "{}~".format(name)
        try:
            _os.unlink(self[backup])
        except FileNotFoundError:
            pass # ignore non-existent files
        _shutil.copy(self[name], self[backup])


    def copy(self, src, dst):
        """Copy file data."""
        srcabs = _os.path.isabs(self, src)
        dstabs = _os.path.isabs(self, dst)
        if srcabs and dstabs:
            raise ValueError("absolute src and dst")
        limit = (16 * 1024)
        src = src if srcabs else _os.path.join(self.full_prefix, src)
        dst = dst if dstabs else _os.path.join(self.full_prefix, dst)
        with _codecs.open(src, "rb") as istream:
            with _codecs.open(dst, "wb") as ostream:
                while 1:
                    data = istream.read(limit)
                    if not data:
                        break
                    ostream.write(data)


    def hardlink(self, src, dst):
        """Create a hard link to the file."""
        srcabs = _os.path.isabs(self, src)
        dstabs = _os.path.isabs(self, dst)
        if srcabs and dstabs:
            raise ValueError("absolute src and dst")
        src = src if srcabs else _os.path.join(self.full_prefix, src)
        dst = dst if dstabs else _os.path.join(self.full_prefix, dst)
        _os.link(src, dst)


    def lookup(self, name, primary, secondary):
        """
        Try to look up a regular file inside virtual file systems or combine it via patch utility.

        - file is present only inside the primary VFS: open the file inside the primary VFS.
        - file is present inside the secondary VFS: open the file inside the secondary VFS.
        - both file and patch are present: combine in memory.
        - file is not present: raise an FileNotFoundError exception.

        The function returns a pair of values, representing the file path and state.
        The first element, path, represents a regular file system path.
        The second element, dynamic, designates whether the file is temporary.
        It is up to the caller to unlink dynamic files.
        """
        _type_assert("primary", primary, Base)
        _type_assert("secondary", secondary, Base)
        if name in secondary:
            return (secondary[name], False)
        diff = "{}.diff".format(name)
        if diff not in secondary:
            return (primary[name], False)
        tmp = _tempfile.NamedTemporaryFile(mode="w+b", delete=False)
        with _codecs.open(primary[name], "rb") as stream:
            _shutil.copyfileobj(stream, tmp)
            tmp.close()
        stdin = _codecs.open(secondary[diff], "rb")
        cmd = (self.__patch, "-s", tmp.name)
        pipes = _sp.Popen(cmd, stdin=stdin, stdout=_sp.PIPE, stderr=_sp.PIPE)
        (stdout, stderr) = pipes.communicate()
        stdout = stdout.decode("UTF-8")
        stderr = stderr.decode("UTF-8")
        returncode = pipes.returncode
        if returncode != 0:
            cmd = "patch -s {} < {}".format(tmp.name, secondary[diff])
            raise _sp.CalledProcessError(returncode, cmd, stdout, stderr)
        return (tmp.name, True)


    def mkdir(self, name):
        """Create a leaf directory and all intermediate ones recursively."""
        _os.makedirs(self[name], exist_ok=True)


    def move(self, src, dst):
        """Move file data."""
        srcabs = _os.path.isabs(self, src)
        dstabs = _os.path.isabs(self, dst)
        if srcabs and dstabs:
            raise ValueError("absolute src and dst")
        src = src if srcabs else _os.path.join(self.full_prefix, src)
        dst = dst if dstabs else _os.path.join(self.full_prefix, dst)
        _os.rename(src, dst)


    def symlink(self, src, dst):
        """Create a symbolic link to the file."""
        srcabs = _os.path.isabs(self, src)
        dstabs = _os.path.isabs(self, dst)
        if srcabs and dstabs:
            raise ValueError("absolute src and dst")
        src = src if srcabs else _os.path.join(self.full_prefix, src)
        dst = dst if dstabs else _os.path.join(self.full_prefix, dst)
        _os.symlink(src, dst)


    def unlink(self, name, backup=True):
        """Unlink a file, backing it up if necessary."""
        if backup:
            self.backup(name)
        _os.unlink(self[name])



class GnulibGit(Base):
    """gnulib git repository"""
    _EXCLUDE = {
        "."                 : str.startswith,
        "~"                 : str.endswith,
        "-tests"            : str.endswith,
        "ChangeLog"         : str.__eq__,
        "COPYING"           : str.__eq__,
        "README"            : str.__eq__,
        "TEMPLATE"          : str.__eq__,
        "TEMPLATE-TESTS"    : str.__eq__,
        "TEMPLATE-EXTENDED" : str.__eq__,
    }


    def __init__(self, prefix, **table):
        super().__init__(prefix, **table)
        if not _os.path.exists(self.full_prefix):
            raise FileNotFoundError(self.full_prefix)
        if not _os.path.isdir(self.full_prefix):
            raise NotADirectoryError(self.full_prefix)
        if not _os.path.isdir(_os.path.join(self.full_prefix, ".git")):
            raise TypeError("{} is not a gnulib repository".format(prefix))


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def module(self, name, full=True):
        """obtain gnulib module by name"""
        _type_assert("name", name, str)
        _type_assert("full", full, bool)
        if name in GnulibGit._EXCLUDE:
            raise ValueError("illegal module name")
        path = _os.path.join(self["modules"], name)
        try:
            return _FileModule(path, name=name) if full else _BaseModule(name)
        except FileNotFoundError:
            raise _UnknownModuleError(name)


    def modules(self, full=True):
        """iterate over all available modules"""
        prefix = self["modules"]
        for root, _, files in _os.walk(prefix):
            names = []
            for name in files:
                exclude = False
                for (key, method) in GnulibGit._EXCLUDE.items():
                    if method(name, key):
                        exclude = True
                        break
                if not exclude:
                    names += [name]
            for name in names:
                path = _os.path.join(root, name)
                name = path[len(prefix) + 1:]
                yield self.module(name, full)
