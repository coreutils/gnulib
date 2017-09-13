#!/usr/bin/python
# encoding: UTF-8



import codecs as _codecs_
import collections as _collections_
import os as _os_
import re as _re_


from .error import type_assert as _type_assert_
from .error import AutoconfVersionError as _AutoconfVersionError_



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
        "lgpl"              : 0,
        "tests"             : False,
        "obsolete"          : False,
        "cxx_tests"         : False,
        "longrunning_tests" : False,
        "privileged_tests"  : False,
        "unportable_tests"  : False,
        "all_tests"         : False,
        "libtool"           : False,
        "conddeps"          : False,
        "vc_files"          : False,
        "autoconf"          : 2.59,
        "modules"           : set(),
        "avoid"             : set(),
        "files"             : set(),
    }


    def __init__(self, **kwargs):
        self.__table = dict(Base._TABLE_)
        for (key, value) in kwargs.items():
            self[key] = value


    def __repr__(self):
        module = self.__class__.__module__
        name = self.__class__.__name__
        return "{0}.{1}{2}".format(module, name, repr(self.__table))


    def __iter__(self):
        for key in Base._TABLE_:
            value = self[key]
            yield key, value


    @property
    def root(self):
        """target directory"""
        return self["root"]

    @root.setter
    def root(self, value):
        self["root"] = value


    @property
    def local(self):
        """local override directory"""
        return self["local"]

    @local.setter
    def local(self, value):
        self["local"] = value


    @property
    def source_base(self):
        """directory relative to ROOT where source code is placed; defaults to 'lib'"""
        return self["source_base"]

    @source_base.setter
    def source_base(self, value):
        self["source_base"] = value


    @property
    def m4_base(self):
        """directory relative to ROOT where *.m4 macros are placed; defaults to 'm4'"""
        return self["m4_base"]

    @m4_base.setter
    def m4_base(self, value):
        self["m4_base"] = value


    @property
    def po_base(self):
        """directory relative to ROOT where *.po files are placed; defaults to 'po'"""
        return self["po_base"]

    @po_base.setter
    def po_base(self, value):
        self["po_base"] = value


    @property
    def doc_base(self):
        """directory relative to ROOT where doc files are placed; defaults to 'doc'"""
        return self["doc_base"]

    @doc_base.setter
    def doc_base(self, value):
        self["doc_base"] = value


    @property
    def tests_base(self):
        """directory relative to ROOT where unit tests are placed; defaults to 'tests'"""
        return self["tests_base"]

    @tests_base.setter
    def tests_base(self, value):
        self["tests_base"] = value


    @property
    def auxdir(self):
        """directory relative to ROOT where auxiliary build tools are placed"""
        return self["auxdir"]

    @auxdir.setter
    def auxdir(self, value):
        self["auxdir"] = value


    @property
    def lib(self):
        """library name; defaults to 'libgnu'"""
        return self["lib"]

    @lib.setter
    def lib(self, value):
        self["lib"] = value


    @property
    def makefile_name(self):
        """name of makefile in automake syntax in the source-base and tests-base directories"""
        return self["makefile_name"]

    @makefile_name.setter
    def makefile_name(self, value):
        self["makefile_name"] = value


    @property
    def macro_prefix(self):
        """
        the prefix of the macros 'gl_EARLY' and 'gl_INIT' (default is 'gl');
        the change of this parameter also affects include_guard_prefix parameter
        """
        return self["macro_prefix"]

    @macro_prefix.setter
    def macro_prefix(self, value):
        self["macro_prefix"] = value


    @property
    def po_domain(self):
        """the prefix of the i18n domain"""
        return self["po_domain"]

    @po_domain.setter
    def po_domain(self, value):
        self["po_domain"] = value


    @property
    def witness_c_macro(self):
        """the C macro that is defined when the sources are compiled or used"""
        return self["witness_c_macro"]

    @witness_c_macro.setter
    def witness_c_macro(self, value):
        self["witness_c_macro"] = value


    @property
    def lgpl(self):
        """abort if modules aren't available under the LGPL; also modify license template"""
        return self["lgpl"]

    @lgpl.setter
    def lgpl(self, value):
        self["lgpl"] = value


    @property
    def tests(self):
        """include unit tests for the included modules"""
        return self["tests"]

    @tests.setter
    def tests(self, value):
        self["tests"] = value


    @property
    def obsolete(self):
        """include obsolete modules when they occur among the modules"""
        return self["obsolete"]

    @obsolete.setter
    def obsolete(self, value):
        self["obsolete"] = value


    @property
    def cxx_tests(self):
        """include even unit tests for C++ interoperability"""
        return self["cxx_tests"]

    @cxx_tests.setter
    def cxx_tests(self, value):
        self["cxx_tests"] = value


    @property
    def longrunning_tests(self):
        """include even unit tests that are long-runners"""
        return self["longrunning_tests"]

    @longrunning_tests.setter
    def longrunning_tests(self, value):
        self["longrunning_tests"] = value


    @property
    def privileged_tests(self):
        """include even unit tests that require root privileges"""
        return self["privileged_tests"]

    @privileged_tests.setter
    def privileged_tests(self, value):
        self["privileged_tests"] = value


    @property
    def unportable_tests(self):
        """include even unit tests that fail on some platforms"""
        return self["unportable_tests"]

    @unportable_tests.setter
    def unportable_tests(self, value):
        self["unportable_tests"] = value


    @property
    def all_tests(self):
        """include all kinds of problematic unit tests"""
        result = True
        result &= self.tests
        result &= self.cxx_tests
        result &= self.privileged_tests
        result &= self.unportable_tests
        result &= self.longrunning_tests
        return result

    @all_tests.setter
    def all_tests(self, value):
        self.tests = value
        self.cxx_tests = value
        self.privileged_tests = value
        self.unportable_tests = value
        self.longrunning_tests = value


    @property
    def libtool(self):
        """use libtool rules"""
        return self["libtool"]

    @libtool.setter
    def libtool(self, value):
        self["libtool"] = value


    @property
    def conddeps(self):
        """support conditional dependencies (may save configure time and object code)"""
        return self["conddeps"]

    @conddeps.setter
    def conddeps(self, value):
        self["conddeps"] = value


    @property
    def vc_files(self):
        """update version control related files"""
        return self["vc_files"]

    @vc_files.setter
    def vc_files(self, value):
        self["vc_files"] = value


    @property
    def autoconf(self):
        """autoconf version"""
        return self["autoconf"]

    @autoconf.setter
    def autoconf(self, value):
        self["autoconf"] = value


    @property
    def modules(self):
        """list of modules"""
        return self["modules"]

    @modules.setter
    def modules(self, value):
        self["modules"] = set(value)


    @property
    def avoid(self):
        """list of modules to avoid"""
        return self["avoid"]

    @avoid.setter
    def avoid(self, value):
        self["avoid"] = set(value)


    @property
    def files(self):
        """list of files to be processed"""
        return self["files"]

    @files.setter
    def files(self, value):
        self["files"] = set(value)


    @property
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self["macro_prefix"].upper()
        default = Base._TABLE_["macro_prefix"]
        return "GL_%s" % prefix if prefix == default else "GL"


    def __getitem__(self, key):
        if key not in Base._TABLE_:
            key = key.replace("-", "_")
            if key not in Base._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        if key == "all_tests":
            return self.all_tests
        return self.__table[key]


    def __setitem__(self, key, value):
        if key not in Base._TABLE_:
            key = key.replace("_", "-")
            if key not in Base._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        key = key.replace("-", "_")
        if key == "all_tests":
            self.all_tests = value
            return

        typeid = type(Base._TABLE_[key])
        if key == "lgpl" and value is None:
            value = 0
        elif key in ("modules", "avoid", "files"):
            typeid = _collections_.Iterable
        _type_assert_(key, value, typeid)

        if key == "autoconf" and value < 2.59:
            raise _AutoconfVersionError_(2.59)
        elif key in ("modules", "avoid", "files"):
            seq = list()
            item_key = "file" if key == "files" else "module"
            for item_value in value:
                _type_assert_(item_key, item_value, str)
                seq += [item_value]
            value = set(seq)
        elif key == "lgpl" and value not in (0, 2, 3):
            raise ValueError("lgpl: None, 2 or 3 expected")

        self.__table[key] = value


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
    _AUTOCONF_ = {
        "autoconf" : _re_.compile(r"AC_PREREQ\(\[(.*?)\]\)", _re_.S | _re_.M),
        "auxdir"   : _re_.compile(r"AC_CONFIG_AUX_DIR\(\[(.*?)\]\)$", _re_.S | _re_.M),
        "libtool"  : _re_.compile(r"A[CM]_PROG_LIBTOOL", _re_.S | _re_.M)
    }
    _GNULIB_CACHE_ = {
        "local"             : (str, "gl_LOCAL_DIR"),
        "libtool"           : (bool, "gl_LIBTOOL"),
        "conddeps"          : (bool, "gl_CONDITIONAL_DEPENDENCIES"),
        "vc_files"          : (bool, "gl_VC_FILES"),
        "tests"             : (bool, "gl_WITH_TESTS"),
        "obsolete"          : (bool, "gl_WITH_OBSOLETE"),
        "cxx_tests"         : (bool, "gl_WITH_CXX_TESTS"),
        "longrunning_tests" : (bool, "gl_WITH_LONGRUNNING_TESTS"),
        "privileged_tests"  : (bool, "gl_WITH_PRIVILEGED_TESTS"),
        "unportable_tests"  : (bool, "gl_WITH_UNPORTABLE_TESTS"),
        "all_tests"         : (bool, "gl_WITH_ALL_TESTS"),
        "source_base"       : (str, "gl_SOURCE_BASE"),
        "m4_base"           : (str, "gl_M4_BASE"),
        "po_base"           : (str, "gl_PO_BASE"),
        "doc_base"          : (str, "gl_DOC_BASE"),
        "tests_base"        : (str, "gl_TESTS_BASE"),
        "makefile_name"     : (str, "gl_MAKEFILE_NAME"),
        "macro_prefix"      : (str, "gl_MACRO_PREFIX"),
        "po_domain"         : (str, "gl_PO_DOMAIN"),
        "witness_c_macro"   : (str, "gl_WITNESS_C_MACRO"),
        "lib"               : (str, "gl_LIB"),
        "modules"           : (list, "gl_MODULES"),
        "avoid"             : (list, "gl_AVOID"),
        "lgpl"              : (str, "gl_LGPL"),
    }
    _GNULIB_CACHE_BOOL_ = []
    _GNULIB_CACHE_STR_ = []
    _GNULIB_CACHE_LIST_ = []
    for (_key_, (_typeid_, _)) in _GNULIB_CACHE_.items():
        if _typeid_ is bool:
            _GNULIB_CACHE_BOOL_ += [_key_]
        elif _typeid_ is str:
            _GNULIB_CACHE_STR_ += [_key_]
        else:
            _GNULIB_CACHE_LIST_ += [_key_]
    _GNULIB_CACHE_PATTERN_ = _re_.compile(r"^(gl_.*?)\(\[(.*?)\]\)$", _re_.S | _re_.M)


    def __init__(self, configure=None, **kwargs):
        super().__init__(**kwargs)
        if configure is None:
            configure = _os_.path.join(self.root, "configure.ac")
            if not _os_.path.exists(configure):
                configure = _os_.path.join(self.root, "configure.in")
        if not _os_.path.isabs(configure):
            configure = _os_.path.join(self.root, configure)
        self.__autoconf(_os_.path.normpath(configure))
        self.__gnulib_cache()
        self.__gnulib_comp()

    def __autoconf(self, configure):
        with _codecs_.open(configure, "rb", "UTF-8") as stream:
            data = stream.read()
        for (key, pattern) in Cache._AUTOCONF_.items():
            match = pattern.findall(data)
            if not match:
                continue
            if key == "autoconf":
                self[key] = float([_ for _ in match if match][-1])
            else:
                self[key] = match[-1]

    def __gnulib_cache(self):
        path = _os_.path.join(self.root, self.m4_base, "gnulib-cache.m4")
        path = _os_.path.normpath(path)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs_.open(path, "rb", "UTF-8") as stream:
            data = stream.read()
        for key in Cache._GNULIB_CACHE_BOOL_:
            (_, macro) = Cache._GNULIB_CACHE_[key]
            if key in data:
                self[key] = True
        match = dict(Cache._GNULIB_CACHE_PATTERN_.findall(data))
        for key in Cache._GNULIB_CACHE_STR_:
            (_, macro) = Cache._GNULIB_CACHE_[key]
            if macro in match:
                self[key] = match[macro].strip()
        for key in Cache._GNULIB_CACHE_LIST_:
            (_, macro) = Cache._GNULIB_CACHE_[key]
            if macro in match:
                self[key] = [_.strip() for _ in match[macro].split("\n") if _.strip()]

    def __gnulib_comp(self):
        path = _os_.path.join(self.root, self.m4_base, "gnulib-comp.m4")
        path = _os_.path.normpath(path)
        if not _os_.path.exists(path):
            raise FileNotFoundError(path)
        with _codecs_.open(path, "rb", "UTF-8") as stream:
            data = stream.read()
        regex = r"AC_DEFUN\(\[{0}_FILE_LIST\], \[(.*?)\]\)".format(self["macro-prefix"])
        pattern = _re_.compile(regex, _re_.S | _re_.M)
        match = pattern.findall(data)
        if match:
            self.files = [_.strip() for _ in match[-1].split("\n") if _.strip()]
