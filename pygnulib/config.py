#!/usr/bin/python
# encoding: UTF-8


import codecs
import os
import re


from .error import AutoconfVersionError


class Config:
    """gnulib generic configuration"""
    _TABLE_ = {
        "root"              : "",
        "local_dir"         : "",
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
        "cxx_tests"         : False,
        "longrunning_tests" : False,
        "privileged_tests"  : False,
        "unportable_tests"  : False,
        "all_tests"         : False,
        "libtool"           : False,
        "conddeps"          : False,
        "vc_files"          : False,
        "autoconf"          : 2.59,
    }


    def __init__(self, **kwargs):
        self._table_ = dict()
        for key in Config._TABLE_:
            self._table_[key] = Config._TABLE_[key]
        for key, value in kwargs.items():
            self[key] = value


    def __repr__(self):
        return repr(self._table_)


    def __iter__(self):
        for key in Config._TABLE_:
            value = self[key]
            yield key, value


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
        if value is None:
            value = 0
        if value not in [0, 2, 3]:
            raise TypeError("lgpl option must be either None or integral version (2 or 3)")
        self["lgpl"] = value


    @property
    def tests(self):
        """include unit tests for the included modules"""
        return self["tests"]

    @tests.setter
    def tests(self, value):
        self["tests"] = value


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
    def include_guard_prefix(self):
        """include guard prefix"""
        prefix = self["macro_prefix"].upper()
        default = Config._TABLE_["macro_prefix"]
        return "GL_%s" % prefix if prefix == default else "GL"


    def __getitem__(self, key):
        if key not in Config._TABLE_:
            key = key.replace("-", "_")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        return self._table_[key]


    def __setitem__(self, key, value):
        if key not in Config._TABLE_:
            key = key.replace("_", "-")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: %r" % key)
        key = key.replace("-", "_")
        typeid = type(Config._TABLE_[key])
        if key == "lgpl":
            if value not in [None, 2, 3]:
                raise TypeError("lgpl option must be either None or integral version (2 or 3)")
        elif key == "autoconf":
            if value < 2.59:
                raise AutoconfVersionError(2.59)
        elif not isinstance(value, typeid):
            raise TypeError("%r option must be of %r type" % (key, typeid))
        self._table_[key] = value


    def items(self):
        """a set-like object providing a view on configuration items"""
        return self._table_.items()


    def keys(self):
        """a set-like object providing a view on configuration keys"""
        return self._table_.keys()


    def values(self):
        """a set-like object providing a view on configuration values"""
        return self._table_.values()


class Cache(Config):
    """gnulib cached configuration"""
    _AUTOCONF_ = {
        "autoconf" : re.compile(".*AC_PREREQ\\(\\[(.*?)\\]\\)", re.S | re.M),
        "aux-dir"  : re.compile("^AC_CONFIG_AUX_DIR\\(\\[(.*?)\\]\\)$", re.S | re.M),
        "libtool"  : re.compile("A[CM]_PROG_LIBTOOL", re.S | re.M)
    }
    _GNULIB_CACHE_ = {
        "libtool"           : (bool, "gl_LIBTOOL"),
        "conddeps"          : (bool, "gl_CONDITIONAL_DEPENDENCIES"),
        "vc-files"          : (bool, "gl_VC_FILES"),
        "tests"             : (bool, "gl_WITH_TESTS"),
        "obsolete"          : (bool, "gl_WITH_OBSOLETE"),
        "cxx-tests"         : (bool, "gl_WITH_CXX_TESTS"),
        "longrunning-tests" : (bool, "gl_WITH_LONGRUNNING_TESTS"),
        "privileged-tests"  : (bool, "gl_WITH_PRIVILEGED_TESTS"),
        "unportable-tests"  : (bool, "gl_WITH_UNPORTABLE_TESTS"),
        "all-tests"         : (bool, "gl_WITH_ALL_TESTS"),
        "local-dir"         : (str, "gl_LOCAL_DIR"),
        "source-base"       : (str, "gl_SOURCE_BASE"),
        "m4-base"           : (str, "gl_M4_BASE"),
        "po-base"           : (str, "gl_PO_BASE"),
        "doc-base"          : (str, "gl_DOC_BASE"),
        "tests-base"        : (str, "gl_TESTS_BASE"),
        "makefile-name"     : (str, "gl_MAKEFILE_NAME"),
        "macro-prefix"      : (str, "gl_MACRO_PREFIX"),
        "po-domain"         : (str, "gl_PO_DOMAIN"),
        "witness-c-macro"   : (str, "gl_WITNESS_C_MACRO"),
        "lib"               : (str, "gl_LIB"),
        "modules"           : (list, "gl_MODULES"),
        "avoids"            : (list, "gl_AVOID"),
        "lgpl"              : (str, "gl_LGPL"),
    }
    _GNULIB_CACHE_BOOL_ = []
    _GNULIB_CACHE_STR_ = []
    _GNULIB_CACHE_LIST_ = []
    for _key_, (_typeid_, _) in _GNULIB_CACHE_.items():
        if _typeid_ is bool:
            _GNULIB_CACHE_BOOL_ += [_key_]
        elif _typeid_ is str:
            _GNULIB_CACHE_STR_ += [_key_]
        else:
            _GNULIB_CACHE_LIST_ += [_key_]
    _GNULIB_CACHE_PATTERN_ = re.compile("^(gl_.*?)\\(\\[(.*?)\\]\\)$", re.S | re.M)


    def __init__(self, root, autoconf=None, **kwargs):
        if not isinstance(root, str):
            raise TypeError("root must be of 'str' type")
        super().__init__(**kwargs)
        self._autoconf_(root, autoconf)
        self._gnulib_cache_(root)
        self._gnulib_comp_(root)

    def _autoconf_(self, root, autoconf):
        if not autoconf:
            autoconf = os.path.join(root, "configure.ac")
            if not os.path.exists(autoconf):
                autoconf = os.path.join(root, "configure.in")
        if not os.path.isabs(autoconf):
            autoconf = os.path.join(root, autoconf)
        with codecs.open(autoconf, "rb", "UTF-8") as stream:
            data = stream.read()
        for key, pattern in Cache._AUTOCONF_.items():
            match = pattern.findall(data)
            if not match:
                continue
            if key == "autoconf":
                self["autoconf"] = sorted(set([float(_.strip()) for _ in match if _.strip()]))[-1]
            else:
                self[key] = match[-1]

    def _gnulib_cache_(self, root):
        m4base = self["m4-base"]
        gnulib_cache = os.path.join(root, m4base, "gnulib-cache.m4")
        if os.path.exists(gnulib_cache):
            with codecs.open(gnulib_cache, "rb", "UTF-8") as stream:
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

    def _gnulib_comp_(self, root):
        m4base = self["m4-base"]
        gnulib_comp = os.path.join(root, m4base, "gnulib-comp.m4")
        if os.path.exists(gnulib_comp):
            with codecs.open(gnulib_comp, "rb", "UTF-8") as stream:
                data = stream.read()
            regex = "AC_DEFUN\\(\\[%s_FILE_LIST\\], \\[(.*?)\\]\\)" % self["macro-prefix"]
            pattern = re.compile(regex, re.S | re.M)
            match = pattern.findall(data)
            if match:
                self["files"] = [_.strip() for _ in match[-1].split("\n") if _.strip()]
