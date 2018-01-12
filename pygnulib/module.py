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


from .error import type_assert as _type_assert
from .error import UnknownModuleError as _UnknownModuleError
from .config import BaseConfig as _BaseConfig



_ITERABLES = frozenset((list, tuple, set, frozenset, type({}.keys()), type({}.values())))



class BaseModule:
    """gnulib generic module"""
    _TABLE = {
        "description"                  : (0x00, str, "Description"),
        "comment"                      : (0x01, str, "Comment"),
        "status"                       : (0x02, frozenset, "Status"),
        "notice"                       : (0x03, str, "Notice"),
        "applicability"                : (0x04, str, "Applicability"),
        "files"                        : (0x05, frozenset, "Files"),
        "dependencies"                 : (0x06, frozenset, "Depends-on"),
        "early_autoconf_snippet"       : (0x07, str, "configure.ac-early"),
        "autoconf_snippet"             : (0x08, str, "configure.ac"),
        "conditional_automake_snippet" : (0x09, str, "Makefile.am"),
        "include_directive"            : (0x0A, str, "Include"),
        "link_directive"               : (0x0B, str, "Link"),
        "licenses"                     : (0x0C, frozenset, "License"),
        "maintainers"                  : (0x0D, frozenset, "Maintainer"),
        # unconditional_automake_snippet
        # automake_snippet
    }
    _LIB_SOURCES = _re.compile(r"^lib_SOURCES\s*\+\=\s*(.*?)$", _re.S | _re.M)


    def __init__(self, name, **kwargs):
        _type_assert("name", name, str)
        if "licenses" in kwargs:
            licenses = set()
            for license in kwargs.get("licenses", frozenset()):
                _type_assert("license", license, str)
                licenses.add(license)
            kwargs["licenses"] = licenses
        if "maintainers" not in kwargs:
            kwargs["maintainers"] = {"all"}
        self.__name = name
        self.__table = _collections.OrderedDict()
        for (key, (_, typeid, _)) in BaseModule._TABLE.items():
            self.__table[key] = typeid(kwargs.get(key, typeid()))


    def __repr__(self):
        return "<" + self.__name + ">"


    def __str__(self):
        return "<" + self.__name + ">"


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    def __hash__(self):
        return hash(self.__name) ^ _json.dumps(self.__table, sort_keys=True)


    def __getitem__(self, key):
        _type_assert("key", key, str)
        if key not in BaseModule._TABLE:
            key = key.replace("-", "_")
            if key not in BaseModule._TABLE:
                raise KeyError(repr(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        _type_assert("key", key, str)
        if key not in BaseModule._TABLE:
            key = key.replace("-", "_")
            if key not in BaseModule._TABLE:
                raise KeyError(repr(key))
        return setattr(self, key, value)


    @property
    def package(self):
        """gnulib-compatible module package"""
        result = ""
        for (key, (_, typeid, field)) in sorted(BaseModule._TABLE.items(), key=lambda k: k[1][0]):
            field += ":\n"
            if typeid in _ITERABLES:
                value = "\n".join(self.__table[key])
            else:
                value = self.__table[key]
            if value:
                result += field
                result += value
                result += "\n\n" if value else "\n"
        return result.strip() + "\n"


    @property
    def name(self):
        """name"""
        return self.__name

    @name.setter
    def name(self, value):
        _type_assert("name", value, str)
        self.__name = value


    @property
    def description(self):
        """description"""
        return self.__table["description"]

    @description.setter
    def description(self, value):
        _type_assert("description", value, str)
        self.__table["description"] = value


    @property
    def comment(self):
        """comment"""
        return self.__table["comment"]

    @comment.setter
    def comment(self, value):
        _type_assert("comment", value, str)
        self.__table["comment"] = value


    @property
    def status(self):
        """status"""
        return self.__table["status"]

    @status.setter
    def status(self, value):
        _type_assert("status", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("status", item, str)
            result.add(item)
        self.__table["status"] = frozenset(result)


    @property
    def obsolete(self):
        """module is obsolete?"""
        return "obsolete" in self.status

    @property
    def cxx_test(self):
        """module is C++ test?"""
        return "c++-test" in self.status

    @property
    def longrunning_test(self):
        """module is C++ test?"""
        return "longrunning-test" in self.status

    @property
    def privileged_test(self):
        """module is privileged test?"""
        return "privileged-test" in self.status

    @property
    def unportable_test(self):
        """module is unportable test?"""
        return "unportable-test" in self.status


    @property
    def notice(self):
        """notice"""
        return self.__table["notice"]

    @notice.setter
    def notice(self, value):
        _type_assert("notice", value, str)
        self.__table["notice"] = value


    @property
    def applicability(self):
        """applicability (usually "main" or "tests")"""
        default = "main" if not self.name.endswith("-tests") else "tests"
        result = self.__table.get("applicability")
        return result if result else default

    @applicability.setter
    def applicability(self, value):
        _type_assert("applicability", value, str)
        if value not in ("all", "main", "tests"):
            raise ValueError("applicability: \"main\", \"tests\" or \"all\"")
        self.__table["applicability"] = value


    @property
    def files(self):
        """file dependencies iterator (set of strings)"""
        return frozenset(self.__table["files"])

    @files.setter
    def files(self, value):
        _type_assert("files", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("file", item, str)
            result.add(item)
        self.__table["files"] = frozenset(result)


    @property
    def dependencies(self):
        """dependencies iterator (name, condition)"""
        entries = set()
        for line in self.__table["dependencies"]:
            line = line.replace("\t", " ").strip()
            index = line.find(" ")
            if index == -1:
                module = line
                condition = ""
            else:
                module = line[:index].strip()
                condition = line[index:].strip()
            if condition.startswith("["):
                condition = condition[1:]
            if condition.endswith("]"):
                condition = condition[:-1]
            if not condition:
                condition = None
            entries.add((module, condition))
        return frozenset(entries)

    @dependencies.setter
    def dependencies(self, value):
        _type_assert("files", value, _ITERABLES)
        result = set()
        for (name, condition) in value:
            _type_assert("name", name, str)
            _type_assert("condition", condition, str)
            result.add((name, condition))
        self.__table["dependencies"] = frozenset(result)


    @property
    def early_autoconf_snippet(self):
        """early configure.ac snippet"""
        return self.__table["early_autoconf_snippet"]

    @early_autoconf_snippet.setter
    def early_autoconf_snippet(self, value):
        _type_assert("early_autoconf_snippet", value, str)
        self.__table["early_autoconf_snippet"] = value


    @property
    def autoconf_snippet(self):
        """configure.ac snippet"""
        return self.__table["autoconf_snippet"]

    @autoconf_snippet.setter
    def autoconf_snippet(self, value):
        _type_assert("autoconf_snippet", value, str)
        self.__table["autoconf_snippet"] = value


    @property
    def conditional_automake_snippet(self):
        """Makefile.am snippet that can be put inside Automake conditionals"""
        return self.__table["conditional_automake_snippet"]

    @conditional_automake_snippet.setter
    def conditional_automake_snippet(self, value):
        _type_assert("conditional_automake_snippet", value, str)
        self.__table["conditional_automake_snippet"] = value
        self.__table.pop("unconditional_automake_snippet", None)


    @property
    def unconditional_automake_snippet(self):
        """Makefile.am snippet that must stay outside of Automake conditionals"""
        if "unconditional_automake_snippet" in self.__table:
            return self.__table["unconditional_automake_snippet"]
        result = ""
        files = self.files
        if self.name.endswith("-tests"):
            # *-tests module live in tests/, not lib/.
            # Synthesize an EXTRA_DIST augmentation.
            test_files = {file for file in files if file.startswith("tests/")}
            if test_files:
                result += ("EXTRA_DIST += {}".format(" ".join(sorted(test_files))) + "\n")
            return result
        snippet = self.conditional_automake_snippet
        lib_SOURCES = False
        lines = list(snippet.splitlines())
        for (index, line) in enumerate(lines):
            if BaseModule._LIB_SOURCES.findall(line):
                (first, last) = (index, index)
                while line.endswith("\\"):
                    line = lines[last]
                    last += 1
                lines = list(lines)[first:(last + 1)]
                lines[0] = BaseModule._LIB_SOURCES.sub("\\1", lines[0])
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
            result += ("EXTRA_DIST += {}".format(" ".join(sorted(extra_files))) + "\n")

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
        prefix = _os.path.join("$(top_srcdir)", "{auxdir}")
        buildaux_files = (file for file in all_files if file.startswith("build-aux/"))
        buildaux_files = tuple(_os.path.join(prefix, file[len("build-aux/"):]) for file in buildaux_files)
        if buildaux_files:
            result += ("EXTRA_DIST += {}".format(" ".join(sorted(buildaux_files))) + "\n")
        self.__table["unconditional_automake_snippet"] = result
        return result


    @property
    def automake_snippet(self):
        """full automake snippet (conditional + unconditional parts)"""
        return self.conditional_automake_snippet + self.unconditional_automake_snippet


    @property
    def include_directive(self):
        """include directive"""
        value = self.__table["include_directive"]
        if value.startswith("<") or value.startswith("\""):
            return "#include {0}".format(value)
        return self.__table["include_directive"]

    @include_directive.setter
    def include_directive(self, value):
        _type_assert("include_directive", value, str)
        self.__table["include_directive"] = value


    @property
    def link_directive(self):
        """link directive"""
        return self.__table["link_directive"]

    @link_directive.setter
    def link_directive(self, value):
        _type_assert("link_directive", value, str)
        self.__table["link_directive"] = value


    @property
    def licenses(self):
        """licenses set"""
        return frozenset(self.__table["licenses"])

    @licenses.setter
    def licenses(self, value):
        _type_assert("licenses", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("license", item, str)
            result.add(value)
        self.__table["licenses"] = frozenset(result)


    @property
    def maintainers(self):
        """maintainers"""
        return self.__table["maintainers"]

    @maintainers.setter
    def maintainers(self, value):
        _type_assert("maintainers", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("maintainer", item, str)
            result.add(item)
        self.__table["maintainers"] = frozenset(result)


    def shell_variable(self, macro_prefix="gl"):
        """Get the name of the shell variable set to true once m4 macros have been executed."""
        module = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), module)):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib.md5(module).hexdigest()
        return "{}_gnulib_enabled_{}".format(macro_prefix, module)


    def shell_function(self, macro_prefix="gl"):
        """Get the name of the shell function containing the m4 macros."""
        module = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), module)):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib.md5(module).hexdigest()
        return "func_{}_gnulib_m4code_{}".format(macro_prefix, module)


    def conditional_name(self, macro_prefix="gl"):
        """Get the automake conditional name."""
        module = self.name
        if any(filter(lambda rune: not (rune.isalnum() or rune == "_"), module)):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib.md5(module).hexdigest()
        return "{}_GNULIB_ENABLED_{}".format(macro_prefix, module)


    def items(self):
        """a set-like object providing a view on module items"""
        return self.__table.items()


    def keys(self):
        """a set-like object providing a view on module keys"""
        for key in BaseModule._TABLE.keys():
            yield key


    def values(self):
        """a set-like object providing a view on module values"""
        for key in BaseModule._TABLE.keys():
            yield self[value]


    def __lt__(self, value):
        _type_assert("value", value, BaseModule)
        return self.name < value.name

    def __le__(self, value):
        return self.__lt__(value) or self.__eq__(value)

    def __eq__(self, value):
        return hash(self) == hash(value)

    def __ne__(self, value):
        return not self.__eq__(value)

    def __ge__(self, value):
        return value.__le__(self)

    def __gt__(self, value):
        return value.__lt__(self)



