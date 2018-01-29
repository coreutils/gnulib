#!/usr/bin/python
# encoding: UTF-8
"""gnulib configuration API"""



import codecs as _codecs
import os as _os
import re as _re

from collections import OrderedDict as _OrderedDict
from distutils import version as _version


from .error import type_assert as _type_assert
from .error import AutoconfVersionError as _AutoconfVersionError
from .error import M4BaseMismatchError as _M4BaseMismatchError



def _compile(regex):
    return _re.compile(regex, _re.S | _re.M)


_ITERABLES = frozenset((list, tuple, set, frozenset, type({}.keys()), type({}.values())))



LGPLv2_LICENSE = frozenset({"LGPLv2", "LGPLv2+"})
LGPLv3_LICENSE = frozenset({"LGPLv2+", "LGPLv3", "LGPLv3+", "LGPL"})
GPLv2_LICENSE = frozenset({"GPLv2", "GPLv2+"})
GPLv3_LICENSE = frozenset({"GPLv2+", "GPLv3", "GPLv3+", "GPL"})
LGPL_LICENSE = frozenset(LGPLv3_LICENSE)
GPL_LICENSE = frozenset(GPLv3_LICENSE)
OTHER_LICENSES = frozenset({
    "GPLed build tool",
    "public domain",
    "unlimited",
    "unmodifiable license text",
})



KEYS = frozenset({
    "root",
    "overrides",
    "source_base",
    "m4_base",
    "po_base",
    "doc_base",
    "tests_base",
    "auxdir",
    "libname",
    "makefile_name",
    "macro_prefix",
    "po_domain",
    "witness_c_macro",
    "licenses",
    "ac_version",
    "ac_file",
    "modules",
    "avoids",
    "files",
    "copymode",
    "local_copymode",
    "tests",
    "obsolete",
    "cxx_tests",
    "longrunning_tests",
    "privileged_tests",
    "unportable_tests",
    "libtool",
    "conditionals",
    "copyrights",
    "gnumake",
    "single_configure",
    "vc_files",
    "all_tests",
})



