#!/usr/bin/python
# encoding: UTF-8
"""gnulib virtual filesystem API"""



import codecs as _codecs_
import os as _os_
import shutil as _shutil_
import tempfile as _tempfile_
import subprocess as _sp_


from .error import type_assert as _type_assert_
from .error import UnknownModuleError as _UnknownModuleError_
from .module import Base as _BaseModule_
from .module import File as _FileModule_



class Base:
    """gnulib generic virtual file system"""
    def __init__(self, root, patch="patch", **table):
        _type_assert_("root", root, str)
        _type_assert_("patch", patch, str)
        self.__table = {}
        for (key, value) in table.items():
            _type_assert_(key, value, str)
            self.__table[key] = _os_.path.normpath(value)
        self.__root = root
        self.__patch = patch


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{{{}}}".format(module, name, repr(self.__root))


    def __getitem__(self, name):
        _type_assert_("name", name, str)
        parts = []
        replaced = False
        path = _os_.path.normpath(name)
        if _os_.path.isabs(path):
            raise ValueError("name cannot be an absolute path")
        for part in path.split(_os_.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced and part in self.__table:
                part = self.__table[part]
                replaced = True
            parts += [part]
        path = _os_.path.sep.join([self.__root] + parts)
        return _os_.path.normpath(path)


    @property
    def path(self):
        """directory path"""
        return self.__root



class Project(Base):
    """project virtual file system"""
    def __init__(self, name, **table):
        path = _os_.path.realpath(name)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        if not _os_.path.isdir(path):
            raise NotADirectoryError(path)
        super().__init__(name, **table)


    def __contains__(self, name):
        path = _os_.path.normpath(name)
        if _os_.path.isabs(name):
            raise ValueError("name must be a relative path")
        path = _os_.path.join(self.__root, name)
        return _os_.path.exists(path)


    def __backup(self, name):
        backup = "{}~".format(name)
        try:
            _os_.unlink(self[backup])
        except FileNotFoundError:
            pass # ignore non-existent files
        _shutil_.copy(self[name], self[backup])


    def backup(self, name):
        """Backup the given file."""
        return self.__backup(name)


    def lookup(self, name, primary, secondary):
        """
        Try to look up a regular file inside virtual file systems or combine it via patch utility.

        - file is present only inside the primary VFS: open the file inside the primary VFS.
        - file is present inside the secondary VFS: open the file inside the secondary VFS.
        - both file and patch are present: combine in memory.
        - file is not present: raise an FileNotFoundError exception.

        The function returns a pair of values, representing the file stream and path (if any).
        The first element, stream, is a file-like object, which supports read operations.
        The second element, path, may be either a regular file system path or None.
        If path is None, then the file was created in-memory via dynamic patching.
        """
        _type_assert_("primary", primary, Base)
        _type_assert_("secondary", secondary, Base)
        if name in secondary:
            path = secondary[name]
            stream = _codecs_.open(path, "rb")
            return (stream, path)
        diff = "{}.diff".format(name)
        if diff not in secondary:
            path = primary[name]
            stream = _codecs_.open(path, "rb")
            return (stream, path)
        tmp = _tempfile_.NamedTemporaryFile(mode="w+b", delete=False)
        with _codecs_.open(primary[name], "rb") as stream:
            _shutil_.copyfileobj(stream, tmp)
            tmp.close()
        stdin = _codecs_.open(secondary[diff], "rb")
        cmd = (self.__patch, "-s", tmp.name)
        pipes = _sp_.Popen(cmd, stdin=stdin, stdout=_sp_.PIPE, stderr=_sp_.PIPE)
        (stdout, stderr) = pipes.communicate()
        stdout = stdout.decode("UTF-8")
        stderr = stderr.decode("UTF-8")
        returncode = pipes.returncode
        if returncode != 0:
            cmd = "patch -s {} < {}".format(tmp.name, secondary[diff])
            raise _sp_.CalledProcessError(returncode, cmd, stdout, stderr)
        stream = _codecs_.open(tmp.name, "rb")
        _os_.unlink(tmp.name)
        return (stream, None)


    def unlink(self, name, backup=True):
        """Unlink a file, backing it up if necessary."""
        if backup:
            self.__backup(name)
        _os_.unlink(self[name])



class GnulibGit(Base):
    """gnulib git repository"""
    _EXCLUDE_ = {
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


    def __init__(self, name, **table):
        path = _os_.path.realpath(name)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        if not _os_.path.isdir(path):
            raise NotADirectoryError(path)
        super().__init__(name, **table)
        if not _os_.path.isdir(_os_.path.join(self.path, ".git")):
            raise TypeError("{} is not a gnulib repository".format(self.path))


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def module(self, name, full=True):
        """obtain gnulib module by name"""
        _type_assert_("name", name, str)
        _type_assert_("full", full, bool)
        if name in GnulibGit._EXCLUDE_:
            raise ValueError("illegal module name")
        path = _os_.path.join(self["modules"], name)
        try:
            return _FileModule_(path, name=name) if full else _BaseModule_(name)
        except FileNotFoundError:
            raise _UnknownModuleError_(name)


    def modules(self, full=True):
        """iterate over all available modules"""
        prefix = self["modules"]
        for root, _, files in _os_.walk(prefix):
            names = []
            for name in files:
                exclude = False
                for (key, method) in GnulibGit._EXCLUDE_.items():
                    if method(name, key):
                        exclude = True
                        break
                if not exclude:
                    names += [name]
            for name in names:
                path = _os_.path.join(root, name)
                name = path[len(prefix) + 1:]
                yield self.module(name, full)