class _DummyModuleMeta(type):
    __INSTANCE = None
    __TABLE = {
        "description": "A dummy file, to make sure the library is non-empty.",
        "comment": "",
        "status": frozenset(),
        "notice": "",
        "applicability": "main",
        "files": frozenset({"lib/dummy.c"}),
        "dependencies": frozenset(),
        "early_autoconf_snippet": "",
        "autoconf_snippet": "",
        "include_directive": "",
        "link_directive": "",
        "licenses": frozenset({"public domain"}),
        "maintainers": frozenset({"all"}),
        "automake_snippet": "lib_SOURCES += dummy.c",
        "conditional_automake_snippet": "lib_SOURCES += dummy.c",
        "unconditional_automake_snippet": "",
    }

    def __new__(cls, name, parents, attributes):
        self = super().__new__(cls, name, parents, attributes)
        for (key, value) in _DummyModuleMeta.__TABLE.items():
            setattr(self, key, property(lambda self, value=value: value))
        return self

    def __call__(cls, *args, **kwargs):
        if _DummyModuleMeta.__INSTANCE is None:
            _DummyModuleMeta.__INSTANCE = super(_DummyModuleMeta, cls).__call__(*args, **kwargs)
        return _DummyModuleMeta.__INSTANCE


class DummyModule(BaseModule, metaclass=_DummyModuleMeta):
    def __init__(self):
        super().__init__(name="dummy")



