#!/usr/bin/python
# encoding: UTF-8


import os


from .config import Config
from .module import FileModule


class Project:
    """gnulib project directory"""
    _SUBST_ = {
        "build-aux" : "aux-dir",
        "doc"       : "doc-base",
        "lib"       : "source-base",
        "m4"        : "m4-base",
        "tests"     : "tests-base",
        "tests=lib" : "tests-base",
        "po"        : "po-base",
    }

    def __init__(self, root, **kwargs):
        if not isinstance(root, str):
            raise TypeError("root must be of 'str' type")
        if not os.path.exists(root):
            raise FileNotFoundError(root)
        if not os.path.isdir(root):
            raise NotADirectoryError(root)
        self._root_ = root
        self._config_ = Config(**kwargs)


    @property
    def root(self):
        return self._root_


    def path(self, name):
        parts = []
        replaced = False
        if not isinstance(name, str):
            raise TypeError("src must be of 'str' type")
        name = os.path.normpath(name)
        if os.path.isabs(name):
            raise ValueError("name must be a relative path")
        for part in name.split(os.path.sep):
            if part == "..":
                parts += [part]
                continue
            if not replaced:
                for old, new in Project._SUBST_.items():
                    if part == old:
                        part = self._config_[new]
                        replaced = True
            parts += [part]
        return os.path.sep.join(parts)


    def exists(self, name):
        path = self.path(name)
        return os.path.exists(path)


    def stat(self, name):
        return os.stat(self.path(name))



class GitRepository(Project):
    """gnulib Git repository"""
    _EXCLUDE_ = {
        "CVS",
        "ChangeLog",
        "COPYING",
        "README",
        "TEMPLATE",
        "TEMPLATE-EXTENDED",
    }


    def __init__(self, root, **kwargs):
        if not os.path.isdir(root):
            raise FileNotFoundError(root)
        if not os.path.isdir(os.path.join(root, ".git")):
            raise TypeError("%r is not a gnulib repository")
        super(GitRepository, self).__init__(root, **kwargs)


    def module(self, name):
        if name in GitRepository._EXCLUDE_:
            raise KeyError("module does not exist")
        path = os.path.join(self.root, "modules", name)
        return FileModule(path, name=name)


    def modules(self):
        for root, _, files in os.walk(os.path.join(self.root, "modules")):
            for name in [_ for _ in files if _ not in GitRepository._EXCLUDE_]:
                path = os.path.join(root, name)
                yield FileModule(path, name=name)
