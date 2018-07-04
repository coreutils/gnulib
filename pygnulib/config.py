#!/usr/bin/python
# encoding: UTF-8
"""gnulib configuration API"""



import codecs as _codecs
import os as _os
import re as _re
from distutils import version as _version

from .error import AutoconfVersionError as _AutoconfVersionError
from .error import M4BaseMismatchError as _M4BaseMismatchError
from .misc import Property as _Property
from .misc import PathProperty as _PathProperty
from .misc import OptionalPathProperty as _OptionalPathProperty
from .misc import BitwiseProperty as _BitwiseProperty
from .misc import StringListProperty as _StringListProperty
from .misc import PathListProperty as _PathListProperty



def _compile(regex):
    return _re.compile(regex, _re.S | _re.M)



LGPLv2_LICENSE = frozenset({"LGPLv2", "LGPLv2+"})
LGPLv3_LICENSE = frozenset({"LGPLv2+", "LGPLv3", "LGPLv3+", "LGPL"})
GPLv2_LICENSE = frozenset({"GPLv2", "GPLv2+"})
GPLv3_LICENSE = frozenset({"GPLv2+", "GPLv3", "GPLv3+", "GPL"})
LGPL_LICENSES = frozenset(LGPLv3_LICENSE)
GPL_LICENSES = frozenset(GPLv3_LICENSE)
OTHER_LICENSES = frozenset({
    "GPLed build tool",
    "public domain",
    "unlimited",
    "unmodifiable license text",
})