class FileModule(BaseModule):
    """gnulib module text file"""
    _TABLE = {_value[2]:(_value[1], _key) for (_key, _value) in BaseModule._TABLE.items()}
    _FIELDS = [_field for (_, _, _field) in BaseModule._TABLE.values()]
    _PATTERN = _re.compile("({}):".format("|".join(_FIELDS)))


    def __init__(self, path, mode="r", name=None, **kwargs):
        table = {}
        if name is None:
            name = _os.path.basename(path)
        if mode not in ("r", "w", "rw"):
            raise ValueError("illegal mode: {}".format(mode))
        if mode == "r":
            with _codecs.open(path, "rb", "UTF-8") as stream:
                match = FileModule._PATTERN.split(stream.read())[1:]
            for (group, value) in zip(match[::2], match[1::2]):
                (typeid, key) = FileModule._TABLE[group]
                if typeid in _ITERABLES:
                    lines = []
                    for line in value.splitlines():
                        if line.strip() and not line.startswith("#"):
                            lines.append(line)
                    table[key] = typeid(lines)
                else:
                    table[key] = value.strip()
            if "licenses" not in table:
                table["licenses"] = ["GPL"]
            if table["licenses"] == "LGPLv3+ or GPLv2":
                table["licenses"] = ["GPLv2, LGPLv3+"]
            self.__stream = None
        elif mode == "w":
            self.__stream = _codecs.open(path, "w+", "UTF-8")
        elif mode == "rw":
            super().__init__(name)
            self.__init__(path, "r")
            self.__stream = _codecs.open(path, "w+", "UTF-8")
        else:
            raise ValueError("illegal mode: {}".format(mode))

        for (key, value) in kwargs.items():
            table[key] = value
        super().__init__(name, **table)


    def close(self):
        """Close the underlying stream and write data into the file."""
        if self.__stream:
            self.__stream.truncate(0)
            self.__stream.write(str(self))
            self.__stream.close()


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        self.close()



