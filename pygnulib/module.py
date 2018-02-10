#!/usr/bin/python
# encoding: UTF-8
"""gnulib module API"""



import ast as _ast
import codecs as _codecs
import collections as _collections
import hashlib as _hashlib
import json as _json
import os as _os
import re as _re
import sys as _sys


from .error import UnknownModuleError as _UnknownModuleError
from .config import BaseConfig as _BaseConfig
from .misc import Property as _Property
from .misc import PathProperty as _PathProperty
from .misc import StringListProperty as _StringListProperty
from .misc import PathListProperty as _PathListProperty



class BaseModule:
    """base module"""
    __slots__ = ("__options", "__flags")


    __PROPERTIES = {
        "name"                           : None,
        "description"                    : "",
        "comment"                        : "",
        "status"                         : "",
        "notice"                         : "",
        "applicability"                  : "",
        "files"                          : "",
        "dependencies"                   : "",
        "early_autoconf_snippet"         : "",
        "autoconf_snippet"               : "",
        "conditional_automake_snippet"   : "",
        "unconditional_automake_snippet" : None,
        "automake_snippet"               : None,
        "include_directives"             : "",
        "link_directives"                : "",
        "licenses"                       : "",
        "maintainers"                    : "",
        "test"                           : "",
    }
    __OPTIONS = {
        "name",
        "description",
        "comment",
        "status",
        "notice",
        "applicability",
        "files",
        "dependencies",
        "early_autoconf_snippet",
        "autoconf_snippet",
        "conditional_automake_snippet",
        "unconditional_automake_snippet",
        "automake_snippet",
        "include_directives",
        "link_directives",
        "licenses",
        "maintainers",
        "test",
    }
    __FLAGS = {}
    __LIB_SOURCES = _re.compile(r"^lib_SOURCES\s*\+\=\s*(.*?)$", _re.S | _re.M)


    def __init__(self, name, **kwargs):
        if not isinstance(name, str):
            raise TypeError("name: str expected")
        if not name.strip():
            raise ValueError("name: invalid name")

        self.__flags = 0
        self.__options = {}
        for key in BaseModule.__OPTIONS:
            value = BaseModule.__PROPERTIES[key]
            if value is not None:
                self.__set_option(key, value)
        for key in BaseModule.__FLAGS:
            state = BaseModule.__PROPERTIES[key]
            mask = getattr(self.__class__, key).mask
            self.__set_flags(mask, state)
        self.__set_option("name", name)

        for (key, value) in kwargs.items():
            setattr(self, key, value)


    def __enter__(self):
        return self


    def __hash__(self):
        return hash(_json.dumps(dict(self.__options), sort_keys=True))


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return f"{module}.{name}[{self.name}]"


    def __getitem__(self, key):
        if key not in BaseModule.__PROPERTIES:
            key = key.replace("-", "_")
            if key not in BaseModule.__PROPERTIES:
                raise KeyError(repr(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        if key not in BaseModule.__PROPERTIES:
            key = key.replace("-", "_")
            if key not in BaseModule.__PROPERTIES:
                raise KeyError(repr(key))
        return setattr(self, key, value)


    def __get_option(self, key):
        return self.__options[key]

    def __set_option(self, key, value):
        self.__options[key] = value


    @property
    def gnulib_package(self):
        """gnulib-compatible module textual representation"""
        def _gnulib():
            yield "Description:"
            yield self.description
            yield "Comment:"
            yield self.comment
            yield "Status:"
            for status in sorted(self.status):
                yield status
            yield "Notice:"
            yield self.notice
            yield "Applicability:"
            yield self.applicability
            yield "Files:"
            for file in self.files:
                yield file
            yield "Depends-on:"
            for (module, condition) in self.dependencies:
                if condition:
                    yield f"{module}    {condition}"
                else:
                    yield f"{module}"
            yield "configure.ac-early:"
            yield self.early_autoconf_snippet
            yield "configure.ac:"
            yield self.autoconf_snippet
            yield "Makefile.am:"
            yield self.conditional_automake_snippet
            yield "Include:"
            for include in self.include_directives:
                yield include
            yield "Link:"
            for link in self.link_directives:
                yield link
            yield "License:"
            for license in self.licenses:
                yield license
            yield "Maintainer:"
            for maintainer in maintainers:
                yield maintainer
        return "\n".join(_gnulib())


    name = _Property(
        fget=lambda self: self.__get_option("name"),
        fset=lambda self, string: self.__set_option("name", string),
        check=lambda value: isinstance(value, str) and value,
        doc="name",
    )
    description = _Property(
        fget=lambda self: self.__get_option("description"),
        fset=lambda self, string: self.__set_option("description", string),
        check=lambda value: isinstance(value, str),
        doc="description",
    )
    comment = _Property(
        fget=lambda self: self.__get_option("comment"),
        fset=lambda self, string: self.__set_option("comment", string),
        check=lambda value: isinstance(value, str),
        doc="comment",
    )
    status = _StringListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("comment"),
        fset=lambda self, string: self.__set_option("comment", string),
        doc="status list",
    )
    obsolete = _Property(
        fget=lambda self: "obsolete" in self.status,
        doc="module is obsolete?",
    )
    cxx_test = _Property(
        fget=lambda self: "c++-test" in self.status,
        doc="module is a C++ test?",
    )
    longrunning_test = _Property(
        fget=lambda self: "longrunning-test" in self.status,
        doc="module is a longrunning test?",
    )
    privileged_test = _Property(
        fget=lambda self: "privileged-test" in self.status,
        doc="module is a privileged test?",
    )
    unportable_test = _Property(
        fget=lambda self: "unportable-test" in self.status,
        doc="module is an unportable test?",
    )
    mask = _Property(
        fget=lambda self: ((0, (1 << 0))[self.obsolete]
                          |(0, (1 << 1))[self.cxx_test]
                          |(0, (1 << 2))[self.longrunning_test]
                          |(0, (1 << 3))[self.privileged_test]
                          |(0, (1 << 4))[self.unportable_test]),
        doc="module acceptibility mask",
    )
    notice = _Property(
        fget=lambda self: self.__get_option("notice"),
        fset=lambda self, string: self.__set_option("notice", string),
        check=lambda value: isinstance(value, str),
        doc="module notice or disclaimer",
    )
    applicability = _Property(
        fget=lambda self: self.__get_option("applicability"),
        fset=lambda self, string: self.__set_option("applicability", string),
        check=lambda value: isinstance(value, str) and value in {"main", "tests", "all"},
        doc="applicability ('main', 'tests' or 'all')",
    )
    files = _PathListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("files"),
        fset=lambda self, string: self.__set_option("files", string),
        doc="file dependencies",
    )
    early_autoconf_snippet = _Property(
        fget=lambda self: self.__get_option("early_autoconf_snippet"),
        fset=lambda self, string: self.__set_option("early_autoconf_snippet", string),
        check=lambda value: isinstance(value, str),
        doc="early configure.ac snippet",
    )
    autoconf_snippet = _Property(
        fget=lambda self: self.__get_option("autoconf_snippet"),
        fset=lambda self, string: self.__set_option("autoconf_snippet", string),
        check=lambda value: isinstance(value, str),
        doc="configure.ac snippet",
    )
    conditional_automake_snippet = _Property(
        fget=lambda self: self.__get_option("conditional_automake_snippet"),
        fset=lambda self, string: self.__set_option("conditional_automake_snippet", string),
        check=lambda value: isinstance(value, str),
        doc="configure.ac snippet",
    )
    automake_snippet = _Property(
        fget=lambda self: "\n".join((self.conditional_automake_snippet, self.unconditional_automake_snippet)),
        doc="full automake snippet (conditional + unconditional parts)",
    )
    include_directives = _StringListProperty(
        fget=lambda self: self.__get_option("include_directives"),
        fset=lambda self, string: self.__set_option("include_directives", string),
        doc="include directive",
    )
    link_directives = _StringListProperty(
        fget=lambda self: self.__get_option("link_directives"),
        fset=lambda self, string: self.__set_option("link_directives", string),
        doc="link directive",
    )
    licenses = _StringListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("licenses"),
        fset=lambda self, name: self.__set_option("licenses", name),
        doc="acceptable licenses for modules",
    )
    maintainers = _StringListProperty(
        sorted=False,
        unique=True,
        fget=lambda self: self.__get_option("maintainers"),
        fset=lambda self, name: self.__set_option("maintainers", name),
        doc="module maintainers list",
    )
    test = _Property(
        fget=lambda self: self.__get_option("test"),
        fset=lambda self, name: self.__set_option("test", name),
        check=lambda value: isinstance(value, bool),
        doc="module is a test?",
    )


    @_Property
    def dependencies(self):
        """dependencies iterator (name, condition)"""
        return self.__options["dependencies"]

    @dependencies.setter
    def dependencies(self, value):
        result = []
        types = (list, tuple, set, frozenset, type({}.keys()), type({}.values()))
        if not isinstance(value, types):
            raise TypeError("value: iterable expected")
        for item in value:
            if not isinstance(value, (list, tuple)):
                raise TypeError("item: pair expected")
            (module, condition) = item
            if not isinstance(module, str):
                raise TypeError("module: str expected")
            if condition is not None and not isinstance(condition, str):
                raise TypeError("condition: str or None expected")
            condition = "" if condition is None else condition
            result.append((module, condition))
        self.__options["dependencies"] = tuple(result)


    @_Property
    def unconditional_automake_snippet(self):
        """Makefile.am snippet that must stay outside of Automake conditionals"""
        result = ""
        files = self.files
        if self.test:
            # *-tests module live in tests/, not lib/.
            # Synthesize an EXTRA_DIST augmentation.
            test_files = tuple(file[len("tests/"):] for file in files if file.startswith("tests/"))
            if test_files:
                result += ("EXTRA_DIST += {}".format(" ".join(test_files)) + "\n")
            return result
        snippet = self.conditional_automake_snippet
        lib_SOURCES = False
        lines = list(snippet.splitlines())
        for (index, line) in enumerate(lines):
            if BaseModule.__LIB_SOURCES.findall(line):
                (first, last) = (index, index)
                while line.endswith("\\"):
                    line = lines[last]
                    last += 1
                lines = list(lines)[first:(last + 1)]
                lines[0] = BaseModule.__LIB_SOURCES.sub("\\1", lines[0])
                lib_SOURCES = True
                break
        lines = tuple(lines) if lib_SOURCES else ()
        lines = filter(lambda line: line.strip(), lines)
        lines = (line.replace("\\", "").strip() for line in lines)
        (all_files, mentioned_files) = (files, [])
        for line in lines:
            for file in line.split():
                if file.strip():
                    mentioned_files += [file]
        mentioned_files = tuple(_collections.OrderedDict.fromkeys(mentioned_files))
        lib_files = tuple(file[len("lib/"):] for file in all_files if file.startswith("lib/"))
        extra_files = tuple(file for file in lib_files if file not in mentioned_files)
        if extra_files:
            result += ("EXTRA_DIST += {}".format(" ".join(extra_files)) + "\n")

        # Synthesize also an EXTRA_lib_SOURCES augmentation.
        # This is necessary so that automake can generate the right list of
        # dependency rules.
        # A possible approach would be to use autom4te --trace of the redefined
        # AC_LIBOBJ and AC_REPLACE_FUNCS macros when creating the Makefile.am
        # (use autom4te --trace, not just grep, so that AC_LIBOBJ invocations
        # inside autoconf's built-in macros are not missed).
        # But it's simpler and more robust to do it here, based on the file list.
        # If some .c file exists and is not used with AC_LIBOBJ - for example,
        # a .c file is preprocessed into another .c file for BUILT_SOURCES -,
        # automake will generate a useless dependency; this is harmless.
        if self.name not in {"relocatable-prog-wrapper", "pt_chown"}:
            extra_files = tuple(file for file in extra_files if file.endswith(".c"))
            if extra_files:
                result += ("EXTRA_lib_SOURCES += {}".format(" ".join(sorted(extra_files))) + "\n")

        # Synthesize an EXTRA_DIST augmentation also for the files in build-aux/.
        prefix = "$(top_srcdir)/{auxdir}"
        buildaux_files = (file for file in all_files if file.startswith("build-aux/"))
        buildaux_files = tuple(_os.path.join(prefix, file[len("build-aux/"):]) for file in buildaux_files)
        if buildaux_files:
            result += ("EXTRA_DIST += {}".format(" ".join(sorted(buildaux_files))) + "\n")
        return result


    def shell_variable(self, macro_prefix="gl"):
        """Get the name of the shell variable set to true once m4 macros have been executed."""
        name = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), name)):
            name = (name + "\n").encode("UTF-8")
            name = _hashlib.md5(name).hexdigest()
        return f"{macro_prefix}_gnulib_enabled_{name}"


    def shell_function(self, macro_prefix="gl"):
        """Get the name of the shell function containing the m4 macros."""
        name = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), name)):
            name = (name + "\n").encode("UTF-8")
            name = _hashlib.md5(name).hexdigest()
        return f"func_{macro_prefix}_gnulib_m4code_{name}"


    def conditional_name(self, macro_prefix="gl"):
        """Get the automake conditional name."""
        name = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), name)):
            name = (name + "\n").encode("UTF-8")
            name = _hashlib.md5(name).hexdigest()
        return f"{macro_prefix}_GNULIB_ENABLED_{name}"


    def items(self):
        """a set-like object providing a view on module items"""
        for key in BaseModule.__PROPERTIES:
            yield (key, self[key])


    @classmethod
    def keys(self):
        """a set-like object providing a view on module keys"""
        for key in BaseModule.__PROPERTIES:
            yield key


    def values(self):
        """a set-like object providing a view on module values"""
        for key in BaseModule.__PROPERTIES:
            yield self[key]


    def __lt__(self, value):
        if value is not None:
            return self.name < value.name
        return False

    def __le__(self, value):
        return self.__lt__(value) or self.__eq__(value)

    def __eq__(self, value):
        if value is not None:
            if self.name != value.name:
                return False
            for key in BaseModule.__PROPERTIES:
                if self[key] != value[key]:
                    return False
            return True
        return False

    def __ne__(self, value):
        return not self.__eq__(value)

    def __ge__(self, value):
        return not value.__le__(self)

    def __gt__(self, value):
        return not value.__lt__(self)



