#!/usr/bin/python
# encoding: UTF-8
"""gnulib module API"""



import codecs as _codecs_
import hashlib as _hashlib_
import os as _os_
import re as _re_


from .error import type_assert as _type_assert_
from .error import UnknownModuleError as _UnknownModuleError_
from .config import Option as _ConfigOption_



class Base:
    """gnulib generic module"""
    _TABLE_ = {
        "description"            : (0x00, str, "Description"),
        "comment"                : (0x01, str, "Comment"),
        "status"                 : (0x02, str, "Status"),
        "notice"                 : (0x03, str, "Notice"),
        "applicability"          : (0x04, str, "Applicability"),
        "files"                  : (0x05, set, "Files"),
        "dependencies"           : (0x06, set, "Depends-on"),
        "early_autoconf_snippet" : (0x07, str, "configure.ac-early"),
        "autoconf_snippet"       : (0x08, str, "configure.ac"),
        "automake_snippet"       : (0x09, str, "Makefile.am"),
        "include_directive"      : (0x0A, str, "Include"),
        "link_directive"         : (0x0B, str, "Link"),
        "licenses"               : (0x0C, set, "License"),
        "maintainers"            : (0x0D, set, "Maintainer"),
    }
    _PATTERN_DEPENDENCIES_ = _re_.compile("^(\\S+)(?:\\s+(.+))*$")


    def __init__(self, name, **kwargs):
        _type_assert_("name", name, str)
        licenses = set()
        for license in kwargs.get("licenses", tuple()):
            _type_assert_("license", license, str)
            licenses.add(license)
        self.__name = name
        self.__table = {}
        for (key, (_, typeid, _)) in Base._TABLE_.items():
            self.__table[key] = typeid()
        self.__table["licenses"] = licenses
        self.__table["maintainers"] = ["all"]
        for (key, value) in kwargs.items():
            self.__table[key] = value


    @property
    def name(self):
        """name"""
        return self.__name

    @name.setter
    def name(self, value):
        _type_assert_("name", value, str)
        self.__name = value


    @property
    def description(self):
        """description"""
        return self.__table["description"]

    @description.setter
    def description(self, value):
        _type_assert_("description", value, str)
        self.__table["description"] = value


    @property
    def comment(self):
        """comment"""
        return self.__table["comment"]

    @comment.setter
    def comment(self, value):
        _type_assert_("comment", value, str)
        self.__table["comment"] = value


    @property
    def status(self):
        """status"""
        return self.__table["status"]

    @status.setter
    def status(self, value):
        _type_assert_("status", value, str)
        self.__table["status"] = value


    @property
    def obsolete(self):
        """module is obsolete?"""
        return self.status == "obsolete"

    @property
    def cxx_test(self):
        """module is C++ test?"""
        return self.status == "c++-test"

    @property
    def longrunning_test(self):
        """module is C++ test?"""
        return self.status == "longrunning-test"

    @property
    def privileged_test(self):
        """module is privileged test?"""
        return self.status == "privileged-test"

    @property
    def unportable_test(self):
        """module is unportable test?"""
        return self.status == "unportable-test"


    @property
    def notice(self):
        """notice"""
        return self.__table["notice"]

    @notice.setter
    def notice(self, value):
        _type_assert_("notice", value, str)
        self.__table["notice"] = value


    @property
    def applicability(self):
        """applicability (usually "main" or "tests")"""
        default = "main" if self.name.endswith("-tests") else "tests"
        result = self.__table.get("applicability")
        return result if result else default

    @applicability.setter
    def applicability(self, value):
        _type_assert_("applicability", value, str)
        if value not in ("all", "main", "tests"):
            raise ValueError("applicability: \"main\", \"tests\" or \"all\"")
        self.__table["applicability"] = value


    @property
    def files(self):
        """file dependencies iterator (set of strings)"""
        for file in self.__table["files"]:
            yield file

    @files.setter
    def files(self, value):
        _type_assert_("files", value, (list, set, tuple))
        result = []
        for item in value:
            _type_assert_("file", item, str)
            result += [item]
        self.__table["files"] = set(result)


    @property
    def dependencies(self):
        """dependencies iterator (name, condition)"""
        for entry in self.__table["dependencies"]:
            yield Base._PATTERN_DEPENDENCIES_.findall(entry)[0]

    @dependencies.setter
    def dependencies(self, value):
        _type_assert_("files", value, (list, set, tuple))
        result = []
        for (name, condition) in value:
            _type_assert_("name", name, str)
            _type_assert_("condition", condition, str)
            result += [(name, condition)]
        self.__table["dependencies"] = set(result)


    @property
    def early_autoconf_snippet(self):
        """early configure.ac snippet"""
        return self.__table["early_autoconf_snippet"]

    @early_autoconf_snippet.setter
    def early_autoconf_snippet(self, value):
        _type_assert_("early_autoconf_snippet", value, str)
        self.__table["early_autoconf_snippet"] = value


    @property
    def autoconf_snippet(self):
        """configure.ac snippet"""
        return self.__table["autoconf_snippet"]

    @autoconf_snippet.setter
    def autoconf_snippet(self, value):
        _type_assert_("autoconf_snippet", value, str)
        self.__table["autoconf_snippet"] = value


    @property
    def automake_snippet(self):
        """Makefile.am snippet"""
        return self.__table["automake_snippet"]

    @automake_snippet.setter
    def automake_snippet(self, value):
        _type_assert_("automake_snippet", value, str)
        self.__table["automake_snippet"] = value


    @property
    def include_directive(self):
        """include directive"""
        value = self.__table["include_directive"]
        if value.startswith("<") or value.startswith("\""):
            return "#include {0}".format(value)
        return self.__table["include_directive"]

    @include_directive.setter
    def include_directive(self, value):
        _type_assert_("include_directive", value, str)
        self.__table["include_directive"] = value


    @property
    def link_directive(self):
        """link directive"""
        return self.__table["link_directive"]

    @link_directive.setter
    def link_directive(self, value):
        _type_assert_("link_directive", value, str)
        self.__table["link_directive"] = value


    @property
    def licenses(self):
        """licenses set"""
        return frozenset(self.__table["licenses"])

    @licenses.setter
    def licenses(self, value):
        _type_assert_("licenses", value, (list, set, tuple))
        result = set()
        for item in value:
            _type_assert_("license", item, str)
            result.add(value)
        self.__table["licenses"] = frozenset(result)


    @property
    def maintainers(self):
        """maintainers"""
        return "\n".join(self.__table["maintainers"])

    @maintainers.setter
    def maintainers(self, value):
        _type_assert_("maintainers", value, (list, set, tuple))
        result = []
        for item in value:
            _type_assert_("maintainer", item, str)
            result += [item]
        self.__table["maintainers"] = set(result)


    def shell_variable(self, macro_prefix="gl"):
        """Get the name of the shell variable set to true once m4 macros have been executed."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib_.md5(module).hexdigest()
        return "%s_gnulib_enabled_%s" % (macro_prefix, module)


    def shell_function(self, macro_prefix="gl"):
        """Get the name of the shell function containing the m4 macros."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib_.md5(module).hexdigest()
        return "func_%s_gnulib_m4code_%s" % (macro_prefix, module)


    def conditional_name(self, macro_prefix="gl"):
        """Get the automake conditional name."""
        module = self.name
        if len(module) != len(module.encode()):
            module = (module + "\n").encode("UTF-8")
            module = _hashlib_.md5(module).hexdigest()
        return "%s_GNULIB_ENABLED_%s" % (macro_prefix, module)


    def __hash__(self):
        return hash(str(self))


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "%s.%s{%r}" % (module, name, self.name)


    def __str__(self):
        result = ""
        for (key, (_, typeid, field)) in sorted(Base._TABLE_.items(), key=lambda k: k[1][0]):
            field += ":\n"
            if typeid in (list, set, tuple):
                value = "\n".join(self.__table[key])
            else:
                value = self.__table[key]
            if value:
                result += field
                result += value
                result += "\n\n" if value else "\n"
        return result.strip() + "\n"


    def __getitem__(self, key):
        if key not in Base._TABLE_:
            key = key.replace("-", "_")
            if key not in Base._TABLE_:
                raise KeyError(repr(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        if key not in Base._TABLE_:
            key = key.replace("-", "_")
            if key not in Base._TABLE_:
                raise KeyError(repr(key))
        return setattr(self, key, value)


    def items(self):
        """a set-like object providing a view on module items"""
        return self.__table.items()


    def keys(self):
        """a set-like object providing a view on module keys"""
        return self.__table.keys()


    def values(self):
        """a set-like object providing a view on module values"""
        return self.__table.values()


    def __lt__(self, value):
        if not isinstance(value, Base):
            return True
        return self.name < value.name

    def __le__(self, value):
        return self.__lt__(value) or self.__eq__(value)

    def __eq__(self, value):
        if not isinstance(value, Base):
            return False
        return self.name == value.name

    def __ne__(self, value):
        return not self.__eq__(value)

    def __ge__(self, value):
        return value.__le__(self)

    def __gt__(self, value):
        return value.__lt__(self)



class File(Base):
    """gnulib module text file"""
    _TABLE_ = {
        "Description"        : (str, "description"),
        "Comment"            : (str, "comment"),
        "Status"             : (str, "status"),
        "Notice"             : (str, "notice"),
        "Applicability"      : (str, "applicability"),
        "Files"              : (set, "files"),
        "Depends-on"         : (set, "dependencies"),
        "configure.ac-early" : (str, "early_autoconf_snippet"),
        "configure.ac"       : (str, "autoconf_snippet"),
        "Makefile.am"        : (str, "automake_snippet"),
        "Include"            : (str, "include_directive"),
        "Link"               : (str, "link_directive"),
        "License"            : (set, "licenses"),
        "Maintainer"         : (set, "maintainers"),
    }
    _FIELDS_ = [field for (_, _, field) in Base._TABLE_.values()]
    _PATTERN_ = _re_.compile("(%s):" % "|".join(_FIELDS_))


    def __init__(self, path, mode="r", name=None, **kwargs):
        table = {}
        if name is None:
            name = _os_.path.basename(path)
        if mode not in ("r", "w", "rw"):
            raise ValueError("illegal mode: %r" % mode)
        if mode == "r":
            with _codecs_.open(path, "rb", "UTF-8") as stream:
                match = File._PATTERN_.split(stream.read())[1:]
            for (group, value) in zip(match[::2], match[1::2]):
                (typeid, key) = File._TABLE_[group]
                if typeid in (list, set, tuple):
                    lines = []
                    for line in value.splitlines():
                        if not line.strip() or line.startswith("#"):
                            continue
                        lines += [line]
                    table[key] = lines
                else:
                    table[key] = value.strip()
            if "licenses" not in table:
                table["licenses"] = ["GPL"]
            if table["licenses"] == "LGPLv3+ or GPLv2":
                table["licenses"] = ["GPLv2, LGPLv3+"]
            self.__stream = None
        elif mode == "w":
            self.__stream = _codecs_.open(path, "w+", "UTF-8")
        elif mode == "rw":
            super().__init__(name)
            self.__init__(path, "r")
            self.__stream = _codecs_.open(path, "w+", "UTF-8")
        else:
            raise ValueError("invalid mode: %r" % mode)

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



def transitive_closure(lookup, modules, options):
    """
    Perform a transitive closure, generating a set of module dependencies.
    Each iteration over the table yields a tuple of (module, demander, condition).

    If condition is None, but demander is not, there is no special condition for this module.
    If demander is None, the module is provided unconditionally (condition is always None).

    lookup must be a callable which obtains a pygnulib module by its name.
    modules is an iterable, yielding a module (either name or instance).
    options may be any combination of gnulib configuration options.
    """
    if not callable(lookup):
        raise TypeError("lookup must be a callable")
    _type_assert_("options", options, int)
    if options & ~_ConfigOption_.All:
        raise ValueError("unknown configuration options")

    obsolete = bool(options & _ConfigOption_.Obsolete)
    tests = bool(options & _ConfigOption_.Tests)
    cxx_tests = bool(options & _ConfigOption_.CXX)
    longrunning_tests = bool(options & _ConfigOption_.Longrunning)
    privileged_tests = bool(options & _ConfigOption_.Privileged)
    unportable_tests = bool(options & _ConfigOption_.Unportable)
    modules = set(lookup(module) for module in modules)

    def _transitive_closure_(tests):
        queue = set()
        previous = set()
        current = set()
        for module in modules:
            current.add((module, None, None))
        while previous != current:
            previous.update(current)
            for (demander, _, _) in previous:
                if demander in queue:
                    continue
                if tests and not demander.name.endswith("-tests"):
                    try:
                        name = "{0}-tests".format(demander.name)
                        current.add((lookup(name), None, None))
                    except _UnknownModuleError_:
                        pass # ignore non-existent tests
                for (dependency, condition) in demander.dependencies:
                    module = lookup(dependency)
                    exclude = (
                        (not obsolete and module.obsolete),
                        (not cxx_tests and module.cxx_test),
                        (not longrunning_tests and module.longrunning_test),
                        (not privileged_tests and module.privileged_test),
                        (not unportable_tests and module.unportable_test),
                    )
                    if not any(exclude):
                        condition = condition if condition.strip() else None
                        current.add((module, demander, condition))
                queue.add(demander)
        return current

    base = _transitive_closure_(False)
    full = _transitive_closure_(True)
    main = set(module for (module, _, _) in base)
    final = set(module for (module, _, _) in full) if tests else set(main)
    tests_final = set(module for module in final if not tests or module.applicability != "all")
    tests_main = set(module for module in main if module.applicability != "all")
    tests = (tests_final ^ tests_main)
    return (base, full, main, final, tests)



def libtests_required(modules):
    """Determine whether libtests.a is required."""
    for module in modules:
        for file in module.files:
            if file.startswith("lib/"):
                return True
    return False



_DUMMY_REQUIRED_PATTERN_ = _re_.compile(r"^lib_SOURCES\s*\+\=\s*(.*?)$", _re_.S | _re_.M)
def dummy_required(modules):
    """Determine whether dummy module is required."""
    for module in modules:
        snippet = module.automake_snippet
        match = _DUMMY_REQUIRED_PATTERN_.findall(snippet)
        for files in match:
            files = (files.split("#", 1)[0].split(" "))
            files = (file.strip() for file in files if file.strip())
            if {file for file in files if not file.endswith(".h")}:
                return True
    return False
