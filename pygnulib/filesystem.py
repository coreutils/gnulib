#!/usr/bin/python
# encoding: UTF-8



import os

from .error import type_assert as _type_assert_
from .config import Base as _BaseConfig_
from .module import Base as _BaseModule_
from .module import File as _FileModule_



class Directory:
    """gnulib generic virtual file system"""
    _SUBST_ = {
        "build-aux" : "aux-dir",
        "doc"       : "doc-base",
        "lib"       : "source-base",
        "m4"        : "m4-base",
        "tests"     : "tests-base",
        "tests=lib" : "tests-base",
        "po"        : "po-base",
    }


    def __init__(self, root, config):
        _type_assert_("root", root, str)
        _type_assert_("config", config, _BaseConfig_)
        if not os.path.exists(root):
            raise FileNotFoundError(root)
        if not os.path.isdir(root):
            raise NotADirectoryError(root)
        self.__config = config
        self.__root = os.path.realpath(root)


    def __getitem__(self, name):
        """retrieve the canonical path of the specified file name"""
        _type_assert_("name", name, str)
        parts = []
        replaced = False
        path = os.path.normpath(name)
        if os.path.isabs(path):
            raise ValueError("name must be a relative path")
        for part in path.split(os.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced:
                for old, new in Directory._SUBST_.items():
                    if part == old:
                        part = self.__config[new]
                        replaced = True
            parts += [part]
        path = os.path.sep.join([self.__root] + parts)
        if not os.path.exists(path):
            raise FileNotFoundError(name)
        return path


    @property
    def root(self):
        """root directory path"""
        return self.__root



class Git(Directory):
    """gnulib Git-based virtual file system"""
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


    def __init__(self, root, config):
        super().__init__(root, config)
        if not os.path.isdir(os.path.join(self.root, ".git")):
            raise TypeError("%r is not a gnulib repository")


    def module(self, name, full=True):
        """instantiate gnulib module by its name"""
        _type_assert_("name", name, str)
        _type_assert_("full", full, bool)
        if name in Git._EXCLUDE_:
            raise ValueError("illegal module name")
        path = os.path.join(self["modules"], name)
        return _FileModule_(path, name=name) if full else _BaseModule_(name)


    def modules(self, full=True):
        """iterate over all available modules"""
        prefix = self["modules"]
        for root, _, files in os.walk(prefix):
            names = []
            for name in files:
                exclude = False
                for key, method in Git._EXCLUDE_.items():
                    if method(name, key):
                        exclude = True
                        break
                if not exclude:
                    names += [name]
            for name in names:
                path = os.path.join(root, name)
                name = path[len(prefix) + 1:]
                yield self.module(name, full)
