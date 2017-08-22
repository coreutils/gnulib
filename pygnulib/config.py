#!/usr/bin/python
# encoding: UTF-8


import codecs
import os
import re


class Config:
    """The most basic gnulib configuration holder"""
    _TABLE_ = {
        "gnulib"            : "",
        "root"              : "",
        "local_dir"         : "",
        "source_base"       : "lib",
        "m4_base"           : "m4",
        "po_base"           : "po",
        "doc_base"          : "doc",
        "tests_base"        : "tests",
        "aux_dir"           : "",
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


    def __repr__(self):
        return repr(self.__dict__["_table_"])


    def __init__(self, **kwargs):
        self.__dict__["_table_"] = dict()
        for key in Config._TABLE_:
            self.__dict__["_table_"][key] = Config._TABLE_[key]
        for key, value in kwargs.items():
            self[key] = value


    def __setattr__(self, key, value):
        self[key] = value
        self.__dict__[key] = value


    def __getattr__(self, key):
        return self[key]


    def __getitem__(self, key):
        if key not in Config._TABLE_:
            key = key.replace("_", "-")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: '%s'" % key)
        return self.__dict__["_table_"][key]


    def __setitem__(self, key, value):
        if key not in Config._TABLE_:
            key = key.replace("_", "-")
            if key not in Config._TABLE_:
                raise KeyError("unsupported option: '%s'" % key)

        typeid = type(Config._TABLE_[key])
        if key == "lgpl":
            if value not in [None, 2, 3]:
                raise TypeError("lgpl option must be either None or integral version (2 or 3)")
        elif key == "autoconf":
            if value < 2.59:
                raise NotImplementedError("pygnulib ")
        elif not isinstance(value, typeid):
            raise TypeError("'%s' option must be of '%s' type" % (key, typeid))

        tests = ["tests", "cxx-tests", "longrunning-tests", "privileged-tests", "unportable-tests"]
        if key == "all-tests":
            for _ in tests:
                self.__dict__["_table_"][_] = value
        else:
            self.__dict__["_table_"][key] = value



class CachedConfig(Config):
    """Cached configuration holder"""
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
        super(CachedConfig, self).__init__(**kwargs)
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
        for key, pattern in CachedConfig._AUTOCONF_.items():
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
            for key in CachedConfig._GNULIB_CACHE_BOOL_:
                (_, macro) = CachedConfig._GNULIB_CACHE_[key]
                if key in data:
                    self[key] = True
            match = dict(CachedConfig._GNULIB_CACHE_PATTERN_.findall(data))
            for key in CachedConfig._GNULIB_CACHE_STR_:
                (_, macro) = CachedConfig._GNULIB_CACHE_[key]
                if macro in match:
                    self[key] = match[macro].strip()
            for key in CachedConfig._GNULIB_CACHE_LIST_:
                (_, macro) = CachedConfig._GNULIB_CACHE_[key]
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
