#!/usr/bin/python
# encoding: UTF-8
"""gnulib virtual filesystem API"""



import os as _os_


from .error import type_assert as _type_assert_
from .error import GnulibModuleNotFoundError as _GnulibModuleNotFoundError_
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
        """directory name"""
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
        """obtain gnulib module by name"""
        _type_assert_("name", name, str)
        _type_assert_("full", full, bool)
        if name in GnulibGit._EXCLUDE_:
            raise ValueError("illegal module name")
        path = _os_.path.join(self["modules"], name)
        try:
            return _FileModule_(path, name=name) if full else _BaseModule_(name)
        except FileNotFoundError:
            raise _GnulibModuleNotFoundError_(name)


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


    def transitive_closure(self, config, modules, tests):
        """
        GnulibGit.transitive_closure(config, modules, tests) ->
            set((
                (module0, demander0, condition0),
                (moduleN, demanderN, conditionN),
            ))

        config is a gnulib configuration (e.g. pygnulib.Config.Base instance).
        modules is an iterable, yielding a module (either name or instance).
        tests designates whether to consider the corresponding test module and its dependencies.

        The returned value is a set, containing a sequence of tuples (module, demander, condition).
        If condition is None, but demander is not, there is no special condition for this module.
        If demander is None, the module is provided unconditionally (condition is always None).
        The correct iteration over the resulting table is shown below:

        for (module, demander, condition) in gnulib.transitive_closure(modules):
            if demander is not None:
                if condition is not None:
                    fmt = "{0} is added as dependency from {1} under '{2}' condition"
                    args = (module.name, demander.name, condition)
                else:
                    fmt = "{0} is added as dependency from {1} without specific condition"
                    args = (module.name, demander.name)
            else:
                fmt = "{0} is added unconditionally"
                args = (module.name,)
            print(fmt.format(args))
        """
        demanders = set()
        (old, new) = (set(), set())
        for module in modules:
            if isinstance(module, str):
                module = self.module(module)
            new.add((module, None, None))
        while old != new:
            old.update(new)
            for (demander, _, _) in old:
                if demander in demanders:
                    continue
                if tests:
                    try:
                        name = "{0}-tests".format(demander.name)
                        new.add((self.module(name), None, None))
                    except _GnulibModuleNotFoundError_:
                        pass # ignore non-existent tests
                for (dependency, condition) in demander.dependencies:
                    module = self.module(dependency)
                    exclude = (
                        (not config.obsolete and module.obsolete),
                        (not config.cxx_tests and module.cxx_test),
                        (not config.longrunning_tests and module.longrunning_test),
                        (not config.privileged_tests and module.privileged_test),
                        (not config.unportable_tests and module.unportable_test),
                    )
                    if any(exclude):
                        continue
                    condition = condition if condition.strip() else None
                    new.add((module, demander, condition))
                demanders.add(demander)
        return new
