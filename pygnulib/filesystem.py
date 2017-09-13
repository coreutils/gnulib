#!/usr/bin/python
# encoding: UTF-8



import os as _os_


from .error import type_assert as _type_assert_
from .config import Base as _BaseConfig_
from .module import Base as _BaseModule_
from .module import File as _FileModule_



class Directory:
    """gnulib generic virtual file system"""
    _TABLE_ = {
        "lib"       : "source_base",
        "doc"       : "doc_base",
        "m4"        : "m4_base",
        "tests"     : "tests_base",
        "tests=lib" : "tests_base",
        "po"        : "po_base",
        "build-aux" : "aux_dir",
    }


    def __init__(self, name, config):
        _type_assert_("name", name, str)
        _type_assert_("config", config, _BaseConfig_)
        path = _os_.path.realpath(name)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        if not _os_.path.isdir(path):
            raise NotADirectoryError(path)
        self.__config = config
        self.__name = name
        self.__path = path


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "%s.%s{%r}" % (module, name, self.name)


    def __getitem__(self, name):
        """retrieve the canonical path of the specified file name"""
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
            if not replaced:
                for old, new in Directory._TABLE_.items():
                    if part == old:
                        part = self.__config[new]
                        replaced = True
            parts += [part]
        path = _os_.path.sep.join([self.__path] + parts)
        if not _os_.path.exists(path):
            raise FileNotFoundError(name)
        return path


    @property
    def name(self):
        return self.__name


    @property
    def path(self):
        """root directory path"""
        return self.__path



class GnulibGit(Directory):
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


    def __init__(self, name):
        path = _os_.path.realpath(name)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        if not _os_.path.isdir(path):
            raise NotADirectoryError(path)
        config = _BaseConfig_(root=path)
        super().__init__(name, config)
        if not _os_.path.isdir(_os_.path.join(self.path, ".git")):
            raise TypeError("%r is not a gnulib repository")


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def module(self, name, full=True):
        """instantiate gnulib module by its name"""
        _type_assert_("name", name, str)
        _type_assert_("full", full, bool)
        if name in GnulibGit._EXCLUDE_:
            raise ValueError("illegal module name")
        path = _os_.path.join(self["modules"], name)
        return _FileModule_(path, name=name) if full else _BaseModule_(name)


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