class FileModule(BaseModule):
    """text-based module"""
    __slots__ = ("__path")


    __DEPENDENCY = _re.compile(r"(\S+)(?:\s+\[(.*?)\])?$", _re.M)
    __STRING = lambda text: text.strip()
    __MULTILINE = lambda text: tuple(filter(
        lambda line: line.strip() and not line.strip().startswith("#"),
        [line.strip() for line in text.strip().splitlines()],
    ))
    __INCLUDE_DIRECTIVES = lambda text: tuple(filter(
        lambda line: line.strip(),
        [line.strip() for line in text.strip().splitlines()],
    ))
    __DEPENDENCIES = lambda text: FileModule.__DEPENDENCY.findall("\n".join(FileModule.__MULTILINE(text)))
    __MAINTAINERS = lambda text: tuple(filter(
        lambda line: line.strip() and not line.strip().startswith("#"),
        {line.strip() for line in text.split((",", "\n")["\n" in text.strip()])},
    ))
    __TABLE = {
        "Description": (
            "description",
            __STRING,
        ),
        "Comment": (
            "comment",
            __STRING,
        ),
        "Status": (
            "status",
            __MULTILINE,
        ),
        "Notice": (
            "notice",
            __STRING,
        ),
        "Applicability": (
            "applicability",
            __STRING,
        ),
        "Files": (
            "files",
            __MULTILINE,
        ),
        "Depends-on": (
            "dependencies",
            __DEPENDENCIES,
        ),
        "configure.ac-early": (
            "early_autoconf_snippet",
            __STRING,
        ),
        "configure.ac": (
            "autoconf_snippet",
            __STRING,
        ),
        "Makefile.am": (
            "conditional_automake_snippet",
            __STRING,
        ),
        "Include": (
            "include_directives",
            __INCLUDE_DIRECTIVES,
        ),
        "Link": (
            "link_directives",
            __MULTILINE,
        ),
        "License": (
            "licenses",
            __MULTILINE,
        ),
        "Maintainer": (
            "maintainers",
            __MAINTAINERS,
        ),
    }
    __PATTERN = _re.compile("({}):".format("|".join(__TABLE)))


    path = _Property(
        fget=lambda self: self.__path,
        doc="module file path",
    )


    def __init__(self, path, name, **kwargs):
        if not isinstance(path, str):
            raise TypeError("path: str expected")
        if not isinstance(name, str):
            raise TypeError("name: str expected")
        with _codecs.open(path, "rb", "UTF-8") as stream:
            match = FileModule.__PATTERN.split(stream.read())[1:]
        for (group, text) in zip(match[::2], match[1::2]):
            (key, hook) = FileModule.__TABLE[group]
            kwargs.setdefault(key, hook(text))
        super().__init__(name=name, **kwargs)
        self.__path = path



