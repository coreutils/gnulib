#!/usr/bin/python
# encoding: UTF-8
"""gnulib virtual filesystem API"""



import codecs as _codecs
import filecmp as _filecmp
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
        self.__prefix = prefix


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{{{}}}".format(module, name, repr(self.__prefix))


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def __contains__(self, name):
        path = _os.path.normpath(name)
        if _os.path.isabs(name):
            raise ValueError("name must be a relative path")
        path = _os.path.join(self.absolute, self[name])
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
        path = _os.path.sep.join(parts)
        return _os.path.normpath(path)


    @property
    def relative(self):
        """base VFS name"""
        return self.__prefix


    @property
    def absolute(self):
        """absolute VFS path"""
        return _os.path.abspath(self.__prefix)



def lookup(name, primary, secondary, patch="patch"):
    """
    Try to look up a regular file inside virtual file systems or combine it via patch utility.
    The name argument is a relative file name which is going to be looked up.
    The primary argument is the primary virtual file system to search for the file.
    The secondary argument is the secondary virtual file system to search for the file.
    The patch argument must be a path to the 'patch' utility binary.

    - file is present only inside the primary VFS: open the file inside the primary VFS.
    - file is present inside the secondary VFS: open the file inside the secondary VFS.
    - both file and patch are present: combine in memory.
    - file is not present: raise an FileNotFoundError exception.

    The function returns a pair of values, representing the file path and state.
    The first element, path, represents a regular file system path.
    The second element, vfs, is either the primary or the secondary VFS.
    If the file was obtained via dynamic patching, the vfs element is None.

    NOTE: It is up to the caller to unlink files obtained after dynamic patching.
    """
    _type_assert("primary", primary, Base)
    _type_assert("secondary", secondary, Base)
    if name in secondary:
        return (secondary, name)
    diff = "{}.diff".format(name)
    if diff not in secondary:
        return (primary, name)
    tmp = _tempfile.NamedTemporaryFile(mode="w+b", delete=False)
    with _codecs.open(primary[name], "rb") as stream:
        _shutil.copyfileobj(stream, tmp)
        tmp.close()
    stdin = _codecs.open(secondary[diff], "rb")
    cmd = (patch, "-s", tmp.name)
    pipes = _sp.Popen(cmd, stdin=stdin, stdout=_sp.PIPE, stderr=_sp.PIPE)
    (stdout, stderr) = pipes.communicate()
    stdout = stdout.decode("UTF-8")
    stderr = stderr.decode("UTF-8")
    returncode = pipes.returncode
    if returncode != 0:
        cmd = "patch -s {} < {}".format(tmp.name, secondary[diff])
        raise _sp.CalledProcessError(returncode, cmd, stdout, stderr)
    return (None, tmp.name)


def mkdir(root, name):
    """Create a leaf directory and all intermediate ones recursively."""
    root = Base(".") if root is None else root
    _os.makedirs(root[name], exist_ok=True)


def backup(root, name):
    """Backup the given file."""
    root = Base(".") if root is None else root
    path = _os.path.join(root.absolute, root[name])
    backup = "{}~".format(path)
    try:
        _os.unlink(backup)
    except FileNotFoundError:
        pass # ignore non-existent files
    _os.rename(path, backup)


def compare(lhs_root, lhs_name, rhs_root, rhs_name):
    """Compare the given files; return True if files contain the same data."""
    lhs_root = Base(".") if lhs_root is None else lhs_root
    rhs_root = Base(".") if rhs_root is None else rhs_root
    lhs_path = _os.path.join(lhs_root.absolute, lhs_root[lhs_name])
    rhs_path = _os.path.join(rhs_root.absolute, rhs_root[rhs_name])
    return _filecmp.cmp(lhs_path, rhs_path, shallow=False)


def copy(src_root, src_name, dst_root, dst_name):
    """Copy file data."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    limit = (16 * 1024)
    src_root = Base(".") if src_root is None else src_root
    dst_root = Base(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    src_path = _os.path.join(src_root.absolute, src_root[src_name])
    dst_path = _os.path.join(dst_root.absolute, dst_root[dst_name])
    with _codecs.open(src_path, "rb") as istream:
        with _codecs.open(dst_path, "wb") as ostream:
            while 1:
                data = istream.read(limit)
                if not data:
                    break
                ostream.write(data)


def exists(root, name):
    """Check whether the given file exists."""
    root = Base(".") if root is None else root
    path = _os.path.join(root.absolute, root[name])
    return _os.path.exists(path)


def hardlink(src_root, src_name, dst_root, dst_name):
    """Create a hard link to the file."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = Base(".") if src_root is None else src_root
    dst_root = Base(".") if dst_root is None else dst_root
    mkdir(src_root, _os.path.dirname(src_name))
    mkdir(dst_root, _os.path.dirname(dst_name))
    src_path = _os.path.join(src_root.absolute, src_root[src_name])
    dst_path = _os.path.join(dst_root.absolute, dst_root[dst_name])
    _os.link(src_path, dst_path)


def move(src_root, src_name, dst_root, dst_name):
    """Move file data."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = Base(".") if src_root is None else src_root
    dst_root = Base(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    src_path = _os.path.join(src_root.absolute, src_root[src_name])
    dst_path = _os.path.join(dst_root.absolute, dst_root[dst_name])
    _os.rename(src_path, dst_path)


def readlink(root, name):
    """Obtain the path to which the symbolic link points."""
    root = Base(".") if root is None else root
    mkdir(root, _os.path.dirname(name))
    path = _os.path.join(root.absolute, root[name])
    return _os.readlink(path)


def symlink(src_root, src_name, dst_root, dst_name, relative=True):
    """Create a symbolic link to the file."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = Base(".") if src_root is None else src_root
    dst_root = Base(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    if not relative:
        src_path = _os.path.join(src_root.absolute, src_root[src_name])
        dst_path = _os.path.join(dst_root.absolute, dst_root[dst_name])
    else:
        src_path = _os.path.join(src_root.relative, src_root[src_name])
        dst_path = _os.path.join(dst_root.relative, dst_root[dst_name])
        prefix = _os.path.relpath(_os.path.dirname(src_path), _os.path.dirname(dst_path))
        suffix = _os.path.basename(src_root[src_name])
        src_path = _os.path.join(prefix, suffix)
        dst_path = _os.path.join(dst_root.absolute, dst_root[dst_name])
    _os.symlink(src_path, dst_path)


def unlink(root, name, backup=True):
    """Unlink a file, backing it up if necessary."""
    root = Base(".") if root is None else root
    mkdir(root, _os.path.dirname(name))
    path = _os.path.join(root.absolute, root[name])
    _os.unlink(path)



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
        if not _os.path.exists(self.absolute):
            raise FileNotFoundError(self.absolute)
        if not _os.path.isdir(self.absolute):
            raise NotADirectoryError(self.absolute)
        if not _os.path.isdir(_os.path.join(self.absolute, ".git")):
            raise TypeError("{} is not a gnulib repository".format(prefix))


    def module(self, name, full=True):
        """obtain gnulib module by name"""
        _type_assert("name", name, str)
        _type_assert("full", full, bool)
        if name in GnulibGit._EXCLUDE:
            raise ValueError("illegal module name")
        path = _os.path.join(self.absolute, self["modules"], name)
        try:
            return _FileModule(path, name=name) if full else _BaseModule(name)
        except FileNotFoundError:
            raise _UnknownModuleError(name)


    def modules(self, full=True):
        """iterate over all available modules"""
        prefix = _os.path.join(self.absolute, self["modules"])
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