class BaseConfig:
    """gnulib generic configuration"""
    __slots__ = ("__options", "__flags", "__active")


    __PROPERTIES = {
        "root"              : ".",
        "overrides"         : tuple(),
        "source_base"       : "lib",
        "m4_base"           : "m4",
        "po_base"           : None,
        "doc_base"          : "doc",
        "tests_base"        : "tests",
        "auxdir"            : "build-aux",
        "libname"           : "libgnu",
        "makefile_name"     : None,
        "macro_prefix"      : "gl",
        "po_domain"         : "",
        "witness_c_macro"   : "",
        "licenses"          : tuple(),
        "ac_version"        : "2.59",
        "ac_file"           : "configure.ac",
        "modules"           : tuple(),
        "avoids"            : tuple(),
        "files"             : tuple(),
        "copymode"          : None,
        "local_copymode"    : None,
        "tests"             : False,
        "cxx_tests"         : False,
        "longrunning_tests" : False,
        "privileged_tests"  : False,
        "unportable_tests"  : False,
        "all_tests"         : False,
        "obsolete"          : False,
        "libtool"           : False,
        "conditionals"      : False,
        "copyrights"        : False,
        "gnumake"           : False,
        "single_configure"  : False,
        "vc_files"          : False,
    }
    __OPTIONS = {
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
    }
    __FLAGS = {
        "tests",
        "cxx_tests",
        "longrunning_tests",
        "privileged_tests",
        "unportable_tests",
        "all_tests",
        "obsolete",
        "libtool",
        "conditionals",
        "copyrights",
        "gnumake",
        "single_configure",
        "vc_files",
    }


    def __init__(self, root, **kwargs):
        if not isinstance(root, str):
            raise TypeError("root: str expected")
        if not root:
            raise ValueError("root: empty path")
        root = _os.path.normpath(root)

        self.__active = {}
        self.__flags = 0
        self.__options = {}
        for key in BaseConfig.__OPTIONS:
            value = BaseConfig.__PROPERTIES[key]
            self.__set_option_pure(key, value)
        for key in BaseConfig.__FLAGS:
            state = BaseConfig.__PROPERTIES[key]
            mask = getattr(self.__class__, key).mask
            self.__set_flags_pure(mask, state)
        self.__set_option("root", root)

        for (key, value) in kwargs.items():
            setattr(self, key, value)


    def __enter__(self):
        return self


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        table = ", ".join(f"{key}={value}" for (key, value) in self.items())
        return f"{module}.{name}{{{table}}}"


    def __getitem__(self, key):
        if key not in BaseConfig.__PROPERTIES:
            key = key.replace("-", "_")
            if key not in BaseConfig.__PROPERTIES:
                raise KeyError("unsupported option: {0}".format(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        if key not in BaseConfig.__PROPERTIES:
            key = key.replace("_", "-")
            if key not in BaseConfig.__PROPERTIES:
                raise KeyError("unsupported option: {0}".format(key))
        return setattr(self, key, value)


    def __get_option(self, key):
        return self.__options[key]

    def __set_option_pure(self, key, value):
        self.__options[key] = value

    def __set_option(self, key, value):
        self.__active[key] = True
        return self.__set_option_pure(key, value)


    def __get_flags(self, mask):
        return self.__flags & mask

    def __set_flags_pure(self, mask, state):
        if state:
            self.__flags |= mask
        else:
            self.__flags &= ~mask

    def __set_flags(self, mask, state):
        key = {getattr(BaseConfig, key).mask:key for key in BaseConfig.__FLAGS}[mask]
        self.__active[key] = True
        return self.__set_flags_pure(mask, state)


    root = _PathProperty(
        fget=lambda self: self.__get_option("root"),
        fset=lambda self, path: self.__set_option("root", path),
        doc="target directory (root project directory)",
    )
    overrides = _PathListProperty(
        sorted=False,
        unique=True,
        fget=lambda self: self.__get_option("overrides"),
        fset=lambda self, paths: self.__set_option("overrides", paths),
        doc="local override directories",
    )
    source_base = _PathProperty(
        fget=lambda self: self.__get_option("source_base"),
        fset=lambda self, path: self.__set_option("source_base", path),
        doc="directory relative to ROOT where source code is placed; defaults to 'lib'",
    )
    m4_base = _PathProperty(
        fget=lambda self: self.__get_option("m4_base"),
        fset=lambda self, path: self.__set_option("m4_base", path),
        doc="directory relative to ROOT where *.m4 macros are placed; defaults to 'm4'",
    )
    po_base = _OptionalPathProperty(
        fget=lambda self: self.__get_option("po_base"),
        fset=lambda self, path: self.__set_option("po_base", path),
        doc="directory relative to ROOT where *.po files are placed; defaults to 'po'",
    )
    doc_base = _PathProperty(
        fget=lambda self: self.__get_option("doc_base"),
        fset=lambda self, path: self.__set_option("doc_base", path),
        doc="directory relative to ROOT where doc files are placed; defaults to 'doc'",
    )
    tests_base = _PathProperty(
        fget=lambda self: self.__get_option("tests_base"),
        fset=lambda self, path: self.__set_option("tests_base", path),
        doc="directory relative to ROOT where unit tests are placed; defaults to 'tests'",
    )
    auxdir = _PathProperty(
        fget=lambda self: self.__get_option("auxdir"),
        fset=lambda self, path: self.__set_option("auxdir", path),
        doc="directory relative to ROOT where auxiliary build tools are placed; defaults to 'build-aux'",
    )
    libname = _Property(
        fget=lambda self: self.__get_option("libname"),
        fset=lambda self, name: self.__set_option("libname", name),
        check=lambda value: isinstance(value, str) and value,
        doc="library name; defaults to 'libgnu'",
    )
    makefile_name = _OptionalPathProperty(
        fget=lambda self: self.__get_option("makefile_name"),
        fset=lambda self, name: self.__set_option("makefile_name", name),
        doc="name of makefile in automake syntax in the source-base and tests-base directories",
    )
    macro_prefix = _Property(
        fget=lambda self: self.__get_option("macro_prefix"),
        fset=lambda self, name: self.__set_option("macro_prefix", name),
        check=lambda value: isinstance(value, str) and value,
        doc="the prefix of the macros 'gl_EARLY' and 'gl_INIT'; defaults to 'gl'",
    )
    po_domain = _Property(
        fget=lambda self: self.__get_option("po_domain"),
        fset=lambda self, name: self.__set_option("po_domain", name),
        check=lambda value: value is None or isinstance(value, str),
        doc="the prefix of the i18n domain",
    )
    witness_c_macro = _Property(
        fget=lambda self: self.__get_option("witness_c_macro"),
        fset=lambda self, name: self.__set_option("witness_c_macro", name),
        check=lambda value: isinstance(value, str) and (value == "") or not value[0].isdigit(),
        doc="the C macro that is defined when the sources are compiled or used",
    )
    modules = _StringListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("modules"),
        fset=lambda self, name: self.__set_option("modules", name),
        doc="sorted set of the desired modules",
    )
    avoids = _StringListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("avoids"),
        fset=lambda self, name: self.__set_option("avoids", name),
        doc="sorted set of the modules that must be avoided",
    )
    files = _PathListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: self.__get_option("files"),
        fset=lambda self, name: self.__set_option("files", name),
        doc="a list of files to be processed",
    )
    copymode = _Property(
        fget=lambda self: self.__get_option("copymode"),
        fset=lambda self, mode: self.__set_option("copymode", mode),
        check=lambda value: value in {None, "hardlink", "symlink"},
        doc="file copy mode ('symlink', 'hardlink' or None)",
    )
    local_copymode = _Property(
        fget=lambda self: self.__get_option("copymode"),
        fset=lambda self, mode: self.__set_option("copymode", mode),
        check=lambda value: value in {None, "hardlink", "symlink"},
        doc="file copy mode for local directory ('symlink', 'hardlink' or None)",
    )
    ac_file = _PathProperty(
        fget=lambda self: self.__get_option("ac_file"),
        fset=lambda self, path: self.__set_option("ac_file", path),
        doc="autoconf file (usually configure.ac or configure.in)",
    )
    ac_version = _Property(
        fget=lambda self: self.__get_option("ac_version"),
        fset=lambda self, version: self.__set_option("ac_version", str(_version.LooseVersion(version))),
        doc="minimal supported autoconf version",
    )
    licenses = _StringListProperty(
        sorted=True,
        unique=True,
        fget=lambda self: set(self.__get_option("licenses")),
        fset=lambda self, name: self.__set_option("licenses", name),
        doc="acceptable licenses for modules",
    )
    tests = _BitwiseProperty(
        mask=(1 << 0),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include unit tests for the included modules?",
    )
    obsolete = _BitwiseProperty(
        mask=(1 << 1),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include obsolete modules when they occur among the modules?",
    )
    cxx_tests = _BitwiseProperty(
        mask=(1 << 2),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include even unit tests for C++ interoperability?",
    )
    longrunning_tests = _BitwiseProperty(
        mask=(1 << 3),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include even unit tests for C++ interoperability?",
    )
    privileged_tests = _BitwiseProperty(
        mask=(1 << 4),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include even unit tests that require root privileges?",
    )
    unportable_tests = _BitwiseProperty(
        mask=(1 << 5),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include even unit tests that fail on some platforms?",
    )
    mask = _BitwiseProperty(
        mask=(obsolete.mask | cxx_tests.mask | longrunning_tests.mask | privileged_tests.mask | unportable_tests.mask),
        fget=lambda self, mask: self.__get_flags(mask),
        doc="configuration acceptibility mask",
    )
    all_tests = _BitwiseProperty(
        mask=(tests.mask | cxx_tests.mask | longrunning_tests.mask | privileged_tests.mask | unportable_tests.mask),
        fget=lambda self, mask: bool(self.__get_flags(mask) == mask),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="include all kinds of unit tests?",
    )
    libtool = _BitwiseProperty(
        mask=(1 << 6),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="use libtool rules?",
    )
    conditionals = _BitwiseProperty(
        mask=(1 << 7),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="support conditional dependencies (may save configure time and object code)?",
    )
    copyrights = _BitwiseProperty(
        mask=(1 << 8),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="update the license copyright text?",
    )
    gnumake = _BitwiseProperty(
        mask=(1 << 9),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="output for GNU Make instead of for the default Automake?",
    )
    single_configure = _BitwiseProperty(
        mask=(1 << 10),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="generate a single configure file?",
    )
    vc_files = _BitwiseProperty(
        mask=(1 << 11),
        fget=lambda self, mask: bool(self.__get_flags(mask)),
        fset=lambda self, mask, state: self.__set_flags(mask, state),
        doc="update version control related files?",
    )


    @property
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self.macro_prefix.upper()
        return "GL" if prefix == "GL" else f"GL_{prefix}"


    def explicit(self, key):
        """Determine if option is set to a non-default value."""
        if key not in BaseConfig.__PROPERTIES:
            key = key.replace("-", "_")
            if key not in BaseConfig.__PROPERTIES:
                raise KeyError("unsupported option: {0}".format(key))
        return self.__active.get(key, False)


    def items(self):
        """a set-like object providing a view on configuration items"""
        for key in BaseConfig.__PROPERTIES:
            value = self[key]
            yield (key, value)


    def keys(self):
        """a set-like object providing a view on configuration keys"""
        return iter(BaseConfig.__PROPERTIES)


    def values(self):
        """a set-like object providing a view on configuration values"""
        for key in BaseConfig.__PROPERTIES:
            yield self[key]