class _DummyModuleMeta(type):
    __INSTANCE = None
    __PROPERTIES = {
        "description": "A dummy file, to make sure the library is non-empty.",
        "comment": "",
        "status": tuple(),
        "notice": "",
        "applicability": "main",
        "files": tuple({"lib/dummy.c"}),
        "dependencies": tuple(),
        "early_autoconf_snippet": "",
        "autoconf_snippet": "",
        "include_directives": "",
        "link_directives": "",
        "licenses": tuple({"public domain"}),
        "maintainers": tuple({"all"}),
        "automake_snippet": "lib_SOURCES += dummy.c",
        "conditional_automake_snippet": "lib_SOURCES += dummy.c",
        "unconditional_automake_snippet": "",
    }


    def __new__(mcs, name, parents, attributes):
        for (key, value) in _DummyModuleMeta.__PROPERTIES.items():
            fget=lambda self, value=value: value
            doc = getattr(BaseModule, key).__doc__
            attributes[key] = _Property(fget=fget, doc=doc)
        return super().__new__(mcs, name, parents, attributes)


    def __call__(cls, *args, **kwargs):
        if _DummyModuleMeta.__INSTANCE is None:
            _DummyModuleMeta.__INSTANCE = super().__call__(*args, **kwargs)
        return _DummyModuleMeta.__INSTANCE