class TransitiveClosure:
    """transitive closure table"""
    def __init__(self, lookup, config, tests):
        if not callable(lookup):
            raise TypeError("lookup: callable expected")
        _type_assert("config", config, _BaseConfig)
        _type_assert("tests", tests, bool)

        def _exclude(module):
            return any((
                (not config.obsolete and module.obsolete),
                (not config.cxx_tests and module.cxx_test),
                (not config.longrunning_tests and module.longrunning_test),
                (not config.privileged_tests and module.privileged_test),
                (not config.unportable_tests and module.unportable_test),
            ))

        def _lookup(module):
            if not (module is None or isinstance(module, BaseModule)):
                if isinstance(module, str):
                    if lookup is None:
                        raise TypeError("cannot instantiate {} module".format(module))
                    module = lookup(module)
                if not isinstance(module, BaseModule):
                    raise TypeError("module: pygnulib.module.BaseModule expected")
            return module

        handled = set()
        conditional = set()
        unconditional = set()
        demanders = _collections.defaultdict(dict)
        dependencies = _collections.defaultdict(dict)
        modules = {lookup(module) for module in config.modules}
        for dependency in modules:
            unconditional.add(dependency)
            dependencies[dependency][None] = None
        while modules:
            for demander in set(modules):
                if tests and not demander.name.endswith("-tests"):
                    try:
                        dependency = lookup("{}-tests".format(demander.name))
                        if not _exclude(dependency):
                            demanders[None][dependency] = None
                            dependencies[dependency][None] = None
                            modules.add(dependency)
                            dependency = demander
                    except _UnknownModuleError:
                        pass # ignore non-existent tests
                for (dependency, condition) in demander.dependencies:
                    dependency = lookup(dependency)
                    if config.gnumake and condition.startswith("if "):
                        # A module whose Makefile.am snippet contains a reference to an
                        # automake conditional. If we were to use it conditionally, we
                        # would get an error
                        #   configure: error: conditional "..." was never defined.
                        # because automake 1.11.1 does not handle nested conditionals
                        # correctly. As a workaround, make the module unconditional.
                        demanders[None][dependency] = None
                        dependencies[dependency][None] = None
                        unconditional.add(dependency)
                    elif not _exclude(dependency):
                        if condition is not None:
                            conditional.add(dependency)
                        elif demander in conditional:
                            conditional.add(dependency)
                        else:
                            conditional.discard(dependency)
                            unconditional.add(dependency)
                        demanders[demander][dependency] = None
                        dependencies[dependency][demander] = condition
                    modules.add(dependency)
                handled.add(demander)
            modules.difference_update(handled)

        self.__lookup = _lookup
        self.__demanders = demanders
        self.__dependencies = dependencies
        self.__conditional = frozenset(conditional)


    def __iter__(self):
        for dependency in self.__dependencies:
            yield dependency


    def dump(self, indent="  "):
        """Export transitive closure result into string."""
        def _dump():
            unconditional = set()
            storage = _collections.defaultdict(dict)
            yield "{{".format()
            for (key, value) in self.__dependencies.items():
                for (subkey, subvalue) in value.items():
                    dependency = key.name
                    demander = subkey.name if subkey else ""
                    condition = subvalue if subvalue else ""
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
        _type_assert("collection", collection, dict)
        for key in collection:
            _type_assert("key", key, str)
            value = collection[key]
            _type_assert("value", value, dict)
            for (subkey, subvalue) in value.items():
                _type_assert("key", key, str)
                _type_assert("dict", value, dict)
                (dependency, demander, condition) = (key, subkey, subvalue)
                dependency = self.__lookup(dependency)
                demander = self.__lookup(demander)
                if not condition:
                    condition = None
                demanders[demander][dependency] = condition
                dependencies[dependency][demander] = condition
        self.__demanders = dict(demanders)
        self.__dependencies = dict(dependencies)


    def conditional(self, module):
        """
        Test whether module is a conditional dependency.
        Note that this check also takes all parent modules into account.
        Any module with an unconditional demander is also unconditional.
        """
        module = self.__lookup(module)
        if module not in self.__dependencies:
            fmt = "dependency {} not found"
            raise KeyError(fmt.format(module))
        return module in self.__conditional


    def unconditional(self, module):
        """
        Test whether module is an unconditional dependency.
        Note that this check also takes all parent modules into account.
        Any module with an unconditional demander is also unconditional.
        """
        return not self.conditional(module)


    def demanders(self, module):
        """For each demander which requires the module yield the demander and condition."""
        module = self.__lookup(module)
        if module not in self.__dependencies:
            fmt = "dependency {} not found"
            raise KeyError(fmt.format(module))
        for (demander, condition) in self.__dependencies.get(module, {}).items():
            yield (demander, condition)


    def dependencies(self, module):
        """For each dependency of this module yield the dependency and the condition."""
        module = self.__lookup(module)
        if module not in self.__demanders:
            fmt = "demander {} not found"
            raise KeyError(fmt.format(module))
        for (dependency, condition) in self.__demanders.get(module, {}).items():
            yield (dependency, condition)



