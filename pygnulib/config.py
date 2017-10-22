#!/usr/bin/python
# encoding: UTF-8
"""gnulib configuration API"""



import codecs as _codecs_
import collections as _collections_
import os as _os_
import re as _re_


from .error import type_assert as _type_assert_
from .error import AutoconfVersionError as _AutoconfVersionError_
from .error import M4BaseMismatchError as _M4BaseMismatchError_



def _regex_(regex):
    return _re_.compile(regex, _re_.S | _re_.M)


_ITERABLES_ = (list, tuple, set, frozenset)


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



class Base:
    """gnulib generic configuration"""
    _TABLE_ = {
        "root"              : ".",
        "local"             : "",
        "source_base"       : "lib",
        "m4_base"           : "m4",
        "po_base"           : "po",
        "doc_base"          : "doc",
        "tests_base"        : "tests",
        "auxdir"            : "",
        "lib"               : "libgnu",
        "makefile_name"     : "Makefile.am",
        "macro_prefix"      : "gl",
        "po_domain"         : "",
        "witness_c_macro"   : "",
        "licenses"          : set(),
        "libtool"           : False,
        "conddeps"          : False,
        "vc_files"          : False,
        "ac_version"        : 2.59,
        "modules"           : set(),
        "avoid"             : set(),
        "files"             : set(),
        "copymode"          : None,
        "local_copymode"    : None,
    }
    _OPTIONS_ = frozenset({
        "tests",
        "obsolete",
        "cxx_tests",
        "longrunning_tests",
        "privileged_tests",
        "unportable_tests",
    })


    class _Option_:
        """gnulib configuration options"""
        Obsolete = (1 << 0)
        Tests = (1 << 1)
        CXX = (1 << 2)
        Longrunning = (1 << 3)
        Privileged = (1 << 4)
        Unportable = (1 << 5)
        Copyrights = (1 << 6)
        AllTests = (Obsolete | Tests | CXX | Longrunning | Privileged | Unportable)


    def __init__(self, **kwargs):
        self.__table = {"options": 0}
        for (key, value) in Base._TABLE_.items():
            self[key] = kwargs.get(key, value)


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{}.{}{}".format(module, name, repr(self.__table))


    def __iter__(self):
        return iter(self.__table)


    @property
    def root(self):
        """target directory"""
        return self.__table["root"]

    @root.setter
    def root(self, value):
        _type_assert_("root", value, str)
        if not value:
            raise ValueError("non-empty path not allowed")
        self.__table["root"] = _os_.path.normpath(value)


    @property
    def local(self):
        """local override directory"""
        return self.__table["local"]

    @local.setter
    def local(self, value):
        _type_assert_("local", value, str)
        self.__table["local"] = _os_.path.normpath(value) if value else ""


    @property
    def source_base(self):
        """directory relative to ROOT where source code is placed; defaults to 'lib'"""
        return self.__table["source_base"]

    @source_base.setter
    def source_base(self, value):
        _type_assert_("source_base", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("source_base cannot be an absolute path")
        self.__table["source_base"] = _os_.path.normpath(value) if value else "lib"


    @property
    def m4_base(self):
        """directory relative to ROOT where *.m4 macros are placed; defaults to 'm4'"""
        return self.__table["m4_base"]

    @m4_base.setter
    def m4_base(self, value):
        _type_assert_("m4_base", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("m4_base cannot be an absolute path")
        self.__table["m4_base"] = _os_.path.normpath(value) if value else "m4"


    @property
    def po_base(self):
        """directory relative to ROOT where *.po files are placed; defaults to 'po'"""
        return self.__table["po_base"]

    @po_base.setter
    def po_base(self, value):
        _type_assert_("po_base", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("po_base cannot be an absolute path")
        self.__table["po_base"] = _os_.path.normpath(value) if value else "po"


    @property
    def doc_base(self):
        """directory relative to ROOT where doc files are placed; defaults to 'doc'"""
        return self.__table["doc_base"]

    @doc_base.setter
    def doc_base(self, value):
        _type_assert_("doc_base", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("doc_base cannot be an absolute path")
        self.__table["doc_base"] = _os_.path.normpath(value) if value else "doc"


    @property
    def tests_base(self):
        """directory relative to ROOT where unit tests are placed; defaults to 'tests'"""
        return self.__table["tests_base"]

    @tests_base.setter
    def tests_base(self, value):
        _type_assert_("tests_base", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("tests_base cannot be an absolute path")
        self.__table["tests_base"] = _os_.path.normpath(value) if value else "tests"


    @property
    def auxdir(self):
        """directory relative to ROOT where auxiliary build tools are placed"""
        return self.__table["auxdir"]

    @auxdir.setter
    def auxdir(self, value):
        _type_assert_("auxdir", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("auxdir cannot be an absolute path")
        self.__table["auxdir"] = _os_.path.normpath(value) if value else "build-aux"


    @property
    def lib(self):
        """library name; defaults to 'libgnu'"""
        return self.__table["lib"]

    @lib.setter
    def lib(self, value):
        _type_assert_("lib", value, str)
        self.__table["lib"] = value if value else "libgnu"


    @property
    def makefile_name(self):
        """name of makefile in automake syntax in the source-base and tests-base directories"""
        return self.__table["makefile_name"]

    @makefile_name.setter
    def makefile_name(self, value):
        _type_assert_("makefile_name", value, str)
        value = _os_.path.normpath(value)
        if _os_.path.isabs(value):
            return ValueError("makefile_name cannot be an absolute path")
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
        _type_assert_("macro_prefix", value, str)
        self.__table["macro_prefix"] = value


    @property
    def po_domain(self):
        """the prefix of the i18n domain"""
        return self.__table["po_domain"]

    @po_domain.setter
    def po_domain(self, value):
        _type_assert_("po_base", value, str)
        self.__table["po_base"] = value


    @property
    def witness_c_macro(self):
        """the C macro that is defined when the sources are compiled or used"""
        return self.__table["witness_c_macro"]

    @witness_c_macro.setter
    def witness_c_macro(self, value):
        _type_assert_("witness_c_macro", value, str)
        self.__table["witness_c_macro"] = value


    @property
    def licenses(self):
        """abort if modules aren't available under the LGPL; also modify license template"""
        return self.__table["licenses"]

    @licenses.setter
    def licenses(self, value):
        _type_assert_("licenses", value, _ITERABLES_)
        result = set()
        for item in value:
            _type_assert_("license", item, str)
            result.add(item)
        self.__table["licenses"] = frozenset(result)


    @property
    def tests(self):
        """include unit tests for the included modules"""
        return bool(self.__table["options"] & Base._Option_.Tests)

    @tests.setter
    def tests(self, value):
        _type_assert_("tests", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Tests
        else:
            self.__table["options"] &= ~Base._Option_.Tests


    @property
    def obsolete(self):
        """include obsolete modules when they occur among the modules"""
        return bool(self.__table["options"] & Base._Option_.Tests)

    @obsolete.setter
    def obsolete(self, value):
        _type_assert_("obsolete", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Obsolete
        else:
            self.__table["options"] &= ~Base._Option_.Obsolete


    @property
    def cxx_tests(self):
        """include even unit tests for C++ interoperability"""
        return bool(self.__table["options"] & Base._Option_.CXX)

    @cxx_tests.setter
    def cxx_tests(self, value):
        _type_assert_("cxx_tests", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.CXX
        else:
            self.__table["options"] &= ~Base._Option_.CXX


    @property
    def longrunning_tests(self):
        """include even unit tests that are long-runners"""
        return bool(self.__table["options"] & Base._Option_.Longrunning)

    @longrunning_tests.setter
    def longrunning_tests(self, value):
        _type_assert_("longrunning_tests", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Longrunning
        else:
            self.__table["options"] &= ~Base._Option_.Longrunning


    @property
    def privileged_tests(self):
        """include even unit tests that require root privileges"""
        return bool(self.__table["options"] & Base._Option_.Privileged)

    @privileged_tests.setter
    def privileged_tests(self, value):
        _type_assert_("privileged_tests", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Privileged
        else:
            self.__table["options"] &= ~Base._Option_.Privileged


    @property
    def unportable_tests(self):
        """include even unit tests that fail on some platforms"""
        return bool(self.__table["options"] & Base._Option_.Unportable)

    @unportable_tests.setter
    def unportable_tests(self, value):
        _type_assert_("unportable_tests", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Unportable
        else:
            self.__table["options"] &= ~Base._Option_.Unportable


    @property
    def all_tests(self):
        """include all kinds of problematic unit tests"""
        return (self.__table["options"] & Base._Option_.AllTests) == Base._Option_.AllTests

    @all_tests.setter
    def all_tests(self, value):
        if value:
            self.__table["options"] |= Base._Option_.AllTests
        else:
            self.__table["options"] &= Base._Option_.AllTests


    @property
    def libtool(self):
        """use libtool rules"""
        return self.__table["libtool"]

    @libtool.setter
    def libtool(self, value):
        _type_assert_("libtool", value, bool)
        self.__table["libtool"] = value


    @property
    def conddeps(self):
        """support conditional dependencies (may save configure time and object code)"""
        return self.__table["conddeps"]

    @conddeps.setter
    def conddeps(self, value):
        _type_assert_("conddeps", value, bool)
        self.__table["conddeps"] = value


    @property
    def vc_files(self):
        """update version control related files"""
        return self.__table["vc_files"]

    @vc_files.setter
    def vc_files(self, value):
        _type_assert_("vc_files", value, bool)
        self.__table["vc_files"] = value


    @property
    def ac_version(self):
        """minimal supported autoconf version"""
        return self.__table["ac_version"]

    @ac_version.setter
    def ac_version(self, value):
        if isinstance(value, str):
            value = float(value)
        _type_assert_("ac_version", value, float)
        if value < 2.59:
            raise _AutoconfVersionError_(2.59)
        self.__table["ac_version"] = value


    @property
    def modules(self):
        """list of modules"""
        return self.__table["modules"]

    @modules.setter
    def modules(self, value):
        _type_assert_("modules", value, _ITERABLES_)
        result = set()
        for item in value:
            _type_assert_("module", item, str)
            result.add(item)
        self.__table["modules"] = frozenset(result)


    @property
    def avoid(self):
        """list of modules to avoid"""
        return self.__table["avoid"]

    @avoid.setter
    def avoid(self, value):
        _type_assert_("avoid", value, _ITERABLES_)
        result = set()
        for item in value:
            _type_assert_("avoid", item, str)
            result.add(item)
        self.__table["avoid"] = frozenset(result)


    @property
    def files(self):
        """list of files to be processed"""
        return self.__table["files"]

    @files.setter
    def files(self, value):
        _type_assert_("files", value, _ITERABLES_)
        result = set()
        for item in value:
            _type_assert_("file", item, str)
            result.add(item)
        self.__table["files"] = frozenset(result)


    @property
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self.__table["macro_prefix"].upper()
        default = Base._TABLE_["macro_prefix"].upper()
        return "GL_{0}".format(prefix) if prefix == default else "GL"


    @property
    def copymode(self):
        return self.__table["copymode"]

    @copymode.setter
    def copymode(self, value):
        if value not in frozenset({None, "symlink", "hardlink"}):
            raise ValueError("copymode: None, 'symlink' or 'hardlink'")
        self.__table["copymode"] = value


    @property
    def local_copymode(self):
        return self.__table["local_copymode"]

    @local_copymode.setter
    def local_copymode(self, value):
        if value not in frozenset({None, "symlink", "hardlink"}):
            raise ValueError("local_copymode: None, 'symlink' or 'hardlink'")
        self.__table["local_copymode"] = value


    @property
    def copyrights(self):
        return bool(self.__table["options"] & Base._Option_.Copyrights)

    @copyrights.setter
    def copyrights(self, value):
        _type_assert_("copyrights", value, bool)
        if value:
            self.__table["options"] |= Base._Option_.Copyrights
        else:
            self.__table["options"] &= ~Base._Option_.Copyrights


    def __getitem__(self, key):
        table = (set(Base._TABLE_.keys()) | Base._OPTIONS_)
        if key not in table:
            key = key.replace("-", "_")
            if key not in table:
                raise KeyError("unsupported option: {0}".format(key))
        return getattr(self, key)


    def __setitem__(self, key, value):
        table = (set(Base._TABLE_.keys()) | Base._OPTIONS_)
        if key not in table:
            key = key.replace("_", "-")
            if key not in table:
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



class Cache(Base):
    """gnulib cached configuration"""
    _COMMENTS_ = _regex_(r"((?:(?:#)|(?:^dnl\s+)|(?:\s+dnl\s+)).*?)$")
    _AUTOCONF_ = {
        "ac_version" : _regex_(r"AC_PREREQ\(\[(.*?)\]\)"),
        "auxdir"   : _regex_(r"AC_CONFIG_AUX_DIR\(\[(.*?)\]\)$"),
        "libtool"  : _regex_(r"A[CM]_PROG_LIBTOOL")
    }
    _GNULIB_CACHE_ = {
        "local"             : (str, _regex_(r"gl_LOCAL_DIR\(\[(.*?)\]\)")),
        "libtool"           : (bool, _regex_(r"gl_LIBTOOL\(\[(.*?)\]\)")),
        "conddeps"          : (bool, _regex_(r"gl_CONDITIONAL_DEPENDENCIES\(\[(.*?)\]\)")),
        "vc_files"          : (bool, _regex_(r"gl_VC_FILES\(\[(.*?)\]\)")),
        "tests"             : (bool, _regex_(r"gl_WITH_TESTS\(\[(.*?)\]\)")),
        "obsolete"          : (bool, _regex_(r"gl_WITH_OBSOLETE\(\[(.*?)\]\)")),
        "cxx_tests"         : (bool, _regex_(r"gl_WITH_CXX_TESTS\(\[(.*?)\]\)")),
        "longrunning_tests" : (bool, _regex_(r"gl_WITH_LONGRUNNING_TESTS\(\[(.*?)\]\)")),
        "privileged_tests"  : (bool, _regex_(r"gl_WITH_PRIVILEGED_TESTS\(\[(.*?)\]\)")),
        "unportable_tests"  : (bool, _regex_(r"gl_WITH_UNPORTABLE_TESTS\(\[(.*?)\]\)")),
        "all_tests"         : (bool, _regex_(r"gl_WITH_ALL_TESTS\(\[(.*?)\]\)")),
        "source_base"       : (str, _regex_(r"gl_SOURCE_BASE\(\[(.*?)\]\)")),
        "m4_base"           : (str, _regex_(r"gl_M4_BASE\(\[(.*?)\]\)")),
        "po_base"           : (str, _regex_(r"gl_PO_BASE\(\[(.*?)\]\)")),
        "doc_base"          : (str, _regex_(r"gl_DOC_BASE\(\[(.*?)\]\)")),
        "tests_base"        : (str, _regex_(r"gl_TESTS_BASE\(\[(.*?)\]\)")),
        "makefile_name"     : (str, _regex_(r"gl_MAKEFILE_NAME\(\[(.*?)\]\)")),
        "macro_prefix"      : (str, _regex_(r"gl_MACRO_PREFIX\(\[(.*?)\]\)")),
        "po_domain"         : (str, _regex_(r"gl_PO_DOMAIN\(\[(.*?)\]\)")),
        "witness_c_macro"   : (str, _regex_(r"gl_WITNESS_C_MACRO\(\[(.*?)\]\)")),
        "lib"               : (str, _regex_(r"gl_LIB\(\[(.*?)\]\)")),
        "modules"           : (list, _regex_(r"gl_MODULES\(\[(.*?)\]\)")),
        "avoid"             : (list, _regex_(r"gl_AVOID\(\[(.*?)\]\)")),
        "licenses"          : (str, _regex_(r"gl_LGPL\(\[(.*?)\]\)")),
    }


    def __init__(self, configure=None, **kwargs):
        super().__init__(**kwargs)
        if configure is None:
            configure = _os_.path.join(self.root, "configure.ac")
            if not _os_.path.exists(configure):
                configure = _os_.path.join(self.root, "configure.in")
        if not _os_.path.isabs(configure):
            configure = _os_.path.join(self.root, configure)
        kwargs = {"configure": _os_.path.normpath(configure), "explicit": tuple(kwargs.keys())}
        calls = (self.__configure_ac, self.__gnulib_cache, self.__gnulib_comp)
        for call in calls:
            try:
                call(**kwargs)
            except FileNotFoundError:
                pass # ignore non-existent files


    def __configure_ac(self, configure, explicit, **kwargs):
        with _codecs_.open(configure, "rb", "UTF-8") as stream:
            data = Cache._COMMENTS_.sub("", stream.read())
        for (key, pattern) in Cache._AUTOCONF_.items():
            match = pattern.findall(data)
            if match and key not in explicit:
                self[key] = match[-1]

    def __gnulib_cache(self, explicit, **kwargs):
        m4_base = self.m4_base
        path = _os_.path.join(self.root, self.m4_base, "gnulib-cache.m4")
        path = _os_.path.normpath(path)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs_.open(path, "rb", "UTF-8") as stream:
            data = Cache._COMMENTS_.sub("", stream.read())
        for (key, (typeid, pattern)) in Cache._GNULIB_CACHE_.items():
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
                    self[key] = [_.strip() for _ in match[-1].split("\n") if _.strip()]
        if m4_base != self.m4_base:
            raise _M4BaseMismatchError_(path, m4_base, self.m4_base)

    def __gnulib_comp(self, explicit, **kwargs):
        macro_prefix = self.macro_prefix
        path = _os_.path.join(self.root, self.m4_base, "gnulib-comp.m4")
        path = _os_.path.normpath(path)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs_.open(path, "rb", "UTF-8") as stream:
            data = Cache._COMMENTS_.sub("", stream.read())
        pattern = _regex_(r"AC_DEFUN\(\[{0}_FILE_LIST\], \[(.*?)\]\)".format(macro_prefix))
        match = pattern.findall(data)
        if match and "files" not in explicit:
            self.files = [_.strip() for _ in match[-1].split("\n") if _.strip()]