class DummyModule(BaseModule, metaclass=_DummyModuleMeta):
    """dummy module singleton"""
    def __init__(self):
        super().__init__(name="dummy")


    def __repr__(self):
        return "pygnulib.module.DummyModule"



class _GnulibModuleMeta(type):
    def __new__(mcs, name, parents, attributes):
        for key in BaseModule.keys():
            if key in attributes:
                continue
            fget = lambda self, key=key: self.__getitem__(key)
            doc = getattr(BaseModule, key).__doc__
            attributes[key] = _Property(fget=fget, doc=doc)
        return super().__new__(mcs, name, parents, attributes)


class GnulibModule(FileModule, metaclass=_GnulibModuleMeta):
    """read-only gnulib standard module"""
    __slots__ = ("__cache", "__hash", "__mask", "__path", "__test")


    __OBSOLETE = (1 << 0)
    __CXX_TEST = (1 << 1)
    __LONGRUNNING_TEST = (1 << 2)
    __PRIVILEGED_TEST = (1 << 3)
    __UNPORTABLE_TEST = (1 << 4)


    def __init__(self, path, name):
        super(FileModule, self).__init__(name=name)
        try:
            module = FileModule(path=path, name=name, test=name.endswith("-tests"))
        except FileNotFoundError:
            raise _UnknownModuleError(name)
        self.__cache = {_sys.intern(k):v for (k,v) in module.items()}
        self.__hash = super().__hash__()
        self.__mask = module.mask
        self.__path = module.path
        self.__test = module.test


    def __repr__(self):
        return f"{self.name}"
        module = self.__class__.__module__
        name = self.__class__.__name__
        return f"{module}.{name}{{{self.name}}}"


    def __getitem__(self, key):
        return self.__cache[key]


    def __hash__(self):
        return self.__hash


    def __eq__(self, other):
        if isinstance(other, GnulibModule):
            return self.__hash == hash(other)
        return super().__eq__(other)


    obsolete = _Property(
        fget=lambda self: bool(self.__mask & GnulibModule.__OBSOLETE),
        doc="module is obsolete?",
    )
    cxx_test = _Property(
        fget=lambda self: bool(self.__mask & GnulibModule.__CXX_TEST),
        doc="module is a C++ test?",
    )
    longrunning_test = _Property(
        fget=lambda self: bool(self.__mask & GnulibModule.__LONGRUNNING_TEST),
        doc="module is a longrunning test?",
    )
    privileged_test = _Property(
        fget=lambda self: bool(self.__mask & GnulibModule.__PRIVILEGED_TEST),
        doc="module is a privileged test?",
    )
    unportable_test = _Property(
        fget=lambda self: bool(self.__mask & GnulibModule.__UNPORTABLE_TEST),
        doc="module is an unportable test?",
    )
    mask = _Property(
        fget=lambda self: self.__mask,
        doc="module acceptibility mask",
    )
    test = _Property(
        fget=lambda self: self.name.endswith("-tests"),
        doc="module is tests-related?",
    )
    path = _Property(
        fget=lambda self: self.__path,
        doc="module file path",
    )


    @_Property
    def applicability(self):
        """WAGH applicability (usually "main" or "tests")"""
        default = "tests" if self.test else "main"
        current = self.__cache["applicability"]
        return current if current else default