class BaseConfig:
    """gnulib generic configuration"""
    _TABLE = {
        "root"              : ".",
        "overrides"         : set(),
        "source_base"       : "lib",
        "m4_base"           : "m4",
        "po_base"           : "po",
        "doc_base"          : "doc",
        "tests_base"        : "tests",
        "auxdir"            : "",
        "libname"           : "libgnu",
        "makefile_name"     : "Makefile.am",
        "macro_prefix"      : "gl",
        "po_domain"         : "",
        "witness_c_macro"   : "",
        "licenses"          : set(),
        "ac_version"        : "2.59",
        "ac_file"           : "configure.ac",
        "modules"           : set(),
        "avoids"            : set(),
        "files"             : set(),
        "copymode"          : None,
        "local_copymode"    : None,
        "tests"             : False,
        "obsolete"          : False,
        "cxx_tests"         : False,
        "longrunning_tests" : False,
        "privileged_tests"  : False,
        "unportable_tests"  : False,
        "libtool"           : False,
        "conditionals"      : True,
        "copyrights"        : False,
        "gnumake"           : False,
        "single_configure"  : False,
        "vc_files"          : False,
    }


    class _Option:
        """gnulib configuration options"""
        Tests = (1 << 0)
        Obsolete = (1 << 1)
        CXX = (1 << 2)
        Longrunning = (1 << 3)
        Privileged = (1 << 4)
        Unportable = (1 << 5)
        Libtool = (1 << 6)
        Conditionals = (1 << 7)
        Copyrights = (1 << 8)
        GNUMake = (1 << 9)
        SingeConfigure = (1 << 10)
        AllTests = (Obsolete | Tests | CXX | Longrunning | Privileged | Unportable)


    def __init__(self, **kwargs):
        self.__options = 0
        self.__table = {}
        for (key, value) in BaseConfig._TABLE.items():
            self[key] = kwargs.get(key, value)


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{}".format(module, name, repr(self.__table))


    def __enter__(self):
        return self


    def __exit__(self, exctype, excval, exctrace):
        pass


    @property
    def root(self):
        """target directory"""
        return self.__table["root"]

    @root.setter
    def root(self, value):
        _type_assert("root", value, str)
        if not value:
            raise ValueError("non-empty path not allowed")
        self.__table["root"] = _os.path.normpath(value)


    @property
    def overrides(self):
        """local override directories"""
        return self.__table["overrides"]

    @overrides.setter
    def overrides(self, value):
        _type_assert("overrides", value, _ITERABLES)
        result = list()
        for item in value:
            _type_assert("override", item, str)
            result.append(_os.path.normpath(item))
        result = _OrderedDict.fromkeys(result)
        self.__table["overrides"] = tuple(result)


    @property
    def source_base(self):
        """directory relative to ROOT where source code is placed; defaults to 'lib'"""
        return self.__table["source_base"]

    @source_base.setter
    def source_base(self, value):
        _type_assert("source_base", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("source_base cannot be an absolute path")
        self.__table["source_base"] = _os.path.normpath(value) if value else "lib"


    @property
    def m4_base(self):
        """directory relative to ROOT where *.m4 macros are placed; defaults to 'm4'"""
        return self.__table["m4_base"]

    @m4_base.setter
    def m4_base(self, value):
        _type_assert("m4_base", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("m4_base cannot be an absolute path")
        self.__table["m4_base"] = _os.path.normpath(value) if value else "m4"


    @property
    def po_base(self):
        """directory relative to ROOT where *.po files are placed; defaults to 'po'"""
        return self.__table["po_base"]

    @po_base.setter
    def po_base(self, value):
        _type_assert("po_base", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("po_base cannot be an absolute path")
        self.__table["po_base"] = _os.path.normpath(value) if value else "po"


    @property
    def doc_base(self):
        """directory relative to ROOT where doc files are placed; defaults to 'doc'"""
        return self.__table["doc_base"]

    @doc_base.setter
    def doc_base(self, value):
        _type_assert("doc_base", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("doc_base cannot be an absolute path")
        self.__table["doc_base"] = _os.path.normpath(value) if value else "doc"


    @property
    def tests_base(self):
        """directory relative to ROOT where unit tests are placed; defaults to 'tests'"""
        return self.__table["tests_base"]

    @tests_base.setter
    def tests_base(self, value):
        _type_assert("tests_base", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("tests_base cannot be an absolute path")
        self.__table["tests_base"] = _os.path.normpath(value) if value else "tests"


    @property
    def auxdir(self):
        """directory relative to ROOT where auxiliary build tools are placed"""
        return self.__table["auxdir"]

    @auxdir.setter
    def auxdir(self, value):
        _type_assert("auxdir", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("auxdir cannot be an absolute path")
        self.__table["auxdir"] = _os.path.normpath(value) if value else "build-aux"


    @property
    def libname(self):
        """library name; defaults to 'libgnu'"""
        return self.__table["libname"]

    @libname.setter
    def libname(self, value):
        _type_assert("libname", value, str)
        self.__table["libname"] = value if value else "libgnu"


    @property
    def makefile_name(self):
        """name of makefile in automake syntax in the source-base and tests-base directories"""
        return self.__table["makefile_name"]

    @makefile_name.setter
    def makefile_name(self, value):
        _type_assert("makefile_name", value, str)
        value = _os.path.normpath(value)
        if _os.path.isabs(value):
            raise ValueError("makefile_name cannot be an absolute path")
        self.__table["makefile_name"] = value


    @property
    def macro_prefix(self):
        """
        the prefix of the macros 'gl_EARLY' and 'gl_INIT' (default is 'gl');
        the change of this parameter also affects include_guard_prefix parameter
        """
        return self.__table["macro_prefix"]

    @macro_prefix.setter
    def macro_prefix(self, value):
        _type_assert("macro_prefix", value, str)
        self.__table["macro_prefix"] = value


    @property
    def po_domain(self):
        """the prefix of the i18n domain"""
        return self.__table["po_domain"]

    @po_domain.setter
    def po_domain(self, value):
        _type_assert("po_domain", value, str)
        self.__table["po_domain"] = value


    @property
    def witness_c_macro(self):
        """the C macro that is defined when the sources are compiled or used"""
        return self.__table["witness_c_macro"]

    @witness_c_macro.setter
    def witness_c_macro(self, value):
        _type_assert("witness_c_macro", value, str)
        self.__table["witness_c_macro"] = value


    @property
    def licenses(self):
        """abort if modules aren't available under the LGPL; also modify license template"""
        return self.__table["licenses"]

    @licenses.setter
    def licenses(self, value):
        _type_assert("licenses", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("license", item, str)
            result.add(item)
        self.__table["licenses"] = frozenset(result)


    @property
    def ac_version(self):
        """minimal supported autoconf version"""
        return self.__table["ac_version"]

    @ac_version.setter
    def ac_version(self, value):
        _type_assert("ac_version", value, str)
        if _version.LooseVersion(value) < _version.LooseVersion("2.59"):
            raise _AutoconfVersionError("2.59")
        self.__table["ac_version"] = value


    @property
    def ac_file(self):
        """autoconf file (usually configure.ac or configure.in)"""
        return self.__table["ac_file"]

    @ac_file.setter
    def ac_file(self, value):
        _type_assert("ac_file", value, str)
        self.__table["ac_file"] = value


    @property
    def modules(self):
        """list of modules"""
        return self.__table["modules"]

    @modules.setter
    def modules(self, value):
        _type_assert("modules", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("module", item, str)
            result.add(item)
        self.__table["modules"] = frozenset(result)


    @property
    def avoids(self):
        """list of modules to avoid"""
        return self.__table["avoids"]

    @avoids.setter
    def avoids(self, value):
        _type_assert("avoids", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("avoid", item, str)
            result.add(item)
        self.__table["avoids"] = frozenset(result)


    @property
    def files(self):
        """list of files to be processed"""
        return self.__table["files"]

    @files.setter
    def files(self, value):
        _type_assert("files", value, _ITERABLES)
        result = set()
        for item in value:
            _type_assert("file", item, str)
            result.add(item)
        self.__table["files"] = frozenset(result)


    @property
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self.__table["macro_prefix"].upper()
        default = BaseConfig._TABLE["macro_prefix"].upper()
        return "GL" if prefix == default else "GL_{0}".format(prefix)


    @property
    def copymode(self):
        """file copy mode ('symlink', 'hardlink' or None)"""
        return self.__table["copymode"]

    @copymode.setter
    def copymode(self, value):
        if value not in frozenset({None, "symlink", "hardlink"}):
            raise ValueError("copymode: None, 'symlink' or 'hardlink'")
        self.__table["copymode"] = value


    @property
    def local_copymode(self):
        """file copy mode for local directory ('symlink', 'hardlink' or None)"""
        return self.__table["local_copymode"]

    @local_copymode.setter
    def local_copymode(self, value):
        if value not in frozenset({None, "symlink", "hardlink"}):
            raise ValueError("local_copymode: None, 'symlink' or 'hardlink'")
        self.__table["local_copymode"] = value


    @property
    def tests(self):
        """include unit tests for the included modules?"""
        return bool(self.__options & BaseConfig._Option.Tests)

    @tests.setter
    def tests(self, value):
        _type_assert("tests", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Tests
        else:
            self.__options &= ~BaseConfig._Option.Tests


    @property
    def obsolete(self):
        """include obsolete modules when they occur among the modules?"""
        return bool(self.__options & BaseConfig._Option.Obsolete)

    @obsolete.setter
    def obsolete(self, value):
        _type_assert("obsolete", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Obsolete
        else:
            self.__options &= ~BaseConfig._Option.Obsolete


    @property
    def cxx_tests(self):
        """include even unit tests for C++ interoperability?"""
        return bool(self.__options & BaseConfig._Option.CXX)

    @cxx_tests.setter
    def cxx_tests(self, value):
        _type_assert("cxx_tests", value, bool)
        if value:
            self.__options |= BaseConfig._Option.CXX
        else:
            self.__options &= ~BaseConfig._Option.CXX


    @property
    def longrunning_tests(self):
        """include even unit tests that are long-runners?"""
        return bool(self.__options & BaseConfig._Option.Longrunning)

    @longrunning_tests.setter
    def longrunning_tests(self, value):
        _type_assert("longrunning_tests", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Longrunning
        else:
            self.__options &= ~BaseConfig._Option.Longrunning


    @property
    def privileged_tests(self):
        """include even unit tests that require root privileges?"""
        return bool(self.__options & BaseConfig._Option.Privileged)

    @privileged_tests.setter
    def privileged_tests(self, value):
        _type_assert("privileged_tests", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Privileged
        else:
            self.__options &= ~BaseConfig._Option.Privileged


    @property
    def unportable_tests(self):
        """include even unit tests that fail on some platforms?"""
        return bool(self.__options & BaseConfig._Option.Unportable)

    @unportable_tests.setter
    def unportable_tests(self, value):
        _type_assert("unportable_tests", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Unportable
        else:
            self.__options &= ~BaseConfig._Option.Unportable


    @property
    def all_tests(self):
        """include all kinds of problematic unit tests?"""
        return (self.__options & BaseConfig._Option.AllTests) == BaseConfig._Option.AllTests

    @all_tests.setter
    def all_tests(self, value):
        if value:
            self.__options |= BaseConfig._Option.AllTests
        else:
            self.__options &= BaseConfig._Option.AllTests


    @property
    def libtool(self):
        """use libtool rules?"""
        return bool(self.__options & BaseConfig._Option.Libtool)

    @libtool.setter
    def libtool(self, value):
        _type_assert("libtool", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Libtool
        else:
            self.__options &= ~BaseConfig._Option.Libtool


    @property
    def conditionals(self):
        """support conditional dependencies (may save configure time and object code)?"""
        return bool(self.__options & BaseConfig._Option.Conditionals)

    @conditionals.setter
    def conditionals(self, value):
        _type_assert("conditionals", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Conditionals
        else:
            self.__options &= ~BaseConfig._Option.Conditionals


    @property
    def copyrights(self):
        """update the license copyright text?"""
        return bool(self.__options & BaseConfig._Option.Copyrights)

    @copyrights.setter
    def copyrights(self, value):
        _type_assert("copyrights", value, bool)
        if value:
            self.__options |= BaseConfig._Option.Copyrights
        else:
            self.__options &= ~BaseConfig._Option.Copyrights


    @property
    def gnumake(self):
        """update the license copyright text?"""
        return bool(self.__options & BaseConfig._Option.GNUMake)

    @gnumake.setter
    def gnumake(self, value):
        _type_assert("gnumake", value, bool)
        if value:
            self.__options |= BaseConfig._Option.GNUMake
        else:
            self.__options &= ~BaseConfig._Option.GNUMake


    @property
    def single_configure(self):
        """generate a single configure file?"""
        return bool(self.__options & BaseConfig._Option.SingeConfigure)

    @single_configure.setter
    def single_configure(self, value):
        _type_assert("single_configure", value, bool)
        if value:
            self.__options |= BaseConfig._Option.SingeConfigure
        else:
            self.__options &= ~BaseConfig._Option.SingeConfigure


    @property
    def vc_files(self):
        """update version control related files?"""
        return self.__table["vc_files"]

    @vc_files.setter
    def vc_files(self, value):
        _type_assert("vc_files", value, bool)
        self.__table["vc_files"] = value


    def __getitem__(self, key):
        if key not in KEYS:
            key = key.replace("-", "_")
            if key not in KEYS:
                raise KeyError("unsupported option: {0}".format(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        if key not in KEYS:
            key = key.replace("_", "-")
            if key not in KEYS:
                raise KeyError("unsupported option: {0}".format(key))
        return setattr(self, key, value)


    def items(self):
        """a set-like object providing a view on configuration items"""
        return self.__table.items()


    def keys(self):
        """a set-like object providing a view on configuration keys"""
        return self.__table.keys()


    def values(self):
        """a set-like object providing a view on configuration values"""
        return self.__table.values()



class CachedConfig(BaseConfig):
    """gnulib cached configuration"""
    _COMMENTS = _compile(r"((?:(?:#)|(?:^dnl\s+)|(?:\s+dnl\s+)).*?)$")
    _AUTOCONF = {
        "ac_version" : _compile(r"AC_PREREQ\(\[(.*?)\]\)"),
        "auxdir"     : _compile(r"AC_CONFIG_AUX_DIR\(\[(.*?)\]\)$"),
        "libtool"    : _compile(r"A[CM]_PROG_LIBTOOL")
    }
    _GNULIB_CACHE = {
        "overrides"         : (list, _compile(r"gl_LOCAL_DIR\(\[(.*?)\]\)")),
        "libtool"           : (bool, _compile(r"gl_LIBTOOL\(\[(.*?)\]\)")),
        "conditionals"      : (bool, _compile(r"gl_CONDITIONAL_DEPENDENCIES\(\[(.*?)\]\)")),
        "vc_files"          : (bool, _compile(r"gl_VC_FILES\(\[(.*?)\]\)")),
        "tests"             : (bool, _compile(r"gl_WITH_TESTS\(\[(.*?)\]\)")),
        "obsolete"          : (bool, _compile(r"gl_WITH_OBSOLETE\(\[(.*?)\]\)")),
        "cxx_tests"         : (bool, _compile(r"gl_WITH_CXX_TESTS\(\[(.*?)\]\)")),
        "longrunning_tests" : (bool, _compile(r"gl_WITH_LONGRUNNING_TESTS\(\[(.*?)\]\)")),
        "privileged_tests"  : (bool, _compile(r"gl_WITH_PRIVILEGED_TESTS\(\[(.*?)\]\)")),
        "unportable_tests"  : (bool, _compile(r"gl_WITH_UNPORTABLE_TESTS\(\[(.*?)\]\)")),
        "all_tests"         : (bool, _compile(r"gl_WITH_ALL_TESTS\(\[(.*?)\]\)")),
        "source_base"       : (str, _compile(r"gl_SOURCE_BASE\(\[(.*?)\]\)")),
        "m4_base"           : (str, _compile(r"gl_M4_BASE\(\[(.*?)\]\)")),
        "po_base"           : (str, _compile(r"gl_PO_BASE\(\[(.*?)\]\)")),
        "doc_base"          : (str, _compile(r"gl_DOC_BASE\(\[(.*?)\]\)")),
        "tests_base"        : (str, _compile(r"gl_TESTS_BASE\(\[(.*?)\]\)")),
        "makefile_name"     : (str, _compile(r"gl_MAKEFILE_NAME\(\[(.*?)\]\)")),
        "macro_prefix"      : (str, _compile(r"gl_MACRO_PREFIX\(\[(.*?)\]\)")),
        "po_domain"         : (str, _compile(r"gl_PO_DOMAIN\(\[(.*?)\]\)")),
        "witness_c_macro"   : (str, _compile(r"gl_WITNESS_C_MACRO\(\[(.*?)\]\)")),
        "libname"           : (str, _compile(r"gl_LIB\(\[(.*?)\]\)")),
        "modules"           : (list, _compile(r"gl_MODULES\(\[(.*?)\]\)")),
        "avoids"            : (list, _compile(r"gl_AVOID\(\[(.*?)\]\)")),
        "licenses"          : (str, _compile(r"gl_LGPL\(\[(.*?)\]\)")),
    }


    def __init__(self, root, ac_file=None, **kwargs):
        if ac_file is None:
            ac_file = "configure.ac"
        _type_assert("ac_file", ac_file, str)

        ac_path = _os.path.join(root, ac_file)
        if not _os.path.exists(ac_path):
            ac_file = "configure.in"
            ac_path = _os.path.join(root, ac_file)
        ac_path = _os.path.normpath(ac_path)
        super().__init__(root=root, **kwargs)
        self.__autoconf(ac_path, kwargs)
        self.__gnulib_cache(kwargs)
        self.__gnulib_comp(kwargs)


    def __autoconf(self, ac_path, explicit):
        with _codecs.open(ac_path, "rb", "UTF-8") as stream:
            data = CachedConfig._COMMENTS.sub("", stream.read())
        for (key, pattern) in CachedConfig._AUTOCONF.items():
            match = pattern.findall(data)
            if match and key not in explicit:
                self[key] = match[-1]

    def __gnulib_cache(self, explicit):
        expected_m4_base = self.m4_base
        path = _os.path.join(self.root, self.m4_base, "gnulib-cache.m4")
        if not _os.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs.open(path, "rb", "UTF-8") as stream:
            data = CachedConfig._COMMENTS.sub("", stream.read())
        for (key, (typeid, pattern)) in CachedConfig._GNULIB_CACHE.items():
            match = pattern.findall(data)
            if match and key not in explicit:
                if key == "licenses":
                    self[key] = {
                        "2": LGPLv2_LICENSE,
                        "3": LGPLv3_LICENSE,
                        "yes": LGPL_LICENSE,
                        "3orGPLv2": (GPLv2_LICENSE | LGPLv3_LICENSE),
                    }[match[-1]]
                elif typeid is bool:
                    self[key] = True
                elif typeid is str:
                    self[key] = match[-1].strip()
                else:
                    sep = None if key == "overrides" else "\n"
                    self[key] = [_.strip() for _ in match[-1].split(sep) if _.strip()]
        if expected_m4_base != self.m4_base:
            raise _M4BaseMismatchError(path, expected_m4_base, self.m4_base)

    def __gnulib_comp(self, explicit):
        macro_prefix = self.macro_prefix
        path = _os.path.join(self.root, self.m4_base, "gnulib-comp.m4")
        if not _os.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs.open(path, "rb", "UTF-8") as stream:
            data = CachedConfig._COMMENTS.sub("", stream.read())
        pattern = _compile(r"AC_DEFUN\(\[{0}_FILE_LIST\], \[(.*?)\]\)".format(macro_prefix))
        match = pattern.findall(data)
        if match and "files" not in explicit:
            self.files = [_.strip() for _ in match[-1].split("\n") if _.strip()]
