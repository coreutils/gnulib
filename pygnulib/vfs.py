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
    def __init__(self, path, **kwargs):
        _type_assert_("path", path, str)
        self.__table = {}
        for (key, value) in kwargs.items():
            _type_assert_(key, value, str)
            self.__table[key] = _os_.path.normpath(value)
        self.__path = path


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{{{}}}".format(module, name, repr(self.__path))


    def __contains__(self, name):
        path = _os_.path.normpath(name)
        if _os_.path.isabs(name):
            raise ValueError("name must be a relative path")
        path = _os_.path.join(self.__path, name)
        return _os_.path.exists(path)



    def __getitem__(self, name):
        _type_assert_("name", name, str)
        parts = []
        replaced = False
        path = _os_.path.normpath(name)
        if _os_.path.isabs(path):
            raise ValueError("name must be a relative path")
        for part in path.split(_os_.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced and part in self.__table:
                part = self.__table[part]
                replaced = True
            parts += [part]
        return _os_.path.sep.join([self.__path] + parts)


    @property
    def path(self):
        """directory path"""
        return self.__path



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


    def __init__(self, name, **kwargs):
        path = _os_.path.realpath(name)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        if not _os_.path.isdir(path):
            raise NotADirectoryError(path)
        super().__init__(name, **kwargs)
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



def lookup(name, root, local, patch="patch"):
    """
    Look up a file inside base VFS or local VFS, or combine it using patch.
    If file is available or can be generated via patching, return a readable stream.
    Note that this file may not be the original
    """
    _type_assert_("root", root, Base)
    _type_assert_("local", local, Base)
    _type_assert_("patch", patch, str)
    if name in local:
        return _codecs_.open(local[name], "rb")
    diff = "{}.diff".format(name)
    if diff not in local:
        return _codecs_.open(root[name], "rb")
    tmp = _tempfile_.NamedTemporaryFile(mode="w+b", delete=False)
    with _codecs_.open(root[name], "rb") as stream:
        _shutil_.copyfileobj(stream, tmp)
        tmp.close()
    stdin = _codecs_.open(local[diff], "rb")
    cmd = (patch, "-s", tmp.name)
    pipes = _sp_.Popen(cmd, stdin=stdin, stdout=_sp_.PIPE, stderr=_sp_.PIPE)
    (stdout, stderr) = pipes.communicate()
    stdout = stdout.decode("UTF-8")
    stderr = stderr.decode("UTF-8")
    returncode = pipes.returncode
    if returncode != 0:
        cmd = "patch -s {} < {}".format(tmp.name, local[diff])
        raise _sp_.CalledProcessError(returncode, cmd, stdout, stderr)
    stream = _codecs_.open(tmp.name, "rb")
    _os_.unlink(tmp.name)
    return stream
