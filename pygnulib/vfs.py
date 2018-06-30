#!/usr/bin/python
# encoding: UTF-8
"""gnulib virtual filesystem API"""



import codecs as _codecs
import filecmp as _filecmp
import os as _os
import shutil as _shutil
import sys as _sys
import tempfile as _tempfile
import subprocess as _sp


from .error import UnknownModuleError as _UnknownModuleError
from .module import DummyModule as _DummyModule
from .module import GnulibModule as _GnulibModule
from .misc import Executable as _Executable



class BaseVFS:
    """gnulib generic virtual file system"""
    __slots__ = ("__origin", "__root", "__table")


    def __init__(self, origin, **table):
        self.__table = {}
        for (key, value) in table.items():
            self.__table[key] = _os.path.normpath(value)
        self.__origin = _os.path.normpath(origin)
        self.__root = _os.path.abspath(origin)


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{{{}}}".format(module, name, repr(self.__origin))


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def __contains__(self, name):
        if _os.path.isabs(name):
            raise ValueError("name must be a relative path")
        path = _os.path.join(self.__root, self[name])
        return _os.path.exists(path)


    def __getitem__(self, name):
        parts = []
        replaced = False
        name = _os.path.normpath(name)
        if _os.path.isabs(name):
            raise ValueError("name cannot be an absolute path")
        for part in name.split(_os.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced and part in self.__table:
                part = self.__table[part]
                replaced = True
            parts += [part]
        return _os.path.normpath(_os.path.sep.join(parts))


    def __setitem__(self, src, dst):
        for name in (src, dst):
            if _os.path.isabs(name):
                raise ValueError("name cannot be an absoule path")
        src = _os.path.normpath(src)
        dst = _os.path.normpath(dst)
        self.__table[src] = dst


    @property
    def origin(self):
        """origin VFS path"""
        return self.__origin


    @property
    def root(self):
        """absolute VFS path"""
        return self.__root



def lookup(name, primary, secondary, patch):
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
    if not isinstance(name, str):
        raise TypeError("name: str expected")
    if not isinstance(primary, BaseVFS):
        raise TypeError("primary: VFS expected")
    if not isinstance(secondary, BaseVFS):
        raise TypeError("secondary: VFS expected")
    if not isinstance(patch, _Executable):
        raise TypeError("patch: executable expected")

    if name in secondary:
        return (secondary, name)
    diff = f"{name}.diff"
    if diff not in secondary:
        return (primary, name)
    with _codecs.open(primary[name], "rb") as istream:
        with _tempfile.NamedTemporaryFile(mode="w+b", delete=False) as ostream:
            path = ostream.name
            _shutil.copyfileobj(istream, ostream)
    stdin = _codecs.open(secondary[diff], "rb")
    cmd = (patch, "-s", path)
    pipes = _sp.Popen(cmd, stdin=stdin, stdout=_sp.PIPE, stderr=_sp.PIPE)
    (stdout, stderr) = pipes.communicate()
    stdout = stdout.decode("UTF-8")
    stderr = stderr.decode("UTF-8")
    returncode = pipes.returncode
    if returncode != 0:
        cmd = "patch -s {} < {}".format(path, secondary[diff])
        raise _sp.CalledProcessError(returncode, cmd, stdout, stderr)
    return (None, path)


def mkdir(root, name):
    """Create a leaf directory and all intermediate ones recursively."""
    root = BaseVFS(".") if root is None else root
    path = name if _os.path.isabs(name) else _os.path.join(root.root, root[name])
    _os.makedirs(root[name], exist_ok=True)


def backup(root, name):
    """Backup the given file."""
    root = BaseVFS(".") if root is None else root
    original_path = _os.path.join(root.root, root[name])
    backup_path = "{}~".format(original_path)
    try:
        _os.unlink(backup_path)
    except FileNotFoundError:
        pass # ignore non-existent files
    _os.rename(original_path, backup_path)


def compare(lhs_root, lhs_name, rhs_root, rhs_name):
    """Compare the given files; return True if files contain the same data."""
    lhs_root = BaseVFS(".") if lhs_root is None else lhs_root
    rhs_root = BaseVFS(".") if rhs_root is None else rhs_root
    (lhs_path, rhs_path) = (lhs_name, rhs_name)
    if not _os.path.isabs(lhs_name):
        lhs_path = _os.path.join(lhs_root.root, lhs_root[lhs_name])
    if not _os.path.isabs(rhs_name):
        rhs_path = _os.path.join(rhs_root.root, rhs_root[rhs_name])
    return _filecmp.cmp(lhs_path, rhs_path, shallow=False)


def copy(src_root, src_name, dst_root, dst_name):
    """Copy file data."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    limit = (16 * 1024)
    src_root = BaseVFS(".") if src_root is None else src_root
    dst_root = BaseVFS(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    (src_path, dst_path) = (src_name, dst_name)
    if not _os.path.isabs(src_name):
        src_path = _os.path.join(src_root.root, src_root[src_name])
    if not _os.path.isabs(dst_name):
        dst_path = _os.path.join(dst_root.root, dst_root[dst_name])
    with _codecs.open(src_path, "rb") as istream:
        with _codecs.open(dst_path, "wb") as ostream:
            data = True
            while data:
                data = istream.read(limit)
                ostream.write(data)


def exists(root, name):
    """Check whether the given file exists."""
    root = BaseVFS(".") if root is None else root
    path = name if _os.path.isabs(name) else _os.path.join(root.root, root[name])
    return _os.path.exists(path)


def hardlink(src_root, src_name, dst_root, dst_name):
    """Create a hard link to the file."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = BaseVFS(".") if src_root is None else src_root
    dst_root = BaseVFS(".") if dst_root is None else dst_root
    mkdir(src_root, _os.path.dirname(src_name))
    mkdir(dst_root, _os.path.dirname(dst_name))
    (src_path, dst_path) = (src_name, dst_name)
    if not _os.path.isabs(src_name):
        src_path = _os.path.join(src_root.root, src_root[src_name])
    if not _os.path.isabs(dst_name):
        dst_path = _os.path.join(dst_root.root, dst_root[dst_name])
    _os.link(src_path, dst_path)


def move(src_root, src_name, dst_root, dst_name):
    """Move file data."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = BaseVFS(".") if src_root is None else src_root
    dst_root = BaseVFS(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    (src_path, dst_path) = (src_name, dst_name)
    if not _os.path.isabs(src_name):
        src_path = _os.path.join(src_root.root, src_root[src_name])
    if not _os.path.isabs(dst_name):
        dst_path = _os.path.join(dst_root.root, dst_root[dst_name])
    _os.rename(src_path, dst_path)


def iostream(root, name, mode="r", encoding=None):
    """Open file and return a stream. Raise IOError upon failure."""
    root = BaseVFS(".") if root is None else root
    path = name if _os.path.isabs(name) else _os.path.join(root.root, root[name])
    return _codecs.open(path, mode, encoding)


def readlink(root, name):
    """Obtain the path to which the symbolic link points."""
    root = BaseVFS(".") if root is None else root
    mkdir(root, _os.path.dirname(name))
    path = name if _os.path.isabs(name) else _os.path.join(root.root, root[name])
    return _os.readlink(path)


def symlink(src_root, src_name, dst_root, dst_name, relative=True):
    """Create a symbolic link to the file."""
    src_abs = _os.path.isabs(src_name)
    dst_abs = _os.path.isabs(dst_name)
    if src_abs and dst_abs:
        raise ValueError("absolute src and dst")
    src_root = BaseVFS(".") if src_root is None else src_root
    dst_root = BaseVFS(".") if dst_root is None else dst_root
    mkdir(dst_root, _os.path.dirname(dst_name))
    if not relative:
        (src_path, dst_path) = (src_name, dst_name)
        if not _os.path.isabs(src_name):
            src_path = _os.path.join(src_root.root, src_root[src_name])
        if not _os.path.isabs(dst_name):
            dst_path = _os.path.join(dst_root.root, dst_root[dst_name])
    else:
        src_path = _os.path.join(src_root.origin, src_root[src_name])
        dst_path = _os.path.join(dst_root.origin, dst_root[dst_name])
        prefix = _os.path.relpath(_os.path.dirname(src_path), _os.path.dirname(dst_path))
        suffix = _os.path.basename(src_root[src_name])
        src_path = _os.path.join(prefix, suffix)
        dst_path = _os.path.join(dst_root.root, dst_root[dst_name])
    _os.symlink(src_path, dst_path)


def unlink(root, name):
    """Unlink a file, backing it up if necessary."""
    root = BaseVFS(".") if root is None else root
    mkdir(root, _os.path.dirname(name))
    path = name if _os.path.isabs(name) else _os.path.join(root.root, root[name])
    _os.unlink(path)



class GnulibGitVFS(BaseVFS):
    """gnulib git repository"""
    __slots__ = ("__cache", "__prefix")


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


    def __init__(self, origin):
        super().__init__(origin=origin)
        self.__cache = {"dummy": _DummyModule()}
        self.__prefix = _sys.intern(_os.path.join(self.root, "modules"))
        if not _os.path.exists(self.root):
            raise FileNotFoundError(self.root)
        if not _os.path.isdir(self.root):
            raise NotADirectoryError(self.root)
        if not _os.path.isdir(_os.path.join(self.root, ".git")):
            raise TypeError("{} is not a gnulib repository".format(prefix))


    def module(self, name):
        """Try to find the module by name."""
        if name in self.__cache:
            return self.__cache[name]
        path = _os.path.join(self.__prefix, name)
        try:
            result = self.__cache[name] = _GnulibModule(path=path, name=name)
            return result
        except _UnknownModuleError:
            return None


    def modules(self):
        """Iterate over all available modules."""
        for root, _, files in _os.walk(self.__prefix):
            names = []
            for name in files:
                exclude = False
                for (key, method) in GnulibGitVFS._EXCLUDE.items():
                    if method(name, key):
                        exclude = True
                        break
                if not exclude:
                    names += [name]
            for name in names:
                path = _os.path.join(root, name)
                name = path[len(self.__prefix) + 1:]
                yield self.module(name)