class TransitiveClosure:
    """transitive closure table"""
    __slots__ = ("__lookup", "__dependencies", "__demanders", "__paths", "__conditionals")


    __AUTOMAKE_CONDITION = _re.compile("^if\\s+", _re.S | _re.M)


    def __init__(self, lookup, modules, mask, gnumake, conditionals, tests=False, error=True):
        if not callable(lookup):
            raise TypeError("lookup: callable expected")

        table = {None: None}
        def _lookup(module):
            return table.setdefault(module, lookup(module))

        current = set()
        previous = set()
        demanders = _collections.defaultdict(dict)
        dependencies = _collections.defaultdict(dict)
        def _update(demander, dependency, condition):
            table[dependency.name] = dependency
            if dependency.mask == mask:
                # A module whose Makefile.am snippet contains a reference to an
                # automake conditional. If we were to use it conditionally, we
                # would get an error
                #   configure: error: conditional "..." was never defined.
                # because automake 1.11.1 does not handle nested conditionals
                # correctly. As a workaround, make the module unconditional.
                snippet = dependency.automake_snippet
                pattern = TransitiveClosure.__AUTOMAKE_CONDITION
                if condition and pattern.findall(snippet):
                    condition = None
                    demander = None
                if not condition:
                    condition = None
                if demander is not None:
                    demander = demander.name
                dependency = dependency.name
                demanders[demander][dependency] = condition
                dependencies[dependency][demander] = condition
                current.add(dependency)

        for module in modules:
            dependency = lookup(module)
            _update(None, dependency, None)
        while True:
            modules = current.difference(previous)
            if not modules:
                break
            previous.update(current)
            for demander in modules:
                demander = _lookup(demander)
                if tests and not demander.test:
                    dependency = _lookup(demander.name + "-tests")
                    if dependency is not None:
                        _update(None, dependency, bool(dependencies[demander]))
                for (dependency, condition) in demander.dependencies:
                    dependency = _lookup(dependency)
                    _update(demander, dependency, condition)

        self.__lookup = _lookup
        self.__paths = dict()
        self.__conditionals = dict()
        self.__demanders = dict(demanders)
        self.__dependencies = dict(dependencies)


    def __iter__(self):
        for dependency in self.__dependencies:
            yield self.__lookup(dependency)


    def paths(self, module):
        graph = self.__dependencies
        module = self.__lookup(module).name
        if module in self.__paths:
            return self.__paths[module]
        def _paths():
            path = [module]
            seen = {module}
            def search():
                dead_end = True
                for neighbour in graph.get(path[-1], []):
                    if neighbour not in seen:
                        dead_end = False
                        seen.add(neighbour)
                        path.append(neighbour)
                        yield from search()
                        path.pop()
                        seen.remove(neighbour)
                if dead_end:
                    yield tuple(path)
            yield from search()
        result = self.__paths[module] = tuple(path[:-1] for path in _paths())
        return result


    def conditional(self, module):
        """
        Test whether module is a conditional dependency.
        Note that this check also takes all parent modules into account.
        """
        table = self.__dependencies
        module = self.__lookup(module).name
        def _conditional():
            if module in self.__demanders[None]:
                return False
            if module in self.__conditionals:
                return self.__conditionals[module]
            conditions = set()
            paths = self.paths(module)
            for path in paths:
                conditions.add(any({bool(table[dep][dem]) for (dep, dem) in zip(path, path[1:])}))
            return all(conditions)
        return self.__conditionals.setdefault(module, _conditional())


    def unconditional(self, module):
        """
        Test whether module is an unconditional dependency.
        Note that this check also takes all parent modules into account.
        """
        return not self.conditional(module)


    def dump(self, indent="  "):
        """Export transitive closure result into string."""
        def _dump():
            unconditional = set()
            storage = _collections.defaultdict(dict)
            yield "{{".format()
            for (dependency, entries) in self.__dependencies.items():
                for (demander, condition) in entries.items():
                    if condition is None:
                        condition = ""
                    if not demander and not condition:
                        unconditional.add(dependency)
                    condition = condition.replace("\"", "\\\"")
                    storage[dependency][demander] = condition
            for dependency in sorted(storage):
                if dependency in unconditional:
                    yield "{}\"{}\": {{}},".format(indent, dependency)
                    continue
                yield "{}\"{}\": {{".format(indent, dependency)
                for demander in sorted(storage[dependency]):
                    condition = storage[dependency][demander]
                    yield "{}\"{}\": \"{}\",".format((indent * 2), demander, condition)
                yield "{}}},".format(indent)
            yield "}}".format()
        if not self.__dependencies:
            return "{{}}".format()
        return _os.linesep.join(_dump())


    def load(self, string):
        """Import transitive closure result from string."""
        demanders = _collections.defaultdict(dict)
        dependencies = _collections.defaultdict(dict)
        collection = _ast.literal_eval(string)
        for key in collection:
            value = collection[key]
            for (subkey, subvalue) in value.items():
                (dependency, demander, condition) = (key, subkey, subvalue)
                if not condition:
                    condition = None
                demanders[demander][dependency] = condition
                dependencies[dependency][demander] = condition
        self.__demanders = dict(demanders)
        self.__dependencies = dict(dependencies)


    def demanders(self, module):
        """For each demander which requires the module yield the demander and the corresponding condition."""
        module = self.__lookup(module).name
        if module in self.__dependencies:
            for (demander, condition) in self.__dependencies.get(module, {}).items():
                yield (self.__lookup(demander), condition)


    def dependencies(self, module):
        """For each dependency of the module yield this dependency and the corresponding condition."""
        module = self.__lookup(module).name
        if module in self.__demanders:
            for (dependency, condition) in self.__demanders.get(module, {}).items():
                yield (self.__lookup(dependency), condition)