class Database:
    """gnulib module database"""
    __DUMMY_PATTERN = _re.compile(r"^lib_SOURCES\s*\+\=\s*(.*?)$", _re.S | _re.M)


    def __init__(self, lookup, config):
        if not callable(lookup):
            raise TypeError("lookup: callable expected")
        _type_assert("config", config, _BaseConfig)

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

        base_table = TransitiveClosure(lookup, config, False)
        full_table = TransitiveClosure(lookup, config, True)
        main_modules = set(base_table)
        explicit_modules = set()
        for module in full_table:
            if module.name in config.modules:
                explicit_modules.add(module)
        final_modules = set(full_table) if config.tests else main_modules
        test_modules = (final_modules - set(filter(_applicability, main_modules)))
        libtests = _libtests(test_modules)
        if _dummy(main_modules):
            main_modules.add(DummyModule())
        if _dummy(test_modules) and libtests:
            test_modules.add(DummyModule())
        main_files = _files(main_modules)
        test_files = set()
        for file in _files(test_modules):
            if file.startswith("lib/"):
                file = ("lib=tests/" + file[len("lib/"):])
            test_files.add(file)

        self.__libtests = libtests
        self.__base_table = base_table
        self.__full_table = full_table
        self.__main_modules = tuple(sorted(main_modules))
        self.__test_modules = tuple(sorted(test_modules))
        self.__final_modules = tuple(sorted(final_modules))
        self.__explicit_modules = tuple(sorted(explicit_modules))
        self.__main_files = tuple(sorted(main_files))
        self.__test_files = tuple(sorted(test_files))


    @property
    def base_table(self):
        """
        The transitive closure table of the specified modules, ignoring tests modules.
        An iteration yields a (dependency, demander, condition) on each step.
        """
        return self.__base_table


    @property
    def full_table(self):
        """
        The full transitive closure table of the specified modules.
        An iteration yields a (dependency, demander, condition) on each step.
        """
        return self.__full_table


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
