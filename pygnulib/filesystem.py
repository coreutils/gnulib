#!/usr/bin/python
# encoding: UTF-8



import os

from .config import Config
from .module import Module
from .module import FileModule



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
        if not isinstance(root, str):
            raise TypeError("root must be of 'str' type")
        if not isinstance(config, Config):
            raise TypeError("config must be of 'Config' type")
        if not os.path.exists(root):
            raise FileNotFoundError(root)
        if not os.path.isdir(root):
            raise NotADirectoryError(root)
        self._config_ = config
        self._root_ = os.path.realpath(root)


    def __getitem__(self, name):
        """retrieve the canonical path of the specified file name"""
        parts = []
        replaced = False
        if not isinstance(name, str):
            raise TypeError("name must be of 'str' type")
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
                        part = self._config_[new]
                        replaced = True
            parts += [part]
        path = os.path.sep.join([self._root_] + parts)
        if not os.path.exists(path):
            raise FileNotFoundError(name)
        return path



class Git(Directory):
    """gnulib Git-based virtual file system"""
    _EXCLUDE_ = {
        "."                 : str.startswith,
        "~"                 : str.endswith,
        "-tests"            : str.endswith,
        "CVS"               : str.startswith,
        "ChangeLog"         : str.__eq__,
        "COPYING"           : str.__eq__,
        "README"            : str.__eq__,
        "TEMPLATE"          : str.__eq__,
        "TEMPLATE-TESTS"    : str.__eq__,
        "TEMPLATE-EXTENDED" : str.__eq__,
    }


    def __init__(self, root, config):
        if not os.path.isdir(root):
            raise FileNotFoundError(root)
        if not os.path.isdir(os.path.join(root, ".git")):
            raise TypeError("%r is not a gnulib repository")
        super().__init__(root, config)


    def module(self, name, full=True):
        """instantiate gnulib module by its name"""
        if name in Git._EXCLUDE_:
            raise ValueError("illegal module name")
        path = os.path.join(self["modules"], name)
        return FileModule(path, name=name) if full else Module(name)


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