class CachedConfig(BaseConfig):
    """gnulib cached configuration"""
    __slots__ = ()


    __COMMENTS = _compile(r"((?:(?:#)|(?:^dnl\s+)|(?:\s+dnl\s+)).*?)$")
    __AUTOCONF = {
        "ac_version" : _compile(r"AC_PREREQ\(\[(.*?)\]\)"),
        "auxdir"     : _compile(r"AC_CONFIG_AUX_DIR\(\[(.*?)\]\)$"),
        "libtool"    : _compile(r"A[CM]_PROG_LIBTOOL")
    }
    __GNULIB_CACHE = {
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


    def __init__(self, root, gnulib_comp=False, gnulib_cache=False, ac_file=None, **kwargs):
        if ac_file is None:
            ac_file = "configure.ac"
        if not isinstance(ac_file, str):
            raise TypeError("ac_file: str expected")

        ac_path = _os.path.join(root, ac_file)
        if not _os.path.exists(ac_path):
            ac_file = "configure.in"
            ac_path = _os.path.join(root, ac_file)
        super().__init__(root=root, **kwargs)
        self.__autoconf(ac_path, kwargs)
        try:
            self.__gnulib_cache(kwargs)
        except FileNotFoundError:
            if gnulib_cache:
                raise
        try:
            self.__gnulib_comp(kwargs)
        except FileNotFoundError:
            if gnulib_comp:
                raise


    def __autoconf(self, ac_path, explicit):
        with _codecs.open(ac_path, "rb", "UTF-8") as stream:
            data = CachedConfig.__COMMENTS.sub("", stream.read())
        for (key, pattern) in CachedConfig.__AUTOCONF.items():
            match = pattern.findall(data)
            if match and key not in explicit:
                self[key] = match[-1]

    def __gnulib_cache(self, explicit):
        expected_m4_base = self.m4_base
        path = _os.path.join(self.root, self.m4_base, "gnulib-cache.m4")
        if not _os.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs.open(path, "rb", "UTF-8") as stream:
            data = CachedConfig.__COMMENTS.sub("", stream.read())
        for (key, (typeid, pattern)) in CachedConfig.__GNULIB_CACHE.items():
            match = pattern.findall(data)
            if match and key not in explicit:
                if key == "licenses":
                    self[key] = {
                        "2": LGPLv2_LICENSE,
                        "3": LGPLv3_LICENSE,
                        "yes": LGPL_LICENSES,
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
            data = CachedConfig.__COMMENTS.sub("", stream.read())
        pattern = _compile(r"AC_DEFUN\(\[{0}_FILE_LIST\], \[(.*?)\]\)".format(macro_prefix))
        match = pattern.findall(data)
        if match and "files" not in explicit:
            self.files = [_.strip() for _ in match[-1].split("\n") if _.strip()]