class Database:
    """gnulib module database"""
    __DUMMY_PATTERN = _re.compile(r"^lib_SOURCES\s*\+\=\s*(.*?)$", _re.S | _re.M)


    def __init__(self, lookup, config):
        if not callable(lookup):
            raise TypeError("lookup: callable expected")

        mask = config.mask
        gnumake = config.gnumake
        lookup = lambda module, lookup=lookup: module if isinstance(module, BaseModule) else lookup(module)

        def _applicability(module):
            return module.applicability in ({"main", "all"}, {"main"})[config.tests]

        def _dummy(modules):
            if "dummy" in config.avoids:
                return False
            for module in modules:
                snippet = module.conditional_automake_snippet
                for match in Database.__DUMMY_PATTERN.findall(snippet):
                    files = {file.strip() for file in match.split("#", 1)[0].split(" ") if file.strip()}
                    if {file for file in files if not file.endswith(".h")}:
                        return False
            return True

        def _files(modules):
            files = set()
            for module in modules:
                files.update(module.files)
            files.add("m4/00gnulib.m4")
            files.add("m4/gnulib-common.m4")
            if config.ac_version == 2.59:
                files.add("m4/onceonly.m4")
            return files

        def _libtests(modules):
            for module in modules:
                for file in module.files:
                    if file.startswith("lib/"):
                        return True
            return False

        # Perform a transitive closure for modules from the configuration.
        # The result of this transitive closure is a set of main modules.
        conditionals = config.conditionals
        modules = explicit_modules = {lookup(module) for module in config.modules}
        base_closure = TransitiveClosure(lookup, modules, mask, gnumake, conditionals)
        modules = map(lambda module: lookup(module.name + "-tests"), set(base_closure))
        modules = set(filter(lambda module: module is not None, modules))
        full_closure = TransitiveClosure(lookup, (explicit_modules | modules), mask, gnumake, conditionals, True)

        # Once the full transitive closure is completed, populate the database.
        main_modules = set(base_closure)
        final_modules = set(full_closure) if config.tests else main_modules
        test_modules = (final_modules - set(filter(_applicability, sorted(main_modules))))
        libtests = _libtests(test_modules)
        if _dummy(main_modules):
            main_modules.add(DummyModule())
        if _dummy(test_modules) and libtests:
            test_modules.add(DummyModule())
        main_files = _files(main_modules)
        test_files = _files(test_modules)

        self.__libtests = libtests
        self.__closure = full_closure
        self.__main_modules = tuple(sorted(main_modules))
        self.__test_modules = tuple(sorted(test_modules))
        self.__final_modules = tuple(sorted(final_modules))
        self.__explicit_modules = tuple(sorted(explicit_modules))
        self.__main_files = tuple(sorted(main_files))
        self.__test_files = tuple(sorted(test_files))


    def __iter__(self):
        return iter(self.__closure)


    def paths(self, module):
        return self.__closure.paths(module)


    def conditional(self, module):
        """
        Test whether module is a conditional dependency.
        Note that this check also takes all parent modules into account.
        Any module with an unconditional demander is also unconditional.
        """
        return self.__closure.conditional(module)


    def unconditional(self, module):
        """
        Test whether module is an unconditional dependency.
        Note that this check also takes all parent modules into account.
        Any module with an unconditional demander is also unconditional.
        """
        return not self.conditional(module)


    def demanders(self, module):
        """For each demander which requires the module yield the demander and the corresponding condition."""
        return sorted(self.__closure.demanders(module))


    def dependencies(self, module):
        """For each dependency of the module yield this dependency and the corresponding condition."""
        return sorted(self.__closure.dependencies(module))


    @property
    def final_modules(self):
        """
        The final module list is the transitive closure of the specified modules,
        including or ignoring tests modules (depending on the tests configuration
        option).
        """
        return self.__final_modules


    @property
    def main_modules(self):
        """
        The main module list is the transitive closure of the specified modules,
        ignoring tests modules. Its lib/* sources go into {source_base}. If --lgpl
        is specified, it will consist only of LGPLed source.
        """
        return self.__main_modules


    @property
    def main_files(self):
        """The full set of the files required for modules in the main modules list."""
        return self.__main_files


    @property
    def test_modules(self):
        """
        The tests-related module list is the transitive closure of the specified
        modules, including tests modules, minus the main module list excluding
        modules of applicability 'all'. Its lib/* sources (brought in through
        dependencies of *-tests modules) go into {tests_base}. It may contain GPLed
        source, even if --lgpl is specified.
        """
        return self.__test_modules


    @property
    def explicit_modules(self):
        """
        The list of modules which were explicitly required to be imported.
        This list does not include direct or indirect dependencies at all.
        """
        return self.__explicit_modules


    @property
    def test_files(self):
        """The full set of the files required for modules in the test modules list."""
        return self.__test_files


    @property
    def libtests(self):
        """If libtests.a is required, this variable yields true."""
        return self.__libtests
